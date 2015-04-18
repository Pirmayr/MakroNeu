#ifndef NODE_H
#define NODE_H

#include <memory>
#include "globals.h"

struct any;

namespace Node {
  struct PNode;

  typedef enum {
    hasActionNotSet,
    hasActionTrue,
    hasActionFalse,
  } HasAction;

  typedef enum {
    isGlobalNotSet,
    isGlobalTrue,
    isGlobalFalse,
  } IsGlobal;

  struct INode {
    virtual ~INode() {}

    virtual void PushNew(const std::string& tag, const any& value, const Count offset = 0) = 0;
    virtual void PushNewBinary(const std::string& tag, const any& value) = 0;
    virtual void Push(const PNode& node) = 0;
    virtual void Pop() = 0;
    virtual const PNode& Top(Count offset = 0) const = 0;
    virtual void Merge() = 0;
    virtual Count ChildCount() const = 0;
    virtual const PNode& GetChild(Count index) const = 0;
    virtual const std::string& GetTag() const = 0;
    virtual void SetValue(const any& newValue) = 0;
    virtual void SetValue(const any& newValue, const any& selector) = 0;
    virtual const any& GetValue() const = 0;
    virtual const any& GetValue(const any& selector) const = 0;
    virtual void SetValueSelector(const any& valueSelector) = 0;
    virtual const any& GetValueSelector() const = 0;
    virtual void SetHasPreAction(HasAction hasAction) = 0;
    virtual HasAction GetHasPreAction() const = 0;
    virtual void SetPreAction(void* pAction) = 0;
    virtual void* GetPreAction() = 0;
    virtual void SetHasPredefinedRoutine(HasAction hasAction) = 0;
    virtual HasAction GetHasPredefinedRoutine() const = 0;
    virtual void SetPredefinedRoutine(const any& action) = 0;
    virtual const any& GetPredefinedRoutine() const = 0;
    virtual void SetHasPostAction(HasAction hasAction) = 0;
    virtual HasAction GetHasPostAction() const = 0;
    virtual void SetPostAction(void* pAction) = 0;
    virtual void* GetPostAction() = 0;
    virtual void SetIsGlobal(IsGlobal value) = 0;
    virtual IsGlobal GetIsGlobal() const = 0;
    virtual void SetItemOffset(Count value) = 0;
    virtual Count GetItemOffset() const = 0;
    virtual void SetBreakFlow(bool value) = 0;
    virtual bool GetBreakFlow() = 0;
  };

  struct PNode : std::shared_ptr<INode> {
    PNode();
    PNode(const std::string& tag, const any& value, int itemOffset);
  };
}

#endif
