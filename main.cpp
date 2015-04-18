#include "stdafx.h" 
#include <codecvt>
#include <iostream>
#include <sstream>
#include <tchar.h>
#include <time.h>
#include "compiler.h"
#include "helpers.h"
#include "interpreter.h"
#include "node.h"
#include "stack.h"

const std::string RESULT_FAILURE = "failure(s)"; // 1
const bool VERBOSE = true;

using namespace Helpers;

template<typename TChar, typename TTraits = std::char_traits<TChar>> class OutputDebugStringBuf : public std::basic_stringbuf<TChar, TTraits> {
public:
  OutputDebugStringBuf() : _buffer(1024) {
    setg(nullptr, nullptr, nullptr);
    setp(&_buffer[0], &_buffer[0], &_buffer[0] + _buffer.size());
  }

  ~OutputDebugStringBuf() {
  }

  int sync() override
  {
    try {
      MessageOutputer<TChar, TTraits>()(pbase(), pptr());
      setp(&_buffer[0], &_buffer[0], &_buffer[0] + _buffer.size());
      return 0;
    }
    catch (...) {
      return -1;
    }
  }

  std::char_traits<char>::int_type overflow(std::char_traits<char>::int_type c = TTraits::eof()) override
  {
    auto syncRet = sync();
    if (c != TTraits::eof()) {
      _buffer[0] = c;
      setp(&_buffer[0], &_buffer[0] + 1, &_buffer[0] + _buffer.size());
    }
    return syncRet == -1 ? TTraits::eof() : 0;
  }

private:
  std::vector<TChar>	_buffer;

  template<typename TChar, typename TTraits> struct MessageOutputer;

  template<> struct MessageOutputer<char, std::char_traits<char>> {
    template<typename TIterator> void operator()(TIterator begin, TIterator end) const {
      std::string s(begin, end);
      OutputDebugStringA(s.c_str());
    }
  };

  template<> struct MessageOutputer<wchar_t, std::char_traits<wchar_t>> {
    template<typename TIterator>  void operator()(TIterator begin, TIterator end) const {
      std::wstring s(begin, end);
      OutputDebugStringW(s.c_str());
    }
  };
};

std::string parse(std::wstring filename) {
  static OutputDebugStringBuf<char> charDebugOutput;
  std::cout.rdbuf(&charDebugOutput);
  Compiler::PCompiler compiler;
  if (compiler->Parse(filename))
  {
    std::cout << "Parsing succeeded." << std::endl;
    std::cout << "Syntax-tree:" << std::endl;
    DumpNode(compiler->GetTree());
    Interpreter::PInterpreter interpreter;
    auto startTime = clock();
    const std::string result = interpreter->Run(compiler->GetTree());
    printf("%f seconds\n", static_cast<float>(clock() - startTime) / CLOCKS_PER_SEC);
    std::cout << "Seconds: " << static_cast<float>(clock() - startTime) / CLOCKS_PER_SEC << std::endl;
    std::cout << "Interpretation succeeded." << std::endl;
    std::cout << "Result:" << std::endl;
    std::cout << result << std::endl;
    std::cout << "Stack:" << std::endl;
    DumpItems(*interpreter->GetValues());
    std::cout << "Items:" << std::endl;
    DumpItems(*interpreter->GetItems());
    return result;
  }
  std::cout << "Parsing failed." << std::endl;
  return "";
}

int _tmain(int argc, TCHAR* argv[])
{
  if (argc == 2)
  {
    parse(argv[1]);
    getchar();
  }

  return 0;
}

