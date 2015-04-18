#ifndef GLOBALS_H
#define GLOBALS_H

namespace Interpreter
{
  struct IInterpreter;
}

namespace Node
{
  struct INode;
}

typedef unsigned long Count;
typedef void (*PredefinedFunction)(Interpreter::IInterpreter& interpreter, const Node::INode& node);

#endif