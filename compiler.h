#ifndef compiler_h
#define compiler_h

#include <memory>
#include <string>

namespace Node {
  struct PNode;
}

namespace Compiler {
  struct ICompiler {
    virtual ~ICompiler() {}

    // Common methods:
    virtual bool Parse(const std::wstring& path) = 0;
    virtual Node::PNode& GetTree() = 0;

    // Parser methods:
    virtual void DefineIdentifier(const std::string& name) = 0;
    virtual void DefineFunction() = 0;
    virtual void DefineProcedure() = 0;
    virtual void EnterLocalScope() = 0;
    virtual void LeaveLocalScope() = 0;
    virtual void Merge() = 0;
    virtual void PushNew(const std::string& tag) = 0;
    virtual void PushNewBinary(const std::string& tag) = 0;
    virtual void PushNewNumber(const std::string& tag, double value) = 0;
    virtual void PushNewString(const std::string& tag, const std::string& value) = 0;
  };

  struct PCompiler : std::unique_ptr<ICompiler> {
    PCompiler();
  };
}

#endif
