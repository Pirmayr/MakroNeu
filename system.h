#ifndef _system_h
#define _system_h

#ifdef _WIN32

namespace Interpreter {
  struct IInterpreter;
}

extern void InitializeSystem(Interpreter::IInterpreter& interpreter);

#endif

#endif