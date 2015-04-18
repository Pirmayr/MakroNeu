#include "stdafx.h" 
#include <cassert>
#include <functional>
#include <iostream> 
#include "globals.h"
#include "interpreter.h"
#include "node.h"
#include "stack.h"
#include "system.h"

namespace Interpreter { // 4
  using namespace Helpers;
  using namespace Node;
  using namespace Stack;
  using namespace std;

  struct CInterpreter : IInterpreter {
    typedef void (CInterpreter::*Action)(const PNode&);
    typedef unordered_map<string, Action> Actions;

    template <typename B> inline void DoBinaryCalculation(const PNode& /*tree*/) {
      static B operation;
      const auto rhs = PopValue();
      const auto& lhs = PopValue();
      double numberA;
      double numberB;
      if (lhs.TryGet(numberA) && rhs.TryGet(numberB))
      {
        values->PushNew(tagValue, operation(numberA, numberB));
        return;
      }
      string stringA;
      string stringB;
      if (lhs.TryGet(stringA) && rhs.TryGet(stringB))
      {
        if (typeid(operation) == typeid(plus<double>))
        {
          values->PushNew(tagValue, stringA + stringB);
          return;
        }
      }
      values->PushNew(tagValue, 0);
    }

    template <typename T1, typename B> inline void DoBinaryComparison(const PNode& /*tree*/) {
      static B operation;
      const auto rhs = PopValue();
      const auto& lhs = PopValue();
      T1 value1 = lhs;
      T1 value2 = rhs;
      if ((operation(value1, value2))) {
        values->Push(number1);
      } else {
        values->Push(number0);
      }
    }

    PStack items;
    PStack values;
    Actions preActions;
    Actions postActions;
    PredefinedFunctions predefinedFunctions;
    Count currentBase;
    bool leaveFunction;
    PNode number0;
    PNode number1;

    void Check(bool condition) const override
    {
      if (!condition)
      {
        throw "Check failed";
      }
    }

    const PStack& GetItems() const override
    {
      return items;
    }

    PredefinedFunctions& GetPredefinedFunctions() override
    {
      return predefinedFunctions;
    }

    const PStack& GetValues() const override
    {
      return values;
    }

    any Run(const PNode& tree) override
    {
      try {
        currentBase = 0;
        leaveFunction = false;
        number0 = PNode(tagValue, 0, 0);
        number1 = PNode(tagValue, 1, 0);
        preActions[tagFunction] = &CInterpreter::DoRoutineDefinition;
        preActions[tagProcedure] = &CInterpreter::DoRoutineDefinition;
        preActions[tagIteration] = &CInterpreter::DoWhile;
        preActions[tagValue] = &CInterpreter::DoNumber;
        preActions[tagParameters] = &CInterpreter::DoParameters;
        preActions[tagSelection] = &CInterpreter::DoIf;
        postActions[tagAdd] = &CInterpreter::DoBinaryCalculation<plus<double>>;
        postActions[tagAssign] = &CInterpreter::DoAssign;
        postActions[tagDivide] = &CInterpreter::DoBinaryCalculation<divides<double>>;
        postActions[tagEqual] = &CInterpreter::DoBinaryComparison<double, equal_to<double>>;
        postActions[tagGreaterOrEqual] = &CInterpreter::DoBinaryComparison<double, greater_equal<double>>;
        postActions[tagGreater] = &CInterpreter::DoBinaryComparison<double, greater<double>>;
        postActions[tagIdentifier] = &CInterpreter::DoIdentifier;
        postActions[tagLessOrEqual] = &CInterpreter::DoBinaryComparison<double, less_equal<double>>;
        postActions[tagLess] = &CInterpreter::DoBinaryComparison<double, less<double>>;
        postActions[tagMultiply] = &CInterpreter::DoBinaryCalculation<multiplies<double>>;
        postActions[tagNotEqual] = &CInterpreter::DoBinaryComparison<double, not_equal_to<double>>;
        postActions[tagPrint] = &CInterpreter::DoPrint;
        postActions[tagReturn] = &CInterpreter::DoReturn;
        postActions[tagRun] = &CInterpreter::DoRun;
        postActions[tagSubtract] = &CInterpreter::DoBinaryCalculation<minus<double>>;
        InitializeSystem(*this);
        if (!Evaluate(tree)) {
          return false;
        }
        return PopValue();
      } catch (const string& message) {
        WriteError(message);
      } catch (...) {
        WriteError(errorInterpretationFailed);
      }
      return false;
    }

    void DoAssign(const PNode& /*tree*/) {
      const auto rhs = PopValue();
      const auto& lhs = values->Top(0);
      GetItem(lhs)->SetValue(rhs, lhs->GetValueSelector());
      values->Pop();
    }

    void DoIdentifier(const PNode& tree) {
      SetPredefinedRoutine(tree);
      if (0 < tree->ChildCount() && tree->GetHasPredefinedRoutine() != hasActionTrue && !ItemIsFunction(tree)) { // Identifier represents an array ...
        assert(tree->ChildCount() == 1); // Only one-dimensional arrays so far.
        tree->SetValueSelector(PopValue());
      }
      values->Push(tree);
    }

    void DoIf(const PNode& tree) {
      assert(tree->ChildCount() == 2);
      const auto& condition = tree->GetChild(0);
      const auto& block = tree->GetChild(1);
      Evaluate(condition);
      if (leaveFunction) {
        return;
      }
      int conditionResult = PopValue();
      if (conditionResult != 0) {
        Evaluate(block);
        if (leaveFunction) {
          return;
        }
      }
    }

    void DoNumber(const PNode& tree) {
      values->Push(tree);
    }

    void DoParameters(const PNode& tree) {
      const auto childCount = tree->ChildCount();
      for (size_t i = 0; i < childCount; ++i) {
        const auto& currentChild = tree->GetChild(childCount - i - 1);
        GetItem(currentChild);
      }
      for (size_t j = 0; j < childCount; ++j) {
        const auto& currentChild = tree->GetChild(childCount - j - 1);
        GetItem(currentChild)->SetValue(PopValue(), currentChild->GetValueSelector());
      }
    }

    void DoPrint(const PNode& /*tree*/) {
      const auto value = PopValue();
      cout << static_cast<string>(value) << endl;
    }

    void DoReturn(const PNode& tree) {
      values->PushNew(tagValue, PopValue());
      tree->SetBreakFlow(true);
    }

    void DoRoutineDefinition(const PNode& tree) {
      assert(tree->ChildCount() == 2);
      const auto& identifier = tree->GetChild(0);
      assert(identifier->GetTag() == tagIdentifier);
      GetItem(identifier)->SetValue(tree, identifier->GetValueSelector());
    }

    void DoRun(const PNode& /*tree*/) {
      // do nothing
    }

    void DoWhile(const PNode& tree) {
      assert(tree->ChildCount() == 2);
      const auto& condition = tree->GetChild(0);
      const auto& block = tree->GetChild(1);
      Evaluate(condition);
      if (leaveFunction) {
        return;
      }
      int conditionResult = PopValue();
      while (conditionResult != 0) {
        Evaluate(block);
        if (leaveFunction) {
          return;
        }
        Evaluate(condition);
        if (leaveFunction) {
          return;
        }
        conditionResult = PopValue();
      }
    }

    bool Evaluate(const PNode& tree) {
      if (!PerformPreAction(tree)) {
        const auto& node = *tree;
        auto childCount = node.ChildCount();
        for (Count i = 0; i < childCount; ++i) {
          const auto& currentChild = node.GetChild(i);
          Evaluate(currentChild);
          if (leaveFunction) {
            return true;
          }
          if (currentChild->GetBreakFlow()) {
            leaveFunction = true;
            return true;
          }
        }
        PerformPostAction(tree);
      }
      return true;
    }

    inline const PNode& GetItem(const PNode& identifierNode) {
      auto itemOffset = identifierNode->GetItemOffset();
      if (identifierNode->GetIsGlobal() == isGlobalNotSet) {
        if (currentBase == 0) {
          identifierNode->SetIsGlobal(isGlobalTrue);
        } else {
          if (itemOffset < items->Size()) {
            const string& value1 = items->Get(itemOffset)->GetValue();
            const string& value2 = identifierNode->GetValue();
            if (value1 == value2) {
              identifierNode->SetIsGlobal(isGlobalTrue);
            } else {
              identifierNode->SetIsGlobal(isGlobalFalse);
            }
          } else {
            identifierNode->SetIsGlobal(isGlobalFalse);
          }
        }
      }
      auto actualBase = identifierNode->GetIsGlobal() == isGlobalTrue ? 0 : currentBase;
      auto index = actualBase + itemOffset;
      if (items->Size() <= index) {
        items->Resize(index + 1);
        items->Get(index)->SetValue(identifierNode->GetValue());
      }
      return items->Get(index);
    }

    inline const any& GetValue(const PNode& node) {
      static const any defaultValue = 0;
      if (node->GetTag() == tagIdentifier) {
        if (PerformPredefinedRoutine(node))
        {
          return PopValue();
        }
        const auto& result = GetItem(node)->GetValue(node->GetValueSelector());
        if (result.type != any::typePNode) {
          return result; // Result is a "normal" scalar value (e.g. a number).
        }
        // Result is a function to be called:
        const PNode& function = result;
        assert(function->GetTag() == tagFunction || function->GetTag() == tagProcedure);
        assert(function->ChildCount() == 2);
        const auto& block = function->GetChild(1);
        auto oldBase = currentBase;
        currentBase = items->Size();
        Evaluate(block);
        leaveFunction = false;
        items->Resize(currentBase);
        currentBase = oldBase;
        if (function->GetTag() == tagFunction)
        {
          return PopValue();
        }
        return defaultValue;
      }
      return node->GetValue();
    }

    inline bool ItemIsFunction(const PNode& node) {
      const auto& value = GetItem(node)->GetValue(0.0);
      return value.type == any::typePNode;
    }

    const any& PopValue() override
    {
      const auto& top = values->Top(0);
      values->Pop();
      return GetValue(top);
    }

    inline bool PerformPreAction(const PNode& node) {
      auto hasAction = node->GetHasPreAction();
      if (hasAction == hasActionNotSet) {
        const auto& iterator = preActions.find(node->GetTag());
        if (iterator == preActions.end()) {
          node->SetHasPreAction(hasActionFalse);
        } else {
          node->SetPreAction(static_cast<void*>(&iterator->second));
          node->SetHasPreAction(hasActionTrue);
          hasAction = hasActionTrue;
        }
      }
      if (hasAction == hasActionTrue) {
        auto pAction = static_cast<Action*>(node->GetPreAction());
        (this->**pAction)(node);
        return true;
      }
      return false;
    }

    inline void PerformPostAction(const PNode& node) {
      auto hasAction = node->GetHasPostAction();
      if (hasAction == hasActionNotSet) {
        const auto& iterator = postActions.find(node->GetTag());
        if (iterator == postActions.end()) {
          node->SetHasPostAction(hasActionFalse);
        } else {
          node->SetPostAction(static_cast<void*>(&iterator->second));
          node->SetHasPostAction(hasActionTrue);
          hasAction = hasActionTrue;
        }
      }
      if (hasAction == hasActionTrue) {
        auto pAction = static_cast<Action*>(node->GetPostAction());
        (this->**pAction)(node);
      }
    }

    inline void SetPredefinedRoutine(const PNode& node) {
      if (node->GetHasPredefinedRoutine() == hasActionNotSet) {
        const auto& iterator = predefinedFunctions.find(static_cast<string>(node->GetValue()));
        if (iterator == predefinedFunctions.end()) {
          node->SetHasPredefinedRoutine(hasActionFalse);
        } else {
          node->SetHasPredefinedRoutine(hasActionTrue);
          node->SetPredefinedRoutine(iterator->second);
        }
      }
    }

    bool PerformPredefinedRoutine(const PNode& node)
    {
      if (node->GetHasPredefinedRoutine() == hasActionTrue)
      {
        PredefinedFunction predefinedFunction = node->GetPredefinedRoutine();
        predefinedFunction(*this, *node);  
        return true;
      }
      return false;
    }

    void WriteError(const string& message) {
      cout << message << endl;
    }
  };

  PInterpreter::PInterpreter(): shared_ptr<IInterpreter>(new CInterpreter()) {}
}
