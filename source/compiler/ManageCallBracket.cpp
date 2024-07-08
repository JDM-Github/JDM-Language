#include "compiler.hpp"

JDM_DLL
const std::shared_ptr<HigherObject> Compiler::manageCallBrackets(
	const std::shared_ptr<CallObjects > &next,
	const std::shared_ptr<HigherObject> &returnVal,
	const std::shared_ptr<Expression  > &expressionAssign)
{
	auto exprObj = std::dynamic_pointer_cast<ExpressionObjects>(next->currObject);
	auto expr    = this->evaluateExpression(exprObj->expression); // Evaluate the expression
	if (returnVal->isList)
	{
		expr->castToInteger();
		int index = expr->integerValue;
		if (index < 0)
			index += returnVal->listValue.size();
		if (index < 0 || index >= returnVal->listValue.size())
			throw std::runtime_error("Runtime Error: Index out of bounds.");
		if (next->nextObject == nullptr && expressionAssign != nullptr)
		{
			if (returnVal->isConstant)
				throw std::runtime_error("Runtime Error: Variable is Constant.");
			auto exprResult = this->evaluateExpression(expressionAssign);
			auto oldValue   = returnVal->listValue[index];
			if (next->operation == "+=")
				exprResult = this->newOperatedObject(oldValue, "+", exprResult);
			else if (next->operation == "-=")
				exprResult = this->newOperatedObject(oldValue, "-", exprResult);
			else if (next->operation == "*=")
				exprResult = this->newOperatedObject(oldValue, "*", exprResult);
			else if (next->operation == "/=")
				exprResult = this->newOperatedObject(oldValue, "/", exprResult);
			else if (next->operation == "%=")
				exprResult = this->newOperatedObject(oldValue, "%", exprResult);
			returnVal->listValue[index] = exprResult;
			// This is useful because this will tell that the list need sorting again.
			returnVal->isListSorted = false; // This just tell that the list is not sorted.
		}
		return returnVal->listValue[index];
	}
	else if (returnVal->isString)
	{
		expr->castToInteger();
		int index = expr->integerValue;
		if (index < 0)
			index += returnVal->stringValue.size();
		if (index < 0 || index >= returnVal->stringValue.size())
			throw std::runtime_error("Runtime Error: String index out of bounds.");
		if (next->nextObject == nullptr && expressionAssign != nullptr)
		{
			if (returnVal->isConstant)
				throw std::runtime_error("Runtime Error: Variable is Constant.");
			auto exprResult = this->evaluateExpression(expressionAssign);
			exprResult->castToString();
			// No operation is valid in this.
			if (exprResult->stringValue.size() != 1)
				throw std::runtime_error("Runtime Error: Expected single character assignment. Use insert instead.");
			returnVal->stringValue[index] = exprResult->stringValue[0];
		}
		return std::make_shared<HigherObject>(std::string(1, returnVal->stringValue[index]));
	}
	else if (returnVal->isMap)
	{
		if (next->nextObject == nullptr && expressionAssign != nullptr)
		{
			if (returnVal->isConstant)
				throw std::runtime_error("Runtime Error: Variable is Constant.");
			auto key = returnVal->isInMap(expr, true);
			if (key == nullptr && next->operation != "=")
				throw std::runtime_error("Runtime Error: Key in map doesn't exist. Cannot do operation.");
			auto exprResult = this->evaluateExpression(expressionAssign);
			if (key != nullptr)
			{
				auto oldValue = returnVal->mapValue.at(key);
				if (next->operation == "+=")
					exprResult = this->newOperatedObject(oldValue, "+", exprResult);
				else if (next->operation == "-=")
					exprResult = this->newOperatedObject(oldValue, "-", exprResult);
				else if (next->operation == "*=")
					exprResult = this->newOperatedObject(oldValue, "*", exprResult);
				else if (next->operation == "/=")
					exprResult = this->newOperatedObject(oldValue, "/", exprResult);
				else if (next->operation == "%=")
					exprResult = this->newOperatedObject(oldValue, "%", exprResult);
			}
			returnVal->mapValue[((key != nullptr) ? key : expr)] = exprResult;
			return exprResult;
		}
		else
		{
			auto returnValue = returnVal->isInMap(expr);
			if (returnValue == nullptr)
				throw std::runtime_error("Runtime Error: Map key is not found");
			return returnValue;
		}
	}
	return nullptr;
}
