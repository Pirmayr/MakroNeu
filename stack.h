#ifndef stack_h
#define stack_h

#include "helpers.h"
// #include <boost/any.hpp>
#include <memory>

struct any;

namespace Node {
  struct PNode;
}

namespace Stack {
  struct IStack {
  protected:
    ~IStack()
    {
    }

  public:
    virtual void Pop() = 0;
    virtual void Push(const Node::PNode& node) = 0;
    virtual void PushNew(const std::string& tag, const any& value = 0, const Count offset = 0) = 0;
    virtual void PushNewBinary(const std::string& tag, const any& value = 0) = 0;
    virtual const Node::PNode& Top(Count offset = 0) const = 0;
    virtual void Merge() = 0;
    virtual Count Size() const = 0;
    virtual const Node::PNode& Get(Count index) const = 0;
    virtual void Resize(Count size) = 0;
  };

  struct PStack : std::shared_ptr<IStack> {
    PStack();
  };
}


#endif