#include "stdafx.h"
#include "system.h"
#include <iostream> 
#include "any.h"
#include "helpers.h"
#include "interpreter.h"
#include "node.h"
#include "stack.h"

using namespace Helpers;
using namespace Interpreter;
using namespace Node;
using namespace Stack;
using namespace std;

#ifdef WIN32

void DoSetPixel(IInterpreter& interpreter, const INode& node)
{
  auto count = node.ChildCount();
  interpreter.Check(5 == count);
  auto dc = GetDC(nullptr);
  if (dc != nullptr)
  {
    int blue = interpreter.PopValue();
    int green = interpreter.PopValue();
    int red = interpreter.PopValue();
    int y = interpreter.PopValue();
    int x = interpreter.PopValue();
    SetPixel(dc, x, y, RGB(red, green, blue));
  }
  interpreter.GetValues()->PushNew(tagValue, 0);
}

#ifdef __cplusplus_cli

#using <mscorlib.dll>
#using <System.Windows.Forms.dll> 

using namespace System::Runtime::InteropServices;
using namespace System::Windows::Forms;

void DoMessage(IInterpreter& interpreter, const Node::INode& node)
{
  const string stringValue(interpreter.PopValue());
  MessageBox::Show("hi");
  interpreter.GetValues()->PushNew(tagValue, 0.0);
}

#else

void DoMessage(IInterpreter& interpreter, const INode& /*node*/)
{
  const string stringValue(interpreter.PopValue());
  cout << stringValue << endl;
  interpreter.GetValues()->PushNew(tagValue, 0.0);
}

#endif

void DoAbs(IInterpreter& interpreter, const INode& /*node*/)
{
  double value = interpreter.PopValue();
  auto result = fabs(value);
  interpreter.GetValues()->PushNew(tagValue, result);
}

void DoAsc(IInterpreter& interpreter, const INode& /*node*/)
{
  const string stringValue(interpreter.PopValue());
  interpreter.GetValues()->PushNew(tagValue, static_cast<double>(stringValue[0]));
}

void DoChr(IInterpreter& interpreter, const INode& /*node*/)
{
  const auto& value = interpreter.PopValue();
  double doubleValue;
  if (value.TryGet(doubleValue))
  {
    string result;
    result += static_cast<char>(doubleValue);
    interpreter.GetValues()->PushNew(tagValue, result);
  }
  else
  {
    static PNode defaultValue(tagValue, string("\0"), 0);
    interpreter.GetValues()->Push(defaultValue);        
  }
}

void DoLen(IInterpreter& interpreter, const INode& /*node*/)
{
  string value(interpreter.PopValue());
  Count length = value.length();
  interpreter.GetValues()->PushNew(tagValue, static_cast<double>(length));
}

void DoMax(IInterpreter& interpreter, const INode& node)
{
  auto count = node.ChildCount();
  interpreter.Check(2 <= count);
  double result = -DBL_MAX;
  for (Count i = 0; i < count; ++i)
  {
    double currentValue;
    interpreter.Check(interpreter.PopValue().TryGet(currentValue));
    result = max(result, currentValue);
  }
  interpreter.GetValues()->PushNew(tagValue, result);
}

void DoPi(IInterpreter& interpreter, const INode& /*node*/)
{
  static PNode pi(tagValue, 3.1415926, 0);
  interpreter.GetValues()->Push(pi);
}

void DoStr(IInterpreter& interpreter, const INode& /*node*/)
{
  string value = interpreter.PopValue();
  interpreter.GetValues()->PushNew(tagValue, value);
}

void DoString(IInterpreter& interpreter, const INode& /*node*/)
{
  string result;
  const auto& value = interpreter.PopValue();
  const auto& factor = interpreter.PopValue();
  double doubleFactor;
  if (factor.TryGet(doubleFactor))
  {
    auto countFactor = static_cast<Count>(doubleFactor);
    auto character = '\0';
    double doubleValue;
    if (value.TryGet(doubleValue))
    {
      character = static_cast<char>(doubleValue);
    }
    else
    { 
      string stringValue;
      if (value.TryGet(stringValue))
      {
        if (0 < stringValue.length())
        {
          character = stringValue[0];
        }
      }
    }
    for (Count i = 0; i < countFactor; ++i)
    {
      result += character;
    }
  }
  interpreter.GetValues()->PushNew(tagValue, result);
}

void DoVal(IInterpreter& interpreter, const INode& /*node*/)
{
  string value = interpreter.PopValue();
  interpreter.GetValues()->PushNew(tagValue, stod(value));
}

void InitializeSystem(IInterpreter& interpreter)
{
  auto& predefinedFunctions = interpreter.GetPredefinedFunctions();
  predefinedFunctions["abs"] = DoAbs;
  predefinedFunctions["asc"] = DoAsc;
  predefinedFunctions["chr$"] = DoChr;
  predefinedFunctions["len"] = DoLen;
  predefinedFunctions["max"] = DoMax;
  predefinedFunctions["message"] = DoMessage;
  predefinedFunctions["pi"] = DoPi;
  predefinedFunctions["setpixel"] = DoSetPixel;
}

#endif
