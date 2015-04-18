#ifndef ANY_H
#define ANY_H

#include <string>
#include "globals.h"
#include "node.h"

struct any
{
  enum
  {
    typeNone,
    typeInt,
    typeDouble,
    typeString,
    typeCount,
    typePNode,
    typePredefinedFunction,
  } type;

  int intValue;
  double doubleValue;
  std::string stringValue;
  Count countValue;
  const Node::PNode* pNodeValue;
  PredefinedFunction predefinedFunctionValue;
  int intResult;
  Count countResult;
  std::string stringResult;

  any()
  {
    type = typeNone;
  }

  any(const int& value)
  {
    type = typeInt;
    intValue = value;
  }

  any(const double& value)
  {
    type = typeDouble;
    doubleValue = value;
  }

  any(const std::string& value)
  {
    type = typeString;
    stringValue = value;
  }

  any(const Count& value)
  {
    type = typeCount;
    countValue = value;
  }

  any(const Node::PNode& value)
  {
    type = typePNode;
    pNodeValue = &value;
  }

  any(const PredefinedFunction& value)
  {
    type = typePredefinedFunction;
    predefinedFunctionValue = value;
  }

  any(const any& value)
  {
    *this = value;
  }

  void Assign(const std::string& a, const std::string& b) const
  {
    const_cast<std::string&>(a) = b;
  }

  void Assign(const Count& a, const int& b) const
  {
    const_cast<Count&>(a) = static_cast<Count>(b);
  }

  void Assign(const Count& a, const double& b) const
  {
    const_cast<Count&>(a) = static_cast<Count>(b);
  }

  void Assign(const int& a, const double& b) const
  {
    const_cast<int&>(a) = static_cast<int>(b);
  }

  void Assign(const int& a, const Count& b) const
  {
    const_cast<int&>(a) = static_cast<int>(b);
  }

  void operator=(const int& rhs)
  {
    type = typeInt;
    intValue = rhs;
  }

  void operator=(const double& rhs)
  {
    type = typeDouble;
    doubleValue = rhs;
  }

  void operator=(const std::string& rhs)
  {
    type = typeString;
    stringValue = rhs;
  }

  void operator=(const Count& rhs)
  {
    type = typeCount;
    countValue = rhs;
  }

  void operator=(const Node::PNode& rhs)
  {
    type = typePNode;
    pNodeValue = &rhs;
  }

  void operator=(const PredefinedFunction& rhs)
  {
    type = typePredefinedFunction;
    predefinedFunctionValue = rhs;
  }

  void operator=(const any& rhs)
  {
    switch (rhs.type)
    {
    case typeInt:
      type = typeInt;
      intValue = rhs.intValue;
      break;
    case typeDouble:
      type = typeDouble;
      doubleValue = rhs.doubleValue;
      break;
    case typeString:
      type = typeString;
      stringValue = rhs.stringValue;
      break;
    case typeCount:
      type = typeCount;
      countValue = rhs.countValue;
      break;
    case typePNode:
      type = typePNode;
      pNodeValue = rhs.pNodeValue;
      break;
    case typePredefinedFunction:
      type = typePredefinedFunction;
      predefinedFunctionValue = rhs.predefinedFunctionValue;
      break;
    }
  }

  operator const int&() const
  {
    switch (type)
    {
    case typeInt:
      return intValue;
    case typeDouble:
      Assign(intResult, doubleValue);
      break;
    case typeString:
      break;
    case typeCount:
      Assign(intResult, countValue);
      break;
    case typePNode:
      break;
    case typePredefinedFunction:
      break;
    }
    return intResult;
  }

  operator const double&() const
  {
    return doubleValue;
  }

  operator const std::string&() const
  {
    switch (type)
    {
    case typeString:
      return stringValue;
    case typeInt:
      Assign(stringResult, std::to_string(intValue));
      break;
    case typeDouble:
      Assign(stringResult, std::to_string(doubleValue));
      break;
    case typeCount:
      Assign(stringResult, std::to_string(countValue));
      break;
    case typePNode:
      Assign(stringResult, (*pNodeValue)->GetTag());
      break;
    case typePredefinedFunction:
      Assign(stringResult, "PredefinedFunction");
      break;
    }
    return stringResult;
  }

  operator const Count&() const
  {
    switch (type)
    {
    case typeCount:
      return countValue;
    case typeInt:
      Assign(countResult, intValue);
      break;
    case typeDouble:
      Assign(countResult, doubleValue);
      break;
    case typeString:
      break;
    case typePNode:
      break;
    case typePredefinedFunction:
      break;
    default: 
      break;
    }
    return countResult;
  }

  operator const Node::PNode&() const
  {
    return *pNodeValue;
  }

  operator const PredefinedFunction&() const
  {
    return predefinedFunctionValue;
  }

  bool TryGet(int& value) const
  {
    if (type == typeInt)
    {
      value = intValue;
      return true;
    }
    return false;
  }

  bool TryGet(double& value) const
  {
    if (type == typeDouble)
    {
      value = doubleValue;
      return true;
    }
    return false;
  }

  bool TryGet(std::string& value) const
  {
    if (type == typeString)
    {
      value = stringValue;
      return true;
    }
    return false;
  }

  bool TryGet(Count& value) const
  {
    if (type == typeCount)
    {
      value = countValue;
      return true;
    }
    return false;
  }

  bool TryGet(Node::PNode& value) const
  {
    if (type == typePNode)
    {
      value = *pNodeValue;
      return true;
    }
    return false;
  }

  bool TryGet(PredefinedFunction& value) const
  {
    if (type == typePredefinedFunction)
    {
      value = predefinedFunctionValue;
      return true;
    }
    return false;
  }
};

#endif
