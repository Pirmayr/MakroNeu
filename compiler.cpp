#include "stdafx.h" 
#include <map>
#include "compiler.h"
#include "helpers.h"
#include "node.h"
#include "Scanner.h"
#include "Parser.h"

namespace Compiler
{
  using namespace std;

  struct CCompiler : ICompiler
  {
    typedef map<string, Count> ItemOffsets;

    ItemOffsets globalItemOffsets;
    ItemOffsets localItemOffsets;
    bool useLocalScope;
    Node::PNode tree;

    void DefineFunction() override
    {
      tree->PushNew(tagFunction, 0, 0);
    };

    void DefineIdentifier(const string& name) override
    {
      tree->PushNew(tagIdentifier, name, GetLocalItemOffset(name));
    }

    void DefineProcedure() override
    {
      tree->PushNew(tagProcedure, 0, 0);
    };

    void EnterLocalScope() override
    {
      localItemOffsets.clear();
      useLocalScope = true;
    };

    Count GetGlobalItemOffset(const string& name)
    {
      const auto& iterator = globalItemOffsets.find(name);
      if (iterator != globalItemOffsets.end())
      {
        return iterator->second;
      }
      return UINT_MAX ;
    }

    Count GetLocalItemOffset(const string& name)
    {
      auto result = GetGlobalItemOffset(name);
      if (result < UINT_MAX)
      {
        return result;
      }
      auto& itemOffsets = useLocalScope ? localItemOffsets : globalItemOffsets;
      const auto& iterator = itemOffsets.find(name);
      if (iterator != itemOffsets.end())
      {
        return iterator->second;
      }
      result = itemOffsets.size();
      itemOffsets[name] = result;
      return result;
    }

    Node::PNode& GetTree() override
    {
      return tree;
    }

    void LeaveLocalScope() override
    {
      useLocalScope = false;
    };

    void Merge() override
    {
      tree->Merge();
    }

    bool Parse(const wstring& path) override
    {
      useLocalScope = false;
      Scanner scanner(path.c_str());
      Parser parser(&scanner, *this);
      parser.Parse();
      return parser.errors->count == 0;
    }

    void PushNew(const string& tag) override
    {
      tree->PushNew(tag, 0);
    }

    void PushNewBinary(const string& tag) override
    {
      tree->PushNewBinary(tag, 0);
    }

    void PushNewNumber(const string& tag, double value) override
    {
      tree->PushNew(tag, value);
    }

    void PushNewString(const string& tag, const string& value) override
    {
      tree->PushNew(tag, value.substr(1, value.size() - 2));
    }
  };

  PCompiler::PCompiler() : unique_ptr<ICompiler>(new CCompiler())
  {
  }
}
