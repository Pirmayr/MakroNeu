#ifndef helpers_h
#define helpers_h

#include "any.h"

#define DECLARE_ENUMERATION(name) static const char* name = #name

DECLARE_ENUMERATION(tagAdd);
DECLARE_ENUMERATION(tagAssign);
DECLARE_ENUMERATION(tagDefine);
DECLARE_ENUMERATION(tagDivide);
DECLARE_ENUMERATION(tagEqual);
DECLARE_ENUMERATION(tagFunction);
DECLARE_ENUMERATION(tagProcedure);
DECLARE_ENUMERATION(tagGreater);
DECLARE_ENUMERATION(tagGreaterOrEqual);
DECLARE_ENUMERATION(tagIdentifier);
DECLARE_ENUMERATION(tagIteration);
DECLARE_ENUMERATION(tagLess);
DECLARE_ENUMERATION(tagLessOrEqual);
DECLARE_ENUMERATION(tagMultiply);
DECLARE_ENUMERATION(tagNotEqual);
DECLARE_ENUMERATION(tagValue);
DECLARE_ENUMERATION(tagParameters);
DECLARE_ENUMERATION(tagExpressions);
DECLARE_ENUMERATION(tagPrint);
DECLARE_ENUMERATION(tagReturn);
DECLARE_ENUMERATION(tagRun);
DECLARE_ENUMERATION(tagSelection);
DECLARE_ENUMERATION(tagSequence);
DECLARE_ENUMERATION(tagSubtract);

DECLARE_ENUMERATION(errorInterpretationFailed);
DECLARE_ENUMERATION(errorOutOfStackRange);
DECLARE_ENUMERATION(errorStackUnderflow);
DECLARE_ENUMERATION(errorTypesIncompatible);

namespace Node {
  struct PNode;
}

namespace Stack {
  struct IStack;
}

namespace Helpers {
  extern void DumpItems(const Stack::IStack& items, const std::string& indent = "    ");
  extern void DumpNode(const Node::PNode& currentNode, int level = 0);
};

#endif
