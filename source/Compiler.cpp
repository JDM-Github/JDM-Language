#include "Compiler.hpp"
#include "library/ListObject.hpp"
#include "library/StringObject.hpp"
#include "library/MapObject.hpp"

#include "utils/Comparator.hpp"
#include "library/classes/Console.hpp"
#include "library/classes/File.hpp"
#include "library/classes/Math.hpp"

JDM_DLL
Compiler::Compiler(const std::shared_ptr<Block> &mainBlock)
	: __mainBlock(mainBlock)
{
	this->_initializeVariables();
	this->compile(this->__mainBlock);
}

JDM_DLL
const void Compiler::_initializeVariables()
{
	this->__nativeClassMap =
	{
		{ "Console", std::make_shared<ConsoleClass>() },
		{ "File"   , std::make_shared<FileClass>() },
		{ "Math"   , std::make_shared<MathClass>() }
	};
}

// =================================================================================================
// COMPILE
// 
// =================================================================================================
JDM_DLL
const std::shared_ptr<HigherObject> Compiler::compile(
	const std::shared_ptr<Block> &block,
	const std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_variables,
	const std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_AdditionalVariables,
	const std::unordered_map<std::string, std::shared_ptr<FunctionCall>> &_functionsAvail)
{
	// Use for scoping
	if (this->__variable == nullptr)
		this->__variable = std::make_shared<VariableLink>();
	else
	{
		this->__variable->next = std::make_shared<VariableLink>();
		this->__variable->next->prev = this->__variable;
		this->__variable = this->__variable->next;
	}
	this->__variable->variables = _variables;
	for (const auto& element : _AdditionalVariables)
	{
		this->__variable->variables[element.first] = element.second;
	}
	this->__variable->functionMap = _functionsAvail;


	bool willReturn = false;
	std::shared_ptr<HigherObject> valueToReturn = nullptr;
	for (const auto &instruction : block->instruction)
	{
		// Reset the break and continue
		this->__breakLoop   = false;
		this->__isAssigning = false;
		if      (instruction->getType() == DeclarationInstruction   ) this->_doDeclarationInstruction(instruction);
		else if (instruction->getType() == AssignmentInstruction    ) this->_doAssignmentInstruction(instruction);
		else if (instruction->getType() == LoggerInstruction        ) this->_doLoggerInstruction(instruction);
		else if (instruction->getType() == CFunctionInstruction     ) this->_doCFunctionInstruction(instruction);
		else if (instruction->getType() == CreateFunctionInstruction) this->_processCreateFunction(instruction);
		else if (instruction->getType() == CallInstruction          ) this->_processCallFunction(instruction);
		else if (instruction->getType() == IfStatementInstruction   )
		{
			valueToReturn = this->_processIfStatement(instruction);
			if (valueToReturn != nullptr)
				willReturn = true;
		}
		else if (instruction->getType() == ForLoopStatementInstruction)
		{
			valueToReturn = this->_processForStatement(instruction);
			if (valueToReturn != nullptr)
				willReturn = true;
		}
		else if (instruction->getType() == ForEachStatementInstruction)
		{
			valueToReturn = this->_processForEachStatement(instruction);
			if (valueToReturn != nullptr)
				willReturn = true;
		}
		else if (instruction->getType() == WhileStatementInstruction)
		{
			auto valueToReturn = this->_processWhileLoop(instruction);
			if (valueToReturn != nullptr)
				willReturn = true;
		}
		else if (instruction->getType() == BreakInstruction)
		{
			if (this->__loopRunningCount > 0)
			{
				willReturn = true;
				this->__breakLoop = true;
			}
		}
		else if (instruction->getType() == ContinueInstruction)
		{
			if (this->__loopRunningCount > 0)
				willReturn = true;
		}
		else if (instruction->getType() == ReturnInstruction)
		{
			if (block == this->__mainBlock)
				throw std::runtime_error("Runtime Error: Cannot return outside Function.");

			auto returnObj = std::dynamic_pointer_cast<Return>(instruction);
			valueToReturn  = this->_evaluateExpression(returnObj->returnValue);
			willReturn     = true;
		}
		if (willReturn)
		{
			this->setupVariablePrevious();
			return valueToReturn;
		}
	}
	this->setupVariablePrevious();
	return nullptr;
}

// =================================================================================================
// EvaluateExpression
// 
// =================================================================================================
JDM_DLL
std::shared_ptr<HigherObject> Compiler::_getHigherObject(
	const std::shared_ptr<VarObjects> &Value,
	const std::shared_ptr<Expression> &Expression)
{
	if ( !Value )
		// If Value is nullptr then just return the value of expression
		return this->_evaluateExpression(Expression);
	// Get the value from Variable
	JDM::TokenType type = Value->getToken();

	if (type == JDM::TokenType::VARIABLE)
	{
		// Get the variable name of the current Variable
		auto varName = std::dynamic_pointer_cast<VariableObjects>(Value)->returnStringValue();

		// Check if it is a variable and get it's value from pair
		auto var  = this->__variable->variables.find(varName);
		if (var != this->__variable->variables.end())
			return std::make_shared<HigherObject>(var->second.second);

		// Check if it is a function
		auto func = this->__variable->functionMap.find(varName);
		if (func != this->__variable->functionMap.end())
			return std::make_shared<HigherObject>(func->second);

		auto nativeFunc = NativeFunction::allNativeFunction.find(varName);
		if (nativeFunc != NativeFunction::allNativeFunction.end())
		{
			auto newFunction = std::make_shared<FunctionCall>();
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
			vecList.push_back( this->_evaluateExpression(expr) );

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
			auto value = this->_evaluateExpression(expr->key);
			for (auto it = mapList.begin(); it != mapList.end(); ++it)
			{
				if (it->first->compareHigherObject(value))
				{
					value = it->first;
					break;
				}
			}
			if (value->getCurrActive() != ACTIVE_STRING
			 && value->getCurrActive() != ACTIVE_INTEGER
			 && value->getCurrActive() != ACTIVE_DECIMAL
			 && value->getCurrActive() != ACTIVE_BOOLEAN)
				std::runtime_error("Runtime Error: Only 'jstring', 'jint', 'jdouble' and 'jboolean' is allowed to be a key.");

			if (value->getIsReferenced())
				throw std::runtime_error("Runtime Error: Referenced cannot be a key.");

			// Used to create key and value using evaluateExpression
			mapList[value] = this->_evaluateExpression(expr->value);
		}
		return std::make_shared<HigherObject>(mapList);
	}
	// If the expression is Call Object, or function
	if (type == JDM::TokenType::CALL_OBJ)
	{
		// Get the root of CallObjects and get the result using _recursivelyCall
		auto callObjRoot = std::dynamic_pointer_cast<CallObjects>(Value);
		while (callObjRoot->prevObject != nullptr)
			callObjRoot = callObjRoot->prevObject;

		auto result = this->_recursivelyCall(callObjRoot);
		// if (result == nullptr)
		// 	return std::make_shared<HigherObject>(static_cast<int64_t>(0));
		return result;
	}
	// If the expression is a Lambda Expression
	// It is really not a function call so this is separate
	if (type == JDM::TokenType::LAMBDA)
	{
		auto lambdaObj   = std::dynamic_pointer_cast<LambdaObjects>(Value);
		auto newFunction = std::make_shared<FunctionCall>();
		for (const auto &var : lambdaObj->parameters)
		{
			if (var->dataType != nullptr)
			{
				auto dataT   = JDM::dataTypeMap.at(std::get<0>(var->dataType->token));
				newFunction->parameters[var->varName->returnStringValue()] = this->_returnVariable(var->varValue, dataT);
			}
			else
				newFunction->parameters[var->varName->returnStringValue()] = this->_returnVariable(
					var->varValue, DataTypeEnum::DATA_ANY);
		}
		newFunction->blockWillRun     = std::dynamic_pointer_cast<Block>(lambdaObj->blockWillRun);
		newFunction->varNameAccesible = this->_getAllVarName(this->__variable->variables);

		// If the lambda is being called, Then just call it, if not, make it like a function
		if (lambdaObj->willCall)
			return this->_runFunction(newFunction, this->_getVectorHigherObject(lambdaObj->arguments));
		else
			return std::make_shared<HigherObject>(newFunction);
	}
	if (type == JDM::TokenType::CAST)
	{
		auto castObj = std::dynamic_pointer_cast<CastObjects>(Value);
		return this->_castVariable(castObj->expression, castObj->datTypeToTurn, false);
	}
	return nullptr;
}

JDM_DLL
std::shared_ptr<HigherObject> &Compiler::_getVariableObject(
	const std::shared_ptr<Expression> &expr)
{
	if (!expr || expr->firstValue == nullptr)
		throw std::runtime_error("Runtime Error: Expecting a Variable to reference.");

	if (expr->opWillUse)
		throw std::runtime_error("Runtime Error: Expecting L Value not R Value.");

	auto Value = expr->firstValue;
	JDM::TokenType type = Value->getToken();
	if (type != JDM::TokenType::VARIABLE)
		throw std::runtime_error("Runtime Error: Expecting a Variable to reference.");

	auto varName = std::dynamic_pointer_cast<VariableObjects>(Value)->returnStringValue();
	auto var = this->__variable->variables.find(varName);
	if (var != this->__variable->variables.end())
		return var->second.second;

	throw std::runtime_error("Runtime Error: Variable is not declared.");
}

JDM_DLL
std::shared_ptr<HigherObject> Compiler::_evaluateExpression(
	const std::shared_ptr<Expression> &expr)
{
	if (!expr)
		return std::make_shared<HigherObject>(static_cast<int64_t>(0));

	auto firstVal = this->_getHigherObject(expr->firstValue, expr->firstExpression);
	if (firstVal == nullptr)
		throw std::runtime_error("Runtime Error: Invalid Expression. Return null.");

	if (expr->opWillUse)
	{
		if (firstVal->getIsReferenced())
			throw std::runtime_error("Runtime Error: Referenced cannot be used in operation.");

		if (expr->secondValue || expr->secondExpression)
		{
			auto second = this->_getHigherObject(expr->secondValue, expr->secondExpression);
			if (second->getIsReferenced())
				throw std::runtime_error("Runtime Error: Referenced cannot be used in operation.");

			firstVal = this->_newOperatedObject(firstVal, std::get<0>(expr->opWillUse->token), second);
		}
		else
			firstVal = this->_newOperatedObject(firstVal, std::get<0>(expr->opWillUse->token), nullptr);
	}
	return firstVal;
}

// =================================================================================================
// HandleNativeFunction
// 
// =================================================================================================
JDM_DLL
const std::shared_ptr<HigherObject> Compiler::_handleNativeFunction(
	NativeFunction::NativeFunctionEnum nativeType,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_FILTER)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments. Target ITERABLE and Function.");

		if (objects[1]->getCurrActive() != ACTIVE_FUNCTION)
			throw std::runtime_error("Runtime Error: Invalid Function arguments on 'filter'.");

		if (objects[0]->getCurrActive() != ACTIVE_LIST
		 && objects[0]->getCurrActive() != ACTIVE_MAP)
			return objects[0];
		else
		{
			auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();
			newList->listValue.erase(std::remove_if(newList->listValue.begin(), newList->listValue.end(),
				[&](const std::shared_ptr<HigherObject> &obj1)
				{
					std::shared_ptr<HigherObject> filterResult = this->_runFunction(objects[1]->funcValue, { obj1 });
					filterResult->castToBoolean(); return !filterResult->booleanValue;
				}),
			newList->listValue.end());
			return newList;
		}
	}
	else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_UNIQUE)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument. Target ITERABLE");

		if (objects[0]->getCurrActive() != ACTIVE_LIST && objects[0]->getCurrActive() != ACTIVE_MAP)
			return objects[0];
		else
		{
			auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();
			auto resultList = std::make_shared<HigherObject>(static_cast<int64_t>(0)); resultList->castToList();
			resultList->listValue.clear();
			for (int i = 0; i < newList->listValue.size(); i++)
			{
				if (!resultList->isInList(newList->listValue[i]))
					resultList->listValue.push_back(std::make_shared<HigherObject>(newList->listValue[i]));
			}
			return resultList;
		}
	}
	else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_REDUCE)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments. Target ITERABLE and Function.");

		if (objects[1]->getCurrActive() != ACTIVE_FUNCTION)
			throw std::runtime_error("Runtime Error: Invalid Function arguments on 'reduce'.");

		if (objects[0]->getCurrActive() != ACTIVE_LIST && objects[0]->getCurrActive() != ACTIVE_MAP)
			return objects[0];
		else
		{
			auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();
			if (newList->listValue.empty())
				throw std::runtime_error("Runtime Error: Using 'reduce' on empty ITERABLE.");

			auto result = newList->listValue[0];
			for (int i = 1; i < newList->listValue.size(); i++)
				result = this->_runFunction(objects[1]->funcValue, { result, newList->listValue[i] });
			return result;
		}
	}
	else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_MAP)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments. Target ITERABLE and Function.");

		if (objects[1]->getCurrActive() != ACTIVE_FUNCTION)
			throw std::runtime_error("Runtime Error: Invalid Function arguments on 'reduce'.");

		if (objects[0]->getCurrActive() != ACTIVE_LIST
		 && objects[0]->getCurrActive() != ACTIVE_MAP)
			return objects[0];
		else
		{
			auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();
			for (int i = 0; i < newList->listValue.size(); i++)
				newList->listValue[i] = this->_runFunction(objects[1]->funcValue, { newList->listValue[i] });
			return newList;
		}
	}
	else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_SORT_IF)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments. Target ITERABLE and Function.");

		if (objects[1]->getCurrActive() != ACTIVE_FUNCTION)
			throw std::runtime_error("Runtime Error: Invalid Function arguments on 'reduce'.");

		if (objects[0]->getCurrActive() != ACTIVE_LIST
		 && objects[0]->getCurrActive() != ACTIVE_MAP)
			return objects[0];
		else
		{
			auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();
			std::sort(newList->listValue.begin(), newList->listValue.end(),
				[&](const std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2)
				{
					std::shared_ptr<HigherObject> conditionResult = this->_runFunction(objects[1]->funcValue, { obj1, obj2 });
					conditionResult->castToBoolean(); return conditionResult->booleanValue;
				});
			return newList;
		}
	}
	else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_PARTIAL)
	{
		if (objects.size() < 1)
			throw std::runtime_error("Runtime Error: Expecting atleast 1 argument. Target Function.");

		if (objects[0]->getCurrActive() != ACTIVE_FUNCTION)
			throw std::runtime_error("Runtime Error: Invalid Function arguments on 'partial'.");

		auto newFunction              = std::make_shared<FunctionCall>();
		newFunction->funcName         = objects[0]->funcValue->funcName;
		newFunction->varNameAccesible = objects[0]->funcValue->varNameAccesible;
		newFunction->blockWillRun     = objects[0]->funcValue->blockWillRun;
		newFunction->parameters       = objects[0]->funcValue->parameters;
		newFunction->preArgs          = { objects.begin()+1, objects.end() };
		return std::make_shared<HigherObject>(newFunction);
	}
	else
		return NativeFunction::manageFunction( nativeType, objects );

	return nullptr;
}


// =================================================================================================
// HandleVariable
// 
// =================================================================================================
JDM_DLL
const std::vector<std::shared_ptr<HigherObject>> Compiler::_getVectorHigherObject(
	const std::vector<std::shared_ptr<Expression>> &expr)
{
	// Evaluate all expression in vector
	std::vector<std::shared_ptr<HigherObject>> result;
	for (const auto &ex : expr)
	{
		auto exRes = this->_evaluateExpression(ex);
		result.push_back(exRes);
	}
	return result;
}

JDM_DLL
const void Compiler::setupVariablePrevious()
{
	if (this->__variable->prev != nullptr)
	{
		this->__variable = this->__variable->prev;
		std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> tempMap;
		for (const auto& element : this->__variable->variables)
		{
			if (this->__variable->next->variables.count(element.first))
				tempMap[element.first] = this->__variable->next->variables.at(element.first);
			else
				tempMap[element.first] = element.second;
		}
		this->__variable->variables = tempMap;
		this->__variable->next = nullptr;
	}
}

JDM_DLL
const std::vector<std::string> Compiler::_getAllVarName(
	const std::unordered_map<std::string,
	std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_variables)
{
	std::vector<std::string> keys;
	for (const auto& pair : _variables)
	   keys.push_back(pair.first);
	return keys;
}

JDM_DLL
const void Compiler::_addVariable(
	const std::shared_ptr<Expression> &expr,
	DataTypeEnum dataT,
	const std::string &varName,
	bool isConst,
	bool isForce,
	const std::string &operation)
{
	this->__isAssigning = true;
	auto var = this->_evaluateExpression(expr);
	if (var->getIsReferenced())
	{
		if (operation != "=")
			throw std::runtime_error("Runtime Error: Referenced cannot be used in operation.");
	}
	if      (operation == "+=") var = this->_newOperatedObject(this->__variable->variables[varName].second, "+", var);
	else if (operation == "-=") var = this->_newOperatedObject(this->__variable->variables[varName].second, "-", var);
	else if (operation == "*=") var = this->_newOperatedObject(this->__variable->variables[varName].second, "*", var);
	else if (operation == "/=") var = this->_newOperatedObject(this->__variable->variables[varName].second, "/", var);
	else if (operation == "%=") var = this->_newOperatedObject(this->__variable->variables[varName].second, "%", var);

	// if (var->getCurrActive() == ACTIVE_FUNCTION)
	// {

	// }

	if (var->getIsReferenced())
	{
		if (dataT != DataTypeEnum::DATA_ANY && var->getDatatypeEnum() != dataT)
			throw std::runtime_error("Runtime Error: Referenced value must be the same data type.");

		if (var->isConstant != isConst)
			throw std::runtime_error("Runtime Error: Cannot convert constant to non-constant or non-constant to constant");
	}
	else
	{
		var->isConstant         = isConst;
		var->isForcedConstraint = isForce;
		var = this->_checkVariableConstraint(var, dataT);
	}

	if (operation != "=") return;

	// Get the old Variable and just set it's new value to this
	auto oldVar  = this->__variable->variables.find(varName);
	if (oldVar != this->__variable->variables.end())
	{
		// Use the setHigherObject to set the value of variable to var
		if (oldVar->second.second->getIsReferenced() && var->getIsReferenced())
			throw std::runtime_error("Runtime Error: Variable " + varName + " is already a reference.");

		oldVar->second.second->setHigherObject(var);
		this->__variable->variables[varName] = std::make_pair(dataT, oldVar->second.second);
		return;
	}
	if (var->isForcedConstraint && dataT == DataTypeEnum::DATA_ANY)
		this->__variable->variables[varName] = std::make_pair(var->getDatatypeEnum(), var);
	else
		this->__variable->variables[varName] = std::make_pair(dataT, var);
}

JDM_DLL
const std::pair<DataTypeEnum, std::shared_ptr<HigherObject>> Compiler::_returnVariable(
	const std::shared_ptr<Expression> &expr,
	DataTypeEnum dataT)
{
	auto var = this->_castVariable(expr, dataT);
	return std::make_pair(dataT, var);
}

JDM_DLL
const std::shared_ptr<HigherObject> Compiler::_castVariable(
	const std::shared_ptr<Expression> &expr,
	DataTypeEnum dataT,
	bool checkConstraint)
{
	auto var = this->_evaluateExpression(expr);
	if (var->getIsReferenced())
		throw std::runtime_error("Runtime Error: Cannot cast when declaring referenced.");

	return this->_checkVariableConstraint(var, dataT, checkConstraint);
}

JDM_DLL
const std::shared_ptr<HigherObject> Compiler::_checkVariableConstraint(
	const std::shared_ptr<HigherObject> &var,
	DataTypeEnum dataT,
	bool checkConstraint)
{
	if (var->isForcedConstraint && dataT != DataTypeEnum::DATA_ANY && checkConstraint)
	{
		DataTypeEnum varType = var->getDatatypeEnum();
		if (dataT != varType)
		{
			if (dataT == DataTypeEnum::DATA_STRING)
				throw std::runtime_error("Runtime Error: Due to force constraint. Value must be String.");
			else if (dataT == DataTypeEnum::DATA_INTEGER
				  || dataT == DataTypeEnum::DATA_DOUBLE
				  || dataT == DataTypeEnum::DATA_BOOLEAN)
				throw std::runtime_error("Runtime Error: Due to force constraint. Value must be Number.");
			else if (dataT == DataTypeEnum::DATA_LAMBDA)
				throw std::runtime_error("Runtime Error: Due to force constraint. Value must be Function or Lambda.");
			else if (dataT == DataTypeEnum::DATA_OBJECT)
				throw std::runtime_error("Runtime Error: Due to force constraint. Value must be Object.");
			else if (dataT == DataTypeEnum::DATA_LIST)
				throw std::runtime_error("Runtime Error: Due to force constraint. Value must be List.");
			else if (dataT == DataTypeEnum::DATA_MAP)
				throw std::runtime_error("Runtime Error: Due to force constraint. Value must be Map.");
		}	
	}
	if      (dataT == DataTypeEnum::DATA_INTEGER) var->castToInteger ();
	else if (dataT == DataTypeEnum::DATA_STRING ) var->castToString  ();
	else if (dataT == DataTypeEnum::DATA_BOOLEAN) var->castToBoolean ();
	else if (dataT == DataTypeEnum::DATA_DOUBLE ) var->castToDecimal ();
	else if (dataT == DataTypeEnum::DATA_LAMBDA ) var->castToFunction();
	else if (dataT == DataTypeEnum::DATA_OBJECT ) var->castToObject  ();
	else if (dataT == DataTypeEnum::DATA_LIST   ) var->castToList    ();
	else if (dataT == DataTypeEnum::DATA_MAP    ) var->castToMap     ();
	return var;
}

// =================================================================================================
// InstructionObject
// 
// =================================================================================================
JDM_DLL
const void Compiler::_doDeclarationInstruction(const std::shared_ptr<Instruction> &instruction)
{
	auto declare = std::dynamic_pointer_cast<Declaration>(instruction);
	auto dataT   = JDM::dataTypeMap.at(std::get<0>(declare->dataType->token));

	auto varName = declare->varName->returnStringValue();
	auto var    = this->__variable->variables.find(varName);
	if ( var != this->__variable->variables.end() )
		throw std::runtime_error("Runtime Error: Variable '" + varName
			+ "' is already declared. Redeclaration is not allowed.");

	auto func = this->__variable->functionMap.find(varName);
	if (func != this->__variable->functionMap.end())
		throw std::runtime_error("Runtime Error: Variable '" + varName
			+ "' is already declared as a function. Redeclaration is not allowed.");

	auto nativeFunc = NativeFunction::allNativeFunction.find(varName);
	if (nativeFunc != NativeFunction::allNativeFunction.end())
		throw std::runtime_error("Runtime Error: Variable '" + varName
			+ "' is already declared as a native function. Redeclaration is not allowed.");

	this->_addVariable(
		declare->expression,
		dataT,
		varName,
		declare->isConst,
		declare->isForce
	);
}

JDM_DLL
const void Compiler::_doAssignmentInstruction(const std::shared_ptr<Instruction> &instruction)
{
	auto assign = std::dynamic_pointer_cast<Assignment>(instruction);
	auto var    = this->__variable->variables.find(assign->varName->returnStringValue());
	if ( var != this->__variable->variables.end() )
	{
		if (var->second.second->isConstant)
			throw std::runtime_error("Runtime Error: Variable is Constant.");

		this->_addVariable(
			assign->expression,
			var->second.first,
			assign->varName->returnStringValue(),
			var->second.second->isConstant,
			var->second.second->isForcedConstraint,
			std::get<0>(assign->operation->token)
		);
	}
	else
		throw std::runtime_error("Runtime Error: Variable is not declared.");
}

JDM_DLL
const void Compiler::_doLoggerInstruction(const std::shared_ptr<Instruction> &instruction)
{
	this->__isAssigning = true;
	auto logger = std::dynamic_pointer_cast<Logger>(instruction);
	for (const auto &expr : logger->expressions)
	{
		auto resultExpr = this->_evaluateExpression(expr);
		if (resultExpr == nullptr)
			throw std::runtime_error("Runtime Error: Invalid Expression.");

		resultExpr->logValue();
		if (logger->addNewLine)
			Log << '\n';
	}
}

JDM_DLL
const void Compiler::_doCFunctionInstruction(const std::shared_ptr<Instruction> &instruction)
{
	auto cfunction = std::dynamic_pointer_cast<CFunction>(instruction);

	if (cfunction->funcType == CUSFUNC_CLEAR)
		system("cls");

	else if (cfunction->funcType == CUSFUNC_SLEEP)
	{
		// Evaluate and then sleep
		auto timeML = this->_evaluateExpression(cfunction->expression);
		timeML->castToInteger();
		std::this_thread::sleep_for(std::chrono::milliseconds(timeML->integerValue));
	}

	// TO DO WILL CREATE INCLDUE ON STRING
	// FOR PATH, IT MIGHT BECOME IMPORT BUT WHO KNOWS
	else if (cfunction->funcType == CUSFUNC_INCLUDE)
	{
		// Since we don't know the class name of variable
		// My code first treat it as a random variable
		auto varName   = std::dynamic_pointer_cast<VariableObjects>(
			cfunction->expression->firstValue)->returnStringValue();
		auto className = this->__nativeClassMap.find(varName);
		if (className != this->__nativeClassMap.end())
		{
			auto newClass = std::make_shared<ClassObject>();
			newClass->className = varName;

			// Create a HigherObject as a Class
			auto var = std::make_shared<HigherObject>(newClass);
			var->isForcedConstraint = true;
			var->isConstant         = true;

			// Set variable to global variables scope
			this->__variable->variables[varName] = std::make_pair(DataTypeEnum::DATA_OBJECT,
				this->_checkVariableConstraint(var, DataTypeEnum::DATA_OBJECT));
			this->__allInclude.push_back(varName);
		}
		else
			// If it is not valid include. Return error
			throw std::runtime_error("Runtime Error: Cannot find the thing you want to included.");
	}
}

JDM_DLL
const void Compiler::_processCreateFunction(const std::shared_ptr<Instruction> &instruction)
{
	auto createFunc = std::dynamic_pointer_cast<CreateFunction>(instruction);
	std::shared_ptr<FunctionCall> newFunction = std::make_shared<FunctionCall>();
	for (const auto &var : createFunc->parameters)
	{
		if (var->dataType != nullptr)
		{
			auto dataT = JDM::dataTypeMap.at(std::get<0>(var->dataType->token));
			newFunction->parameters[var->varName->returnStringValue()] = this->_returnVariable(var->varValue, dataT);
		}
		else
			newFunction->parameters[var->varName->returnStringValue()] = this->_returnVariable(var->varValue, DataTypeEnum::DATA_ANY);
	}
	auto functionName                         = createFunc->functionName->returnStringValue();
	newFunction->funcName                     = functionName;
	newFunction->blockWillRun                 = createFunc->blockWillRun;
	newFunction->varNameAccesible             = this->_getAllVarName(this->__variable->variables);
	this->__variable->functionMap[functionName] = newFunction;
}

JDM_DLL
const void Compiler::_processCallFunction(const std::shared_ptr<Instruction> &instruction)
{
	auto callFunc    = std::dynamic_pointer_cast<Call>(instruction);
	auto callObjRoot = callFunc->callObj;
	while (callObjRoot->prevObject != nullptr)
		callObjRoot  = callObjRoot->prevObject;

	this->__isAssigning = true;
	this->_recursivelyCall(callObjRoot, callFunc->expression);
}

JDM_DLL
const std::shared_ptr<HigherObject> Compiler::_processIfStatement(const std::shared_ptr<Instruction> &instruction)
{
	if (instruction == nullptr)
		return nullptr;

	auto ifState = std::dynamic_pointer_cast<IfStatement>(instruction);
	if (ifState->condition != nullptr)
	{
		this->__isAssigning = true;
		auto condition = this->_evaluateExpression(ifState->condition);
		condition->castToBoolean();

		if (!condition->booleanValue)
			return this->_processIfStatement(ifState->elseIf);
	}
	return this->compile(ifState->blockWillRun, this->__variable->variables, {}, this->__variable->functionMap);
}

JDM_DLL
const std::shared_ptr<HigherObject> Compiler::_processForStatement(const std::shared_ptr<Instruction> &instruction)
{
	this->__isAssigning = true;
	auto forState = std::dynamic_pointer_cast<ForLoopStatement>(instruction);

	// Setup the for loop
	// The start, step, and stop
	std::shared_ptr<HigherObject> start;
	std::shared_ptr<HigherObject> step ;
	std::shared_ptr<HigherObject> stop = this->_evaluateExpression(forState->stop);

	if (forState->start != nullptr)
		start = this->_evaluateExpression(forState->start);
	if (forState->step  != nullptr)
		step  = this->_evaluateExpression(forState->step);
	if (start == nullptr)
		start = std::make_shared<HigherObject>(static_cast<int64_t>(0));
	if (step  == nullptr)
		step  = std::make_shared<HigherObject>(static_cast<int64_t>(1));

	start->castToInteger();
	step ->castToInteger();
	stop ->castToInteger();

	// The additionalVariables that will be add in iteration
	std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> additionalVariables;
	additionalVariables[forState->variable->returnStringValue()] = std::make_pair(DataTypeEnum::DATA_INTEGER, start);
	// While loop the loop process.
	this->__loopRunningCount++;
	while (start->integerValue != stop->integerValue)
	{
		// Remember compile return a value, this is useful because sometimes you want to continue in loop or break
		auto returnValue = this->compile(
			forState->blockWillRun,
			this->__variable->variables,
			additionalVariables,
			this->__variable->functionMap
		);
		if (this->__breakLoop)
		{
			this->__loopRunningCount--;
			this->__breakLoop = false;
			return nullptr;
		}
		if (returnValue != nullptr)
		{
			this->__loopRunningCount--;
			return returnValue;
		}
		start->integerValue += step->integerValue;
	}
	this->__loopRunningCount--;
	return nullptr;
}

JDM_DLL
const std::shared_ptr<HigherObject> Compiler::_processWhileLoop(const std::shared_ptr<Instruction> &instruction)
{
	this->__isAssigning = true;
	auto whileState = std::dynamic_pointer_cast<WhileStatement>(instruction);
	auto expr       = this->_evaluateExpression(whileState->condition);
	expr->castToBoolean(); bool check = expr->booleanValue;
	this->__loopRunningCount++;
	while (check)
	{
		auto returnValue = this->compile( whileState->blockWillRun,
			this->__variable->variables, {}, this->__variable->functionMap);
		if (this->__breakLoop)
		{
			this->__breakLoop = false;
			this->__loopRunningCount--;
			return nullptr;
		}
		if (returnValue != nullptr)
		{
			this->__loopRunningCount--;
			return returnValue;
		}
		auto expr = this->_evaluateExpression(whileState->condition);
		expr->castToBoolean();
		check = expr->booleanValue;
	}
	this->__loopRunningCount--;
	return nullptr;
}


JDM_DLL
const std::shared_ptr<HigherObject> Compiler::_processForEachStatement(const std::shared_ptr<Instruction> &instruction)
{
	this->__isAssigning = true;
	auto foreachState = std::dynamic_pointer_cast<ForEachStatement>(instruction);
	std::shared_ptr<HigherObject> realValue = nullptr;
	std::shared_ptr<HigherObject> key       = nullptr;
	std::shared_ptr<HigherObject> value     = nullptr;

	bool isReverse = foreachState->isReverse;
	bool isMap     = foreachState->varVal != nullptr;

	if (foreachState->varToRun != nullptr)
	{
		auto variables = this->__variable->variables.find(foreachState->varToRun->returnStringValue());
		if (variables == this->__variable->variables.end())
			throw std::runtime_error("Runtime Error: Variable is not declared.");

		realValue = variables->second.second;
		if (realValue->getCurrActive() != ACTIVE_LIST && realValue->getCurrActive() != ACTIVE_MAP)
			throw std::runtime_error("Runtime Error: Variable is not a 'jlist' or 'jmap'.");
	}
	else if (foreachState->expression)
	{
		realValue = this->_evaluateExpression(foreachState->expression);
		if (realValue->getCurrActive() != ACTIVE_LIST && realValue->getCurrActive() != ACTIVE_MAP)
			throw std::runtime_error("Runtime Error: Invalid 'jlist' | 'jmap' to run.");
	}
	else
		throw std::runtime_error("Runtime Error: Invalid 'jlist' | 'jmap' to run.");


	int index = (isReverse ? realValue->listValue.size() - 1 : 0);
	auto mapIter = realValue->mapValue.begin();

	if (realValue->getCurrActive() == ACTIVE_LIST)
	{
		if (index < 0 || index >= realValue->listValue.size())
			return nullptr;
		key = std::make_shared<HigherObject>(static_cast<int64_t>(index));
		value = std::make_shared<HigherObject>(realValue->listValue[index]);
	}
	else
	{		
		if (mapIter == realValue->mapValue.end())
			return nullptr;

		key = std::make_shared<HigherObject>(mapIter->first);
		value = std::make_shared<HigherObject>(mapIter->second);
	}

	std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> additionalVariables;
	if (isMap)
	{
		// If it is map, then just set it
		additionalVariables[foreachState->varKey->returnStringValue()] = std::make_pair(DataTypeEnum::DATA_ANY, key);
		additionalVariables[foreachState->varVal->returnStringValue()] = std::make_pair(DataTypeEnum::DATA_ANY, value);
	}
	else
	{
		// If it is list, then use value as a key and normal on map
		if (realValue->getCurrActive() == ACTIVE_LIST)
			additionalVariables[foreachState->varKey->returnStringValue()] = std::make_pair(DataTypeEnum::DATA_ANY, value);
		else
			additionalVariables[foreachState->varKey->returnStringValue()] = std::make_pair(DataTypeEnum::DATA_ANY, key);
	}

	this->__loopRunningCount++;
	while (true)
	{
		auto returnValue = this->compile(
			foreachState->blockWillRun,
			this->__variable->variables,
			additionalVariables,
			this->__variable->functionMap
		);
		if (this->__breakLoop)
		{
			this->__loopRunningCount--;
			this->__breakLoop = false;
			return nullptr;
		}
		if (returnValue != nullptr)
		{
			this->__loopRunningCount--;
			return returnValue;
		}

		if (realValue->getCurrActive() == ACTIVE_MAP)
		{
			if (mapIter == realValue->mapValue.end())
				break;
			++mapIter;
			if (mapIter == realValue->mapValue.end())
				break;

			key->setHigherObject(mapIter->first);
			value->setHigherObject(mapIter->second);
		}
		else
		{
			if (isReverse)
			{
				index--;
				if (index < 0)
					break;
			}
			else
			{
				index++;
				if (index >= realValue->listValue.size())
					break;
			}
			key->integerValue = index;
			value->setHigherObject(realValue->listValue[index]);
		}
	}
	this->__loopRunningCount--;
	return nullptr;
}

// =================================================================================================
// ManageCallBracket
// 
// =================================================================================================
JDM_DLL
const std::shared_ptr<HigherObject> Compiler::_manageCallBrackets(
	const std::shared_ptr<CallObjects > &next,
	const std::shared_ptr<HigherObject> &returnVal,
	const std::shared_ptr<Expression  > &expressionAssign)
{
	auto exprObj = std::dynamic_pointer_cast<ExpressionObjects>(next->currObject);
	auto expr    = this->_evaluateExpression(exprObj->expression); // Evaluate the expression
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

			this->__isAssigning = true;
			auto exprResult = this->_evaluateExpression(expressionAssign);
			auto oldValue   = returnVal->listValue[index];
			if (next->operation == "+=")
				exprResult = this->_newOperatedObject(oldValue, "+", exprResult);
			else if (next->operation == "-=")
				exprResult = this->_newOperatedObject(oldValue, "-", exprResult);
			else if (next->operation == "*=")
				exprResult = this->_newOperatedObject(oldValue, "*", exprResult);
			else if (next->operation == "/=")
				exprResult = this->_newOperatedObject(oldValue, "/", exprResult);
			else if (next->operation == "%=")
				exprResult = this->_newOperatedObject(oldValue, "%", exprResult);

			if (returnVal->listValue[index]->isConstant)
				throw std::runtime_error("Runtime Error: Variable is Constant.");
			this->_checkVariableConstraint(returnVal->listValue[index], exprResult->getDatatypeEnum());

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

			this->__isAssigning = true;
			auto exprResult = this->_evaluateExpression(expressionAssign);
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

			this->__isAssigning = true;
			auto exprResult = this->_evaluateExpression(expressionAssign);
			if (key != nullptr)
			{
				auto oldValue = returnVal->mapValue.at(key);
				if (next->operation == "+=")
					exprResult = this->_newOperatedObject(oldValue, "+", exprResult);
				else if (next->operation == "-=")
					exprResult = this->_newOperatedObject(oldValue, "-", exprResult);
				else if (next->operation == "*=")
					exprResult = this->_newOperatedObject(oldValue, "*", exprResult);
				else if (next->operation == "/=")
					exprResult = this->_newOperatedObject(oldValue, "/", exprResult);
				else if (next->operation == "%=")
					exprResult = this->_newOperatedObject(oldValue, "%", exprResult);
			}

			if (key != nullptr)
			{
				if (returnVal->mapValue[key]->isConstant)
					throw std::runtime_error("Runtime Error: Variable is Constant.");
				this->_checkVariableConstraint(returnVal->mapValue[key], exprResult->getDatatypeEnum());

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

// =================================================================================================
// ManageEndCall
// 
// =================================================================================================
// This is the return value on what you call. Example the return value of function is list.
// Then you can still do operation because. you know, you can?.
JDM_DLL
const std::shared_ptr<HigherObject> Compiler::_manageEndCall(
	const std::shared_ptr<CallObjects> &callObj,
	std::shared_ptr<HigherObject> &returnVal,
	const std::shared_ptr<Expression> &expressionAssign)
{
	// Setup new return
	std::shared_ptr<HigherObject> newReturn;
	auto newCallObj = callObj->nextObject;

	// If there is no call object, just return the value
	if (newCallObj == nullptr)
		return std::make_shared<HigherObject>(returnVal);

	// The chance of this happening is low
	// because If my lexer and parser do their job properly, it will work flawlessly.
	if (returnVal == nullptr)
		throw std::runtime_error("Runtime Error: Return Value cannot be called or act like class.");

	// Get the next object
	auto newCurrObj = newCallObj->currObject;
	auto tok        = newCurrObj->getToken();
	if (returnVal->getCurrActive() == ACTIVE_FUNCTION)
	{
		if (tok != JDM::TokenType::FUNCTIONS)
			// This will happen because, example a function has been return
			// the you do this
			/*
			jfunc test() => {
				jreturn () => { $logn => "TEST"; }; 
			}
			$logn => test().test();
			*/
			throw std::runtime_error("Runtime Error: Return Value is Function. Not a class.");
		// Get the function object of the newCallObject
		auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(newCurrObj);
		if (functionObj->returnStringValue() != "(")
			throw std::runtime_error("Runtime Error: Return Value is Function. Not a class.");

		this->__isAssigning = true;
		return this->_runFunction(returnVal->funcValue, this->_getVectorHigherObject(functionObj->arguments));
	}
	// This thing is still not properly coded or implemented.
	// I will do this bit later or next time
	// TO DO
	else if (returnVal->getCurrActive() == ACTIVE_OBJECT)
	{
		if (tok == JDM::TokenType::FUNCTIONS)
		{
			auto classFunc   = this->__nativeClassMap.find(returnVal->objectValue->className);
			if (classFunc != this->__nativeClassMap.end())
			{
				auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(newCurrObj);

				// Will never be true, as recursively call, will handle it, but just in case.
				if (functionObj->returnStringValue() == "(")
					newReturn = classFunc->second->constructor(returnVal, this->_getVectorHigherObject(functionObj->arguments));
				else
				{
					// This still not really cannot do anything.
					auto func = classFunc->second->mapFunctions.find(functionObj->returnStringValue());
					if (func == classFunc->second->mapFunctions.end())
						throw std::runtime_error("Runtime Error: This method is not a member of class '" + returnVal->objectValue->className + "'.");

					this->__isAssigning = true;
					newReturn = classFunc->second->manageFunction(func->second, returnVal, this->_getVectorHigherObject(functionObj->arguments));
				}
			}
		}
		// This will be used when the object next is a variable.
		// I will get the value of the next obj, and call the manageEnd again.
		// TO DO
		else if (tok == JDM::TokenType::VARIABLE)
		{
			
		}
	}
	// If the return value is string, do something, make it act like a string
	else if (returnVal->getCurrActive() == ACTIVE_STRING)
	{
		if (tok == JDM::TokenType::EXPRESSION)
			// If the next token is like this
			/*
			jfunc test() => {
				jreturn "TEST"; 
			}
			$log => test()[0];
			*/
			newReturn = this->_manageCallBrackets(newCallObj, returnVal, expressionAssign);

		else if (tok == JDM::TokenType::FUNCTIONS)
		{
			// If the next token is something like this
			/*
			jfunc test() => {
				jreturn "Hello World";
			}
			$logn => test().split();
			*/
			auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(newCurrObj);
			if (functionObj->returnStringValue() == "(")
				// If the return value is string then you call it.
				/*
				jfunc test() => {
					jreturn "test";
				}
				$logn => test()();
				*/
				throw std::runtime_error("Runtime Error: A String cannot be called.");

			auto func = StringHigherFunctions::stringFunctions.find(functionObj->returnStringValue());
			if (func == StringHigherFunctions::stringFunctions.end())
				// If the return value is string then you call it and the function is not member or valid func.
				/*
				jfunc test() => {
					jreturn "test";
				}
				$logn => test().test();
				*/
				throw std::runtime_error("Runtime Error: This function is not a member of class 'jstring'.");

			this->__isAssigning = true;
			newReturn = StringHigherFunctions::manageFunction(
				func->second, returnVal,
				this->_getVectorHigherObject(functionObj->arguments)
			);
		}
	}
	else if (returnVal->getCurrActive() == ACTIVE_LIST || returnVal->getCurrActive() == ACTIVE_MAP)
	{
		if (tok == JDM::TokenType::EXPRESSION)
		{
			// This will manage when you want to know the index of something
			newReturn = this->_manageCallBrackets(newCallObj, returnVal, expressionAssign);
		}

		else if (tok == JDM::TokenType::FUNCTIONS)
		{
			auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(newCurrObj);
			if (returnVal->getCurrActive() == ACTIVE_LIST)
			{
				if (functionObj->returnStringValue() == "(")
					throw std::runtime_error("Runtime Error: A 'jlist' cannot be called.");

				// jlist native function. Acts like a class.
				auto func = ListHigherFunctions::listFunctions.find(functionObj->name);						
				if (func == ListHigherFunctions::listFunctions.end())
					// If the function is not a member of jlist
					/*
					jfunc test() => {
						jreturn [1, 2, 3, 4, 5]; 
					}
					$log => test().test();
					*/
					throw std::runtime_error("Runtime Error: This function is not a member of class 'jlist'.");

				this->__isAssigning = true;
				newReturn = ListHigherFunctions::manageFunction(
					func->second, returnVal,
					this->_getVectorHigherObject(functionObj->arguments)
				);
			}
			else // Return Value is Map
			{
				if (functionObj->returnStringValue() == "(")
					throw std::runtime_error("Runtime Error: A 'jmap' cannot be called.");

				// jmap native function. Acts like a class.
				auto func = MapHigherFunctions::mapFunctions.find(functionObj->name);						
				if (func == MapHigherFunctions::mapFunctions.end())
					// If the function is not a member of jmap
					/*
					jfunc test() => {
						jreturn { "test" => 10 }; 
					}
					$log => test().test();
					*/
					throw std::runtime_error("Runtime Error: This function is not a member of class 'jmap'.");

				this->__isAssigning = true;
				newReturn = MapHigherFunctions::manageFunction(
					func->second, returnVal,
					this->_getVectorHigherObject(functionObj->arguments)
				);
			}
		}
	}
	else
		// When the return value is not what you expect.
		// This soon will be remove, because after I add native for other data type
		// Example I add on integer, float or boolean
		throw std::runtime_error("Runtime Error: Return Value cannot be called or act like class.");

	// Return the new Value calculated above
	return this->_manageEndCall(newCallObj, newReturn);
}


// =================================================================================================
// OperatedObject
// 
// =================================================================================================
JDM_DLL
std::shared_ptr<HigherObject> Compiler::_newOperatedObject(
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

	auto cActive = firstVal->getCurrActive();
	if      (cActive == ACTIVE_STRING  ) this->_handleNewOperatedString(firstVal, operation, secondVal);
	else if (cActive == ACTIVE_LIST    ) this->_handleNewOperatedList  (firstVal, operation, secondVal);
	else if (cActive == ACTIVE_MAP     ) this->_handleNewOperatedMap   (firstVal, operation, secondVal);
	else if (cActive == ACTIVE_FUNCTION) this->_handleNewOperatedFunc  (firstVal, operation, secondVal);
	else if (cActive == ACTIVE_OBJECT  ) this->_handleNewOperatedObject(firstVal, operation, secondVal);
	else if (cActive == ACTIVE_INTEGER
		  || cActive == ACTIVE_DECIMAL
		  || cActive == ACTIVE_BOOLEAN)
		// Handle number types (integer, decimal, boolean)
		this->_handleNewOperatedNumber(firstVal, operation, secondVal);

	// Return the modified first value
	return firstVal;
}

JDM_DLL
void Compiler::_handleNewOperatedString(
	std::shared_ptr<HigherObject> &firstVal,
	const std::string &operation,
	const std::shared_ptr<HigherObject> &secondVal)
{
	auto cActive = secondVal->getCurrActive();
	if (operation == "+")
	{
		secondVal->castToString();
		firstVal->stringValue += secondVal->stringValue;
	}
	else if (operation == "*")
	{
		if (cActive != ACTIVE_INTEGER && cActive != ACTIVE_DECIMAL)
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
		if (cActive != ACTIVE_STRING)
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
void Compiler::_handleNewOperatedList(
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
		if (secondVal->getCurrActive() != ACTIVE_LIST)
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
void Compiler::_handleNewOperatedMap(
	std::shared_ptr<HigherObject> &firstVal,
	const std::string &operation,
	const std::shared_ptr<HigherObject> &secondVal)
{
	if (secondVal->getCurrActive() != ACTIVE_MAP)
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
void Compiler::_handleNewOperatedFunc(
	std::shared_ptr<HigherObject> &firstVal,
	const std::string &operation,
	const std::shared_ptr<HigherObject> &secondVal)
{
	if (secondVal->getCurrActive() != ACTIVE_FUNCTION)
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
void Compiler::_handleNewOperatedNumber(
	std::shared_ptr<HigherObject> &firstVal,
	const std::string &operation,
	const std::shared_ptr<HigherObject> &secondVal)
{
	auto cActive = secondVal->getCurrActive();
	if (cActive == ACTIVE_STRING  ) throw std::runtime_error("Runtime Error: Cannot do operation on Number and 'jstring'.");
	if (cActive == ACTIVE_OBJECT  ) throw std::runtime_error("Runtime Error: Cannot do operation on Number and 'jobject'.");
	if (cActive == ACTIVE_FUNCTION) throw std::runtime_error("Runtime Error: Cannot do operation on Number and 'func'.");
	if (cActive == ACTIVE_LIST    ) throw std::runtime_error("Runtime Error: Cannot do operation on Number and 'jlist.");
	if (cActive == ACTIVE_MAP     ) throw std::runtime_error("Runtime Error: Cannot do operation on Number and 'jmap'.");

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
void Compiler::_handleNewOperatedObject(
	std::shared_ptr<HigherObject> &firstVal,
	const std::string &operation,
	const std::shared_ptr<HigherObject> &secondVal)
{
	if (secondVal->getCurrActive() != ACTIVE_OBJECT)
		throw std::runtime_error("Runtime Error: Cannot do operation on 'jobject' and not 'jobject'.");

	bool result = Comparator::DoOperationObject(
		firstVal,
		secondVal,
		operation
	);
	firstVal->castToBoolean();
	firstVal->booleanValue = result;
}

// =================================================================================================
// RecursivelyCall
// 
// =================================================================================================
JDM_DLL
const std::shared_ptr<HigherObject> Compiler::_recursivelyCall(
	const std::shared_ptr<CallObjects> &callObj,
	const std::shared_ptr<Expression> &expressionAssign)
{
	auto tok = callObj->currObject->getToken();
	std::shared_ptr<HigherObject> returnValue = nullptr;

	if (tok == JDM::TokenType::FUNCTIONS)
	{
		auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(callObj->currObject);
		std::shared_ptr<FunctionCall> newFunc = nullptr;
		auto variables = this->__variable->variables  .find(functionObj->returnStringValue());
		auto function  = this->__variable->functionMap.find(functionObj->returnStringValue());

		if (function != this->__variable->functionMap.end())
			newFunc = function->second;

		// Used to run a lambda
		else if (variables != this->__variable->variables.end() && variables->second.second->getCurrActive() == ACTIVE_FUNCTION)
			newFunc = variables->second.second->funcValue;

		else if (variables != this->__variable->variables.end() && variables->second.second->getCurrActive() == ACTIVE_OBJECT)
		{
			for (auto &e : this->__allInclude)
			{
				if (e.compare(variables->second.second->objectValue->className) == 0)
				{
					// Check if the object is callable
					if (!variables->second.second->objectValue->fromMainSource)
						throw std::runtime_error("Runtime Error: This object is not callable.");

					auto newObj = std::make_shared<HigherObject>();
					newObj->castToObject();
					newObj->objectValue->className = variables->second.second->objectValue->className;
					newObj->objectValue->members   = variables->second.second->objectValue->members;
					newObj->objectValue->methods   = variables->second.second->objectValue->methods;
					newObj->objectValue->fromMainSource = false;
					returnValue = this->__nativeClassMap[e]->constructor(newObj, this->_getVectorHigherObject(functionObj->arguments));

					return this->_manageEndCall(callObj, returnValue, expressionAssign);
				}
			}
			throw std::runtime_error("Runtime Error: No CONSTRUCTOR implemented yet.");
		}
		else
		{
			auto nativeFunc = NativeFunction::allNativeFunction.find(functionObj->returnStringValue());
			if (nativeFunc != NativeFunction::allNativeFunction.end())
			{
				if (nativeFunc->second == NativeFunction::NativeFunctionEnum::NATIVE_REFERENCE)
				{
					if (callObj->prevObject != nullptr)
						throw std::runtime_error("Runtime Error: Invalid usage of 'ref' inside the expression.");

					if (callObj->nextObject != nullptr)
						throw std::runtime_error("Runtime Error: Reference can't have recursive call.");

					if (!this->__isAssigning)
						throw std::runtime_error("Runtime Error: Reference keyword must be only use in Assignment and Declaration");

					if (functionObj->arguments.size() != 1)
						throw std::runtime_error("Runtime Error: Reference expect L Value not R Value..");

					auto varReferenced = this->_getVariableObject(functionObj->arguments[0]);
					varReferenced->setIsReferenced(true);
					return varReferenced;
				}
				else
					returnValue = this->_handleNativeFunction(nativeFunc->second,
						this->_getVectorHigherObject(functionObj->arguments));

				return this->_manageEndCall( callObj, returnValue, expressionAssign );
			}
			else throw std::runtime_error("Runtime Error: Function is not declared.");
		}
		if (newFunc == nullptr)
			throw std::runtime_error("Runtime Error: Invalid Function.");

		returnValue = this->_runFunction(newFunc, this->_getVectorHigherObject(functionObj->arguments));
	}
	else if (tok == JDM::TokenType::EXPRESSION)
	{
		// Get the expression object
		auto exprObj = std::dynamic_pointer_cast<ExpressionObjects>(callObj->currObject);
		auto variables = this->__variable->variables.find(exprObj->returnStringValue());
		if (variables == this->__variable->variables.end())
			throw std::runtime_error("Runtime Error: Variable is not declared.");

		auto varList = variables->second.second;
		if (varList->getCurrActive() == ACTIVE_LIST
		 || varList->getCurrActive() == ACTIVE_MAP
		 || varList->getCurrActive() == ACTIVE_STRING)
			returnValue = this->_manageCallBrackets(callObj, varList, expressionAssign);
		else
			throw std::runtime_error("Runtime Error: Variable is not a String, List or a Map.");
	}
	else if (tok == JDM::TokenType::VARIABLE)
	{
		auto var = std::dynamic_pointer_cast<VariableObjects>(callObj->currObject);
		auto variables = this->__variable->variables.find(var->returnStringValue());
		if (variables == this->__variable->variables.end())
			throw std::runtime_error("Runtime Error: Variable is not declared.");

		returnValue = variables->second.second;
	}
	else if (tok == JDM::TokenType::STRING)
		returnValue = std::make_shared<HigherObject>(callObj->currObject->returnStringValue());

	return this->_manageEndCall(callObj, returnValue, expressionAssign);
}

// =================================================================================================
// RunFunction
// 
// =================================================================================================
JDM_DLL
const std::shared_ptr<HigherObject> Compiler::_runFunction(
	const std::shared_ptr<FunctionCall> &newFunc,
	const std::vector<std::shared_ptr<HigherObject>> &arguments)
{
	std::vector<std::shared_ptr<HigherObject>> newArgVec = { arguments.begin(), arguments.end() };
	newArgVec.insert(newArgVec.end(), newFunc->preArgs.begin(), newFunc->preArgs.end());
	if (newFunc->funcName.find("$native-") == 0)
	{
		auto nativeFunc = NativeFunction::allNativeFunction.find(newFunc->funcName.substr(8));
		if (nativeFunc != NativeFunction::allNativeFunction.end())
			return this->_handleNativeFunction(nativeFunc->second, newArgVec);
		else
			throw std::runtime_error("Runtime Error: Function is not declared.");
	}
	if (newArgVec.size() > newFunc->parameters.size()) throw std::runtime_error("Runtime Error: Too many arguments.");
	std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> scopedVariables;
	for (const auto& varName : newFunc->varNameAccesible)
	{
		scopedVariables[varName] = this->__variable->variables.at(varName);
	}
	int index = 0;
	std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> newParam
		= newFunc->parameters;
	for (const auto &param : newFunc->parameters)
	{
		if (index == newArgVec.size())
			break;
		auto var = this->_checkVariableConstraint(newArgVec[newArgVec.size()-1-index], param.second.first);
		newParam[param.first] = std::make_pair(param.second.first, var);
		index++;
	}
	return this->compile(newFunc->blockWillRun, scopedVariables, newParam, this->__variable->functionMap);
}
