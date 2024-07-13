#include "Compiler.hpp"

JDM_DLL
const std::shared_ptr<HigherObject> Compiler::manageCallBrackets(
	const std::shared_ptr<CallObjects > &next,
	const std::shared_ptr<HigherObject> &returnVal,
	const std::shared_ptr<Expression  > &expressionAssign)
{
	auto exprObj = std::dynamic_pointer_cast<ExpressionObjects>(next->currObject);
	auto expr    = this->evaluateExpression(exprObj->expression); // Evaluate the expression
	if (returnVal->getCurrActive() == ACTIVE_LIST)
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

			this->isAssigning = true;
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

			if (returnVal->listValue[index]->isConstant)
				throw std::runtime_error("Runtime Error: Variable is Constant.");
			this->checkVariableConstraint(returnVal->listValue[index], exprResult->getDatatypeEnum());

			if (returnVal == exprResult)
				throw std::runtime_error("Runtime Error: Self-referencing in list");

			returnVal->listValue[index]->setHigherObject(exprResult);

			// This is useful because this will tell that the list need sorting again.
			returnVal->isListSorted = false; // This just tell that the list is not sorted.
			return nullptr;
		}
		return returnVal->listValue[index];
	}
	else if (returnVal->getCurrActive() == ACTIVE_STRING)
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

			this->isAssigning = true;
			auto exprResult = this->evaluateExpression(expressionAssign);
			if (exprResult->getIsReferenced())
				throw std::runtime_error("Runtime Error: Invalid assignment. Referenced variables cannot be assigned to specific elements in a 'jstring'.");

			exprResult->castToString();
			// No operation is valid in this.
			if (exprResult->stringValue.size() != 1)
				throw std::runtime_error("Runtime Error: Expected single character assignment. Use insert instead.");

			returnVal->stringValue[index] = exprResult->stringValue[0];
		}
		return std::make_shared<HigherObject>(std::string(1, returnVal->stringValue[index]));
	}
	else if (returnVal->getCurrActive() == ACTIVE_MAP)
	{
		if (next->nextObject == nullptr && expressionAssign != nullptr)
		{
			if (returnVal->isConstant)
				throw std::runtime_error("Runtime Error: Variable is Constant.");
			auto key = returnVal->isInMap(expr, true);

			if (key == nullptr && next->operation != "=")
				throw std::runtime_error("Runtime Error: Key in map doesn't exist. Cannot do operation.");

			this->isAssigning = true;
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

			if (key != nullptr)
			{
				if (returnVal->mapValue[key]->isConstant)
					throw std::runtime_error("Runtime Error: Variable is Constant.");
				this->checkVariableConstraint(returnVal->mapValue[key], exprResult->getDatatypeEnum());

				if (returnVal == exprResult)
					throw std::runtime_error("Runtime Error: Self-referencing in list");

				returnVal->mapValue[key]->setHigherObject(exprResult);
				return nullptr;
			}

			if (expr->getCurrActive() != ACTIVE_STRING
			 && expr->getCurrActive() != ACTIVE_INTEGER
			 && expr->getCurrActive() != ACTIVE_DECIMAL
			 && expr->getCurrActive() != ACTIVE_BOOLEAN)
				std::runtime_error("Runtime Error: Only 'jstring', 'jint', 'jdouble' and 'jboolean' is allowed to be a key.");

			if (expr->getIsReferenced())
				throw std::runtime_error("Runtime Error: Referenced cannot be a key.");

			returnVal->mapValue[expr] = exprResult;
			return nullptr;
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
