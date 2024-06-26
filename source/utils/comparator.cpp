#include "utils/comparator.hpp"

JDM_DLL const bool Comparator::DoOperationString(
	const std::string &first,
	const std::string &second,
	const std::string &operation)
{
	bool firstBool  = !first.empty();
	bool secondBool = !second.empty();

	if (operation == "&&") return firstBool && secondBool;
	if (operation == "||") return firstBool || secondBool;
	if (operation == "==") return first == second;
	if (operation == "!=") return first != second;
	if (operation == "<" ) return first.size() <  second.size();
	if (operation == ">" ) return first.size() >  second.size();
	if (operation == "<=") return first.size() <= second.size();
	if (operation == ">=") return first.size() >= second.size();
	throw std::runtime_error("Runtime Error: This operation is not allowed on 'jstring'.");
}

JDM_DLL const bool Comparator::DoOperationList(
	std::shared_ptr<HigherObject> &firstVal,
	const std::shared_ptr<HigherObject> &secondVal,
	const std::string &operation)
{
	bool firstBool  = !firstVal ->listValue.empty();
	bool secondBool = !secondVal->listValue.empty();

	if (operation == "&&") return  firstBool && secondBool;
	if (operation == "||") return  firstBool || secondBool;
	if (operation == "==") return  firstVal->compareHigherObject(secondVal);
	if (operation == "!=") return !firstVal->compareHigherObject(secondVal);
	if (operation == "<" ) return  firstVal->listValue.size() <  secondVal->listValue.size();
	if (operation == ">" ) return  firstVal->listValue.size() >  secondVal->listValue.size();
	if (operation == "<=") return  firstVal->listValue.size() <= secondVal->listValue.size();
	if (operation == ">=") return  firstVal->listValue.size() >= secondVal->listValue.size();
	throw std::runtime_error("Runtime Error: This operation is not allowed on 'jlist'.");
}

JDM_DLL const bool Comparator::DoOperationMap(
	std::shared_ptr<HigherObject> &firstVal,
	const std::shared_ptr<HigherObject> &secondVal,
	const std::string &operation)
{
	bool firstBool  = !firstVal ->mapValue.empty();
	bool secondBool = !secondVal->mapValue.empty();

	if (operation == "&&") return  firstBool && secondBool;
	if (operation == "||") return  firstBool || secondBool;
	if (operation == "==") return  firstVal->compareHigherObject(secondVal);
	if (operation == "!=") return !firstVal->compareHigherObject(secondVal);
	if (operation == "<" ) return  firstVal->mapValue.size() <  secondVal->mapValue.size();
	if (operation == ">" ) return  firstVal->mapValue.size() >  secondVal->mapValue.size();
	if (operation == "<=") return  firstVal->mapValue.size() <= secondVal->mapValue.size();
	if (operation == ">=") return  firstVal->mapValue.size() >= secondVal->mapValue.size();
	throw std::runtime_error("Runtime Error: This operation is not allowed on 'jmap'.");
}

JDM_DLL const bool Comparator::DoOperationFunc(
	std::shared_ptr<HigherObject> &firstVal,
	const std::shared_ptr<HigherObject> &secondVal,
	const std::string &operation)
{
	bool firstBool  = firstVal ->funcValue != nullptr;
	bool secondBool = secondVal->funcValue != nullptr;

	if (operation == "&&") return  firstBool && secondBool;
	if (operation == "||") return  firstBool || secondBool;
	if (operation == "==") return  firstVal->compareHigherObject(secondVal);
	if (operation == "!=") return !firstVal->compareHigherObject(secondVal);
	throw std::runtime_error("Runtime Error: This operation is not allowed on 'jfunc'.");
}

JDM_DLL const bool Comparator::DoOperationObject(
	std::shared_ptr<HigherObject> &firstVal,
	const std::shared_ptr<HigherObject> &secondVal,
	const std::string &operation)
{
	bool firstBool  = firstVal ->objectValue != nullptr;
	bool secondBool = secondVal->objectValue != nullptr;

	if (operation == "&&") return  firstBool && secondBool;
	if (operation == "||") return  firstBool || secondBool;
	if (operation == "==") return  firstVal->compareHigherObject(secondVal);
	if (operation == "!=") return !firstVal->compareHigherObject(secondVal);
	throw std::runtime_error("Runtime Error: This operation is not allowed on 'jobject'.");
}

JDM_DLL const bool Comparator::DoOperationNumber(
	double firstVal,
	double secondVal,
	const std::string &operation)
{
	bool firstBool  = firstVal  > 0.0;
	bool secondBool = secondVal > 0.0;

	if (operation == "&&") return firstBool && secondBool;
	if (operation == "||") return firstBool || secondBool;
	if (operation == "==") return firstVal  == secondVal;
	if (operation == "!=") return firstVal  != secondVal;
	if (operation == "<" ) return firstVal  <  secondVal;
	if (operation == ">" ) return firstVal  >  secondVal;
	if (operation == "<=") return firstVal  <= secondVal;
	if (operation == ">=") return firstVal  >= secondVal;
	throw std::runtime_error("Runtime Error: This operation is not allowed on 'number'."); 
}