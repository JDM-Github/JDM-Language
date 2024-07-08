#include "compiler.hpp"

JDM_DLL
std::shared_ptr<HigherObject> Compiler::getHigherObject(
	const std::shared_ptr<VarObjects> &Value,
	const std::shared_ptr<Expression> &Expression)
{
	if ( !Value )
		// If Value is nullptr then just return the value of expression
		return this->evaluateExpression(Expression);
	// Get the value from Variable
	JDM::TokenType type = Value->getToken();

	if (type == JDM::TokenType::VARIABLE)
	{
		// Get the variable name of the current Variable
		auto varName = std::dynamic_pointer_cast<VariableObjects>(Value)->returnStringValue();

		// Check if it is a variable and get it's value from pair
		auto var  = this->variable->variables.find(varName);
		if (var != this->variable->variables.end())
			return std::make_shared<HigherObject>(var->second.second);

		// Check if it is a function
		auto func = this->variable->functionMap.find(varName);
		if (func != this->variable->functionMap.end())
			return std::make_shared<HigherObject>(func->second);

		auto nativeFunc = NativeFunction::allNativeFunction.find(varName);
		if (nativeFunc != NativeFunction::allNativeFunction.end())
		{
			auto newFunction = std::make_shared<HigherObject::FunctionCall>();
			// If it is a native function rename it to add native, this is useful
			// For me check what is native and what is not.
			newFunction->funcName = "$native-" + varName;
			return std::make_shared<HigherObject>(newFunction);
		}
		throw std::runtime_error("Runtime Error: Variable or Function is not declared.");
	}
	if (type == JDM::TokenType::STRING )
		return std::make_shared<HigherObject>(std::dynamic_pointer_cast<StringObjects>(Value)->returnValue());
	if (type == JDM::TokenType::INTEGER)
		return std::make_shared<HigherObject>(std::dynamic_pointer_cast<IntegerObjects>(Value)->returnValue());
	if (type == JDM::TokenType::DOUBLE)
		return std::make_shared<HigherObject>(std::dynamic_pointer_cast<DoubleObjects>(Value)->returnValue());
	if (type == JDM::TokenType::BOOLED)
		return std::make_shared<HigherObject>(std::dynamic_pointer_cast<BooleanObjects>(Value)->returnValue());
	if (type == JDM::TokenType::LIST)
	{
		std::vector<std::shared_ptr<HigherObject>> vecList;

		// Evaluate all the list in this list
		auto exprList = std::dynamic_pointer_cast<ListObject>(Value)->returnValue();
		for (const auto &expr : exprList)
			// [1+2, 3+4] -> [3, 7]
			vecList.push_back( this->evaluateExpression(expr) );

		// Create a new HigherObject List
		return std::make_shared<HigherObject>(vecList);
	}
	if (type == JDM::TokenType::MAP)
	{
		// Evaluate all the list in this map
		std::unordered_map<std::shared_ptr<HigherObject>, std::shared_ptr<HigherObject>> mapList;
		auto mapStruct = std::dynamic_pointer_cast<MapObject>(Value)->returnValue();
		for (const auto &expr : mapStruct)
		{
			auto value = this->evaluateExpression(expr->key);
			for (auto it = mapList.begin(); it != mapList.end(); ++it)
			{
				if (it->first->compareHigherObject(value))
				{
					value = it->first;
					break;
				}
			}
			// Used to create key and value using evaluateExpression
			mapList[value] = this->evaluateExpression(expr->value);
		}
		return std::make_shared<HigherObject>(mapList);
	}
	// If the expression is Call Object, or function
	if (type == JDM::TokenType::CALL_OBJ)
	{
		// Get the root of CallObjects and get the result using recursivelyCall
		auto callObjRoot = std::dynamic_pointer_cast<CallObjects>(Value);
		while (callObjRoot->prevObject != nullptr)
			callObjRoot = callObjRoot->prevObject;
		auto result = this->recursivelyCall(callObjRoot);
		if (result == nullptr)
			return std::make_shared<HigherObject>(static_cast<int64_t>(0));
		return result;
	}
	// If the expression is a Lambda Expression
	// It is really not a function call so this is separate
	if (type == JDM::TokenType::LAMBDA)
	{
		auto lambdaObj   = std::dynamic_pointer_cast<LambdaObjects>(Value);
		auto newFunction = std::make_shared<HigherObject::FunctionCall>();
		for (const auto &var : lambdaObj->parameters)
		{
			if (var->dataType != nullptr)
			{
				auto dataT   = JDM::dataTypeMap.at(std::get<0>(var->dataType->token));
				newFunction->parameters[var->varName->returnStringValue()] = this->returnVariable(var->varValue, dataT);
			}
			else
				newFunction->parameters[var->varName->returnStringValue()] = this->returnVariable(
					var->varValue, DataTypeEnum::DATA_ANY);
		}
		newFunction->blockWillRun     = std::dynamic_pointer_cast<Block>(lambdaObj->blockWillRun);
		newFunction->varNameAccesible = this->getAllVarName(this->variable->variables);

		// If the lambda is being called, Then just call it, if not, make it like a function
		if (lambdaObj->willCall)
			return this->runFunction(newFunction, this->getVectorHigherObject(lambdaObj->arguments));
		else
			return std::make_shared<HigherObject>(newFunction);
	}
	if (type == JDM::TokenType::CAST)
	{
		auto castObj = std::dynamic_pointer_cast<CastObjects>(Value);
		return this->castVariable(castObj->expression, castObj->datTypeToTurn, false);
	}
	return nullptr;
}

JDM_DLL
std::shared_ptr<HigherObject> &Compiler::getVariableObject(
	const std::shared_ptr<Expression> &expr)
{
	if (!expr || expr->firstValue == nullptr)
		throw std::runtime_error("Runtime Error: Expecting a Variable.");
	if (expr->opWillUse)
		throw std::runtime_error("Runtime Error: Expecting L Value not R Value.");

	auto Value = expr->firstValue;
	JDM::TokenType type = Value->getToken();
	if (type != JDM::TokenType::VARIABLE)
		throw std::runtime_error("Runtime Error: Expecting a Variable.");

	auto varName = std::dynamic_pointer_cast<VariableObjects>(Value)->returnStringValue();
	auto var = this->variable->variables.find(varName);
	if (var != this->variable->variables.end())
		return var->second.second;

	throw std::runtime_error("Runtime Error: Variable is not declared.");
}

JDM_DLL
std::shared_ptr<HigherObject> Compiler::evaluateExpression(
	const std::shared_ptr<Expression> &expr)
{
	if (!expr)
		return std::make_shared<HigherObject>(static_cast<int64_t>(0));

	auto firstVal = this->getHigherObject(expr->firstValue, expr->firstExpression);
	if (firstVal == nullptr)
		throw std::runtime_error("Runtime Error: Invalid Expression. Return null.");

	if (expr->opWillUse)
	{
		if (expr->secondValue || expr->secondExpression)
		{
			auto second = this->getHigherObject  (expr->secondValue, expr->secondExpression);
			firstVal    = this->newOperatedObject(firstVal, std::get<0>(expr->opWillUse->token), second);
		}
		else
			firstVal = this->newOperatedObject(firstVal, std::get<0>(expr->opWillUse->token), nullptr);
	}
	return firstVal;
}
