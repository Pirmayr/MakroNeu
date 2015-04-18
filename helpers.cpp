#include "stdafx.h" 
#include "helpers.h"
#include "stack.h"
#include <iostream>

namespace Helpers {
  using namespace Interpreter;
  using namespace Node;
  using namespace Stack;
  using namespace std;

  void DumpItems(const IStack& items, const string& indent) {
    auto itemsCount = items.Size();
    for (Count i = 0; i < itemsCount; ++i) {
      const auto& currentItem = items.Get(i);
      const string& currentName = currentItem->GetValue();
      const string& currentValue(currentItem->GetValue(0.0));
      cout << indent << currentName << ": " << currentValue << endl;
    }
  }

  void DumpNode(const PNode& currentNode, int level) {
    if (!currentNode->GetTag().empty()) {
      for (auto i = 0; i < level; ++i) {
        cout << "    ";
      }
      string valueString(currentNode->GetValue());
      if (valueString.empty()) {
        cout << currentNode->GetTag() << endl;
      } else {
        cout << currentNode->GetTag() << "(" << valueString << ")" << " / " << currentNode->GetItemOffset() << endl;
      }
    }
    auto childCount = currentNode->ChildCount();
    for (Count i = 0; i < childCount; ++i) {
      DumpNode(currentNode->GetChild(i), level + 1);
    }
  }
}
