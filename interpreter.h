#ifndef interpreter_h
#define interpreter_h

#include <memory>
#include <unordered_map>
#include "any.h"
#include "globals.h"

namespace Node {
  struct INode;
  struct PNode;
}

namespace Stack {
  struct PStack;
}

namespace Interpreter {
  typedef std::unordered_map<std::string, Node::PNode> Items;

  struct IInterpreter {
    typedef std::unordered_map<std::string, PredefinedFunction> PredefinedFunctions;

    virtual ~IInterpreter() { }

    virtual void Check(bool condition) const = 0;
    virtual const Stack::PStack& GetItems() const = 0;
    virtual PredefinedFunctions& GetPredefinedFunctions() = 0;
    virtual const Stack::PStack& GetValues() const = 0;
    virtual any Run(const Node::PNode& tree) = 0;
    virtual const any& PopValue() = 0;
  };

  struct PInterpreter : std::shared_ptr<IInterpreter> {
    PInterpreter();
  };
}

#endif
