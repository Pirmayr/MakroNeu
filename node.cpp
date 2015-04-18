#include "stdafx.h" 
#include <list>
#include <vector>
#include "any.h"
#include "node.h"
#include "stack.h"

namespace Node {
  using namespace Stack;
  // using namespace boost;
  using namespace std;

  static any defaultValue = 0.0;

  struct CNode : INode {
    string tag;
    any value;
    vector<any> values;
    any valueSelector;
    PStack children;
    HasAction hasPreAction;
    void* pPreAction;
    HasAction hasPredefinedRoutine;
    any predefinedRoutine;
    HasAction hasPostAction;
    void* pPostAction;
    IsGlobal isGlobal;
    Count localNameOffset;
    bool breakFlow;

    CNode() {
      // Initialize();
    }

    CNode(const string& tag, const any& value, int itemOffset) {
      Initialize(tag, value, itemOffset);
    }

    void Initialize() {
      this->tag = "";
      this->localNameOffset = 0;
      valueSelector = 0;
      hasPreAction = hasActionNotSet;
      hasPredefinedRoutine = hasActionNotSet;
      hasPostAction = hasActionNotSet;
      isGlobal = isGlobalNotSet;
      breakFlow = false;
    }

    void Initialize(const string& tag, const any& value, int itemOffset) {
      this->tag = tag;
      this->value = value;
      this->localNameOffset = itemOffset;
      valueSelector = 0;
      hasPreAction = hasActionNotSet;
      hasPredefinedRoutine = hasActionNotSet;
      hasPostAction = hasActionNotSet;
      isGlobal = isGlobalNotSet;
      breakFlow = false;
    }

    void PushNew(const string& tag, const any& value = 0, const Count offset = 0) override {
      children->PushNew(tag, value, offset);
    }

    void PushNewBinary(const string& tag, const any& value = 0) {
      children->PushNewBinary(tag, value);
    }

    void Push(const PNode& node) {
      children->Push(node);
    }

    void Pop() {
      children->Pop();
    }

    const PNode& Top(Count offset = 0) const {
      return children->Top(offset);
    }

    void Merge() {
      children->Merge();
    }

    Count ChildCount() const {
      return children->Size();
    }

    const PNode& GetChild(Count index) const {
      return children->Get(index);
    }

    const string& GetTag() const {
      return tag;
    }

    void SetValue(const any& newValue) {
      value = newValue;
    }

    void SetValue(const any& newValue, const any& valueSelector) {
      Count index = valueSelector;
      if (this->values.size() <= index) {
        this->values.resize(index + 1);
      }
      values[index] = newValue;
    }

    const any& GetValue() const {
      return value;
    }

    const any& GetValue(const any& valueSelector) const {
      Count index = valueSelector;
      if (this->values.size() <= index) {
        return defaultValue;
      }
      return values[index];
    }

    void SetValueSelector(const any& valueSelector) {
      this->valueSelector = valueSelector;
    }

    const any& GetValueSelector() const {
      return valueSelector;
    }

    void SetHasPreAction(HasAction hasAction) {
      hasPreAction = hasAction;
    }

    HasAction GetHasPreAction() const {
      return hasPreAction;
    }

    virtual void SetPreAction(void* pAction) {
      pPreAction = pAction;
    }

    virtual void* GetPreAction() {
      return pPreAction;
    }

    void SetHasPredefinedRoutine(HasAction hasAction) {
      hasPredefinedRoutine = hasAction;
    }

    HasAction GetHasPredefinedRoutine() const {
      return hasPredefinedRoutine;
    }

    virtual void SetPredefinedRoutine(const any& action) {
      predefinedRoutine = action;
    }

    virtual const any& GetPredefinedRoutine() const {
      return predefinedRoutine;
    }

    virtual void SetHasPostAction(HasAction hasAction) {
      hasPostAction = hasAction;
    }

    HasAction GetHasPostAction() const {
      return hasPostAction;
    }

    void SetPostAction(void* pAction) {
      pPostAction = pAction;
    }

    void* GetPostAction() {
      return pPostAction;
    }

    virtual void SetIsGlobal(IsGlobal value) {
      isGlobal = value;
    }

    virtual IsGlobal GetIsGlobal() const {
      return isGlobal;
    }

    void SetItemOffset(Count value) {
      localNameOffset = value;
    }

    Count GetItemOffset() const {
      return localNameOffset;
    }

    virtual void SetBreakFlow(bool value) {
      breakFlow = value;
    }

    virtual bool GetBreakFlow() {
      return breakFlow;
    }
  };

  static list<CNode*> objectPool;

  static CNode* NewObject() {
    if (objectPool.empty()) {
      return new CNode();
    }
    CNode* result = objectPool.front();
    objectPool.pop_front();
    result->Initialize();
    return result;
  }

  static CNode* NewObject(const string& tag, const any& value, int itemOffset) {
    if (objectPool.empty()) {
      return new CNode(tag, value, itemOffset);
    }
    CNode* result = objectPool.front();
    objectPool.pop_front();
    result->Initialize(tag, value, itemOffset);
    return result;
  }

  struct Cleanup {
    ~Cleanup() {
      for (auto currentNode: objectPool) {
        delete currentNode;
      }
      objectPool.clear();
    }
  } cleanup;

  static void DeleteObject(CNode* object) {
    objectPool.push_front(object);
  }

  PNode::PNode(): shared_ptr<INode>(NewObject(), DeleteObject) 
  {
    int a = 0;
  }
  PNode::PNode(const string& tag, const any& value, int itemOffset): shared_ptr<INode>(NewObject(tag, value, itemOffset), DeleteObject) {}
}
