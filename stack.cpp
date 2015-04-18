#include "stdafx.h" 
#include <vector>
#include "stack.h"
#include "node.h"

namespace Stack {
  using namespace Node;
  using namespace std;

  struct CStack : IStack {
    virtual ~CStack() {}

    vector<PNode> elements;

    void Pop() override
    {
      elements.pop_back();
    }

    void Push(const PNode& node) override
    {
      elements.push_back(node);
    }

    void PushNew(const string& tag, const any& value = 0, const Count offset = 0) override
    {
      elements.emplace_back(tag, value, offset);
    }

    void PushNewBinary(const string& tag, const any& value = 0) override
    {
      PNode newObject(tag, value, 0);
      newObject->Push(elements[elements.size() - 2]);
      newObject->Push(elements[elements.size() - 1]);
      elements.pop_back();
      elements.pop_back();
      Push(newObject);
    }

    const PNode& Top(Count offset = 0) const override
    {
      return elements[elements.size() - offset - 1];
    }

    void Merge() override
    {
      elements[elements.size() - 2]->Push(elements[elements.size() - 1]);
      elements.pop_back();
    }

    Count Size() const override
    {
      return elements.size();
    }

    const PNode& Get(Count index) const override
    {
      return elements[index];
    }

    void Resize(Count size) override
    {
      elements.resize(size);
    }
  };

  PStack::PStack(): shared_ptr<IStack>(new CStack()) {}
}

