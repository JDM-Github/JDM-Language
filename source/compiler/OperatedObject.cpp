#include "compiler.hpp"

JDM_DLL
std::shared_ptr<HigherObject> Compiler::newOperatedObject(
	std::shared_ptr<HigherObject> &firstVal,
	const std::string &operation,
	const std::shared_ptr<HigherObject> &secondVal)
{
	// If no operation is specified, return the first value as is
	if (operation == "")
		return firstVal;

	// If a second value is provided and the operation is "!", negate the boolean value of the first object
	if (secondVal == nullptr)
	{
		if (operation == "!")
		{
			firstVal->castToBoolean();
			firstVal->booleanValue = !firstVal->booleanValue;
		}
		return firstVal;
	}
	// Handle different types of firstVal based on its type
	if      (firstVal->isString) this->handleNewOperatedString(firstVal, operation, secondVal);
	else if (firstVal->isList  ) this->handleNewOperatedList  (firstVal, operation, secondVal);
	else if (firstVal->isMap   ) this->handleNewOperatedMap   (firstVal, operation, secondVal);
	else if (firstVal->isFunc  ) this->handleNewOperatedFunc  (firstVal, operation, secondVal);
	else if (firstVal->isObject) this->handleNewOperatedObject(firstVal, operation, secondVal);
	else if (firstVal->isInteger || firstVal->isDecimal || firstVal->isBoolean)
		// Handle number types (integer, decimal, boolean)
		this->handleNewOperatedNumber(firstVal, operation, secondVal);

	// Return the modified first value
	return firstVal;
}

JDM_DLL
void Compiler::handleNewOperatedString(
	std::shared_ptr<HigherObject> &firstVal,
	const std::string &operation,
	const std::shared_ptr<HigherObject> &secondVal)
{
	if (operation == "+")
	{
		secondVal->castToString();
		firstVal->stringValue += secondVal->stringValue;
	}
	else if (operation == "*")
	{
		if (!secondVal->isInteger && !secondVal->isDecimal)
			throw std::runtime_error("Runtime Error: Must be a Number to multiply to String.");

		secondVal->castToInteger();
		if (secondVal->integerValue < 0)
			throw std::runtime_error("Runtime Error: Integer must be greater than 0.");

		const std::string &temporary = firstVal->stringValue;
		for (int i = 1; i < secondVal->integerValue; i++)
			firstVal->stringValue += temporary;
	}
	else
	{
		if (!secondVal->isString)
			throw std::runtime_error("Runtime Error: Cannot do operation on String and not String.");

		bool result = Comparator::DoOperationString(
			firstVal->stringValue,
			secondVal->stringValue,
			operation
		);
		firstVal->castToBoolean();
		firstVal->booleanValue = result;
	}
}

JDM_DLL
void Compiler::handleNewOperatedList(
	std::shared_ptr<HigherObject> &firstVal,
	const std::string &operation,
	const std::shared_ptr<HigherObject> &secondVal)
{
	if (operation == "+") 
	{
		secondVal->castToList();
		firstVal->listValue.insert(
			firstVal ->listValue.end(),
			secondVal->listValue.begin(),
			secondVal->listValue.end()
		);
	}
	else
	{
		if (!secondVal->isList)
			throw std::runtime_error("Runtime Error: Cannot do operation on List and not List.");

		bool result = Comparator::DoOperationList(
			firstVal,
			secondVal,
			operation
		);
		firstVal->castToBoolean();
		firstVal->booleanValue = result;
	}
}

JDM_DLL
void Compiler::handleNewOperatedMap(
	std::shared_ptr<HigherObject> &firstVal,
	const std::string &operation,
	const std::shared_ptr<HigherObject> &secondVal)
{
	if (!secondVal->isMap)
		throw std::runtime_error("Runtime Error: Cannot do operation on 'jmap' and not 'jmap'.");

	if (operation == "+")
	{
		for (const auto& element : secondVal->mapValue)
		{
			auto isInKey = firstVal->isInMap(element.first, true);
			if (isInKey)
				firstVal->mapValue[isInKey] = element.second;
			else
				firstVal->mapValue[element.first] = element.second;
		}
	}
	else
	{		
		bool result = Comparator::DoOperationMap(
			firstVal,
			secondVal,
			operation
		);
		firstVal->castToBoolean();
		firstVal->booleanValue = result;
	}
}

JDM_DLL
void Compiler::handleNewOperatedFunc(
	std::shared_ptr<HigherObject> &firstVal,
	const std::string &operation,
	const std::shared_ptr<HigherObject> &secondVal)
{
	if (!secondVal->isFunc)
		throw std::runtime_error("Runtime Error: Cannot do operation on 'jfunc' and not 'jfunc'.");

	bool result = Comparator::DoOperationFunc(
		firstVal,
		secondVal,
		operation
	);
	firstVal->castToBoolean();
	firstVal->booleanValue = result;
}


JDM_DLL
void Compiler::handleNewOperatedNumber(
	std::shared_ptr<HigherObject> &firstVal,
	const std::string &operation,
	const std::shared_ptr<HigherObject> &secondVal)
{
	if (secondVal->isString) throw std::runtime_error("Runtime Error: Cannot do operation on Number and 'jstring'.");
	if (secondVal->isObject) throw std::runtime_error("Runtime Error: Cannot do operation on Number and 'jobject'.");
	if (secondVal->isFunc  ) throw std::runtime_error("Runtime Error: Cannot do operation on Number and 'func'.");
	if (secondVal->isList  ) throw std::runtime_error("Runtime Error: Cannot do operation on Number and 'jlist.");
	if (secondVal->isMap   ) throw std::runtime_error("Runtime Error: Cannot do operation on Number and 'jmap'.");

	DataTypeEnum varType = firstVal->getDatatypeEnum();
	firstVal ->castToDecimal();
	secondVal->castToDecimal();


	if      (operation == "+" ) firstVal->doubleValue += secondVal->doubleValue;
	else if (operation == "-" ) firstVal->doubleValue -= secondVal->doubleValue;
	else if (operation == "*" ) firstVal->doubleValue *= secondVal->doubleValue;
	else if (operation == "/" ) firstVal->doubleValue /= secondVal->doubleValue;
	else if (operation == "//") firstVal->doubleValue  = static_cast<int64_t>(firstVal->doubleValue / secondVal->doubleValue);
	else if (operation == "%" ) firstVal->doubleValue  = static_cast<int64_t>(firstVal->doubleValue) %  static_cast<int64_t>(secondVal->doubleValue);
	else if (operation == "&" ) firstVal->doubleValue  = static_cast<int64_t>(firstVal->doubleValue) &  static_cast<int64_t>(secondVal->doubleValue);
	else if (operation == "|" ) firstVal->doubleValue  = static_cast<int64_t>(firstVal->doubleValue) |  static_cast<int64_t>(secondVal->doubleValue);
	else if (operation == "<<") firstVal->doubleValue  = static_cast<int64_t>(firstVal->doubleValue) << static_cast<int64_t>(secondVal->doubleValue);
	else if (operation == ">>") firstVal->doubleValue  = static_cast<int64_t>(firstVal->doubleValue) >> static_cast<int64_t>(secondVal->doubleValue);
	else if (operation == "^" ) firstVal->doubleValue  = static_cast<int64_t>(firstVal->doubleValue) >> static_cast<int64_t>(secondVal->doubleValue);
	else if (operation == "**") firstVal->doubleValue  = pow(firstVal->doubleValue, secondVal->doubleValue);
	else
	{
		bool result = Comparator::DoOperationNumber(
			firstVal->doubleValue,
			secondVal->doubleValue,
			operation
		);
		firstVal->castToBoolean();
		firstVal->booleanValue = result;
		return;
	}
	if      (varType == DataTypeEnum::DATA_INTEGER) firstVal->castToInteger ();
	else if (varType == DataTypeEnum::DATA_BOOLEAN) firstVal->castToBoolean ();
	else if (varType == DataTypeEnum::DATA_DOUBLE ) firstVal->castToDecimal ();
}

JDM_DLL
void Compiler::handleNewOperatedObject(
	std::shared_ptr<HigherObject> &firstVal,
	const std::string &operation,
	const std::shared_ptr<HigherObject> &secondVal)
{
	if (!secondVal->isObject)
		throw std::runtime_error("Runtime Error: Cannot do operation on 'jobject' and not 'jobject'.");

	bool result = Comparator::DoOperationObject(
		firstVal,
		secondVal,
		operation
	);
	firstVal->castToBoolean();
	firstVal->booleanValue = result;
}
