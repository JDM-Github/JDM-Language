#include "Compiler.hpp"

JDM_DLL
const void Compiler::doDeclarationInstruction(const std::shared_ptr<Instruction> &instruction)
{
	auto declare = std::dynamic_pointer_cast<Declaration>(instruction);
	auto dataT   = JDM::dataTypeMap.at(std::get<0>(declare->dataType->token));

	auto var    = this->variable->variables.find(declare->varName->returnStringValue());
	if ( var != this->variable->variables.end() )
		throw std::runtime_error("Runtime Error: Variable '" + declare->varName->returnStringValue()
			+ "' is already declared. Redeclaration is not allowed.");

	this->addVariable(
		declare->expression,
		dataT,
		declare->varName->returnStringValue(),
		declare->isConst, declare->isForce
	);
}

JDM_DLL
const void Compiler::doAssignmentInstruction(const std::shared_ptr<Instruction> &instruction)
{
	auto assign = std::dynamic_pointer_cast<Assignment>(instruction);
	auto var    = this->variable->variables.find(assign->varName->returnStringValue());
	if ( var != this->variable->variables.end() )
	{
		if (var->second.second->isConstant)
			throw std::runtime_error("Runtime Error: Variable is Constant.");

		this->addVariable(
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
const void Compiler::doLoggerInstruction(const std::shared_ptr<Instruction> &instruction)
{
	auto logger = std::dynamic_pointer_cast<Logger>(instruction);
	for (const auto &expr : logger->expressions)
	{
		auto resultExpr = this->evaluateExpression(expr);
		if (resultExpr == nullptr)
			throw std::runtime_error("Runtime Error: Invalid Expression.");

		resultExpr->logValue();
		if (logger->addNewLine)
			Log << '\n';
	}
}

JDM_DLL
const void Compiler::doCFunctionInstruction(const std::shared_ptr<Instruction> &instruction)
{
	auto cfunction = std::dynamic_pointer_cast<CFunction>(instruction);

	if (cfunction->funcType == CUSFUNC_CLEAR)
		system("cls");

	else if (cfunction->funcType == CUSFUNC_SLEEP)
	{
		// Evaluate and then sleep
		auto timeML = this->evaluateExpression(cfunction->expression);
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
		auto className = this->nativeClassMap.find(varName);
		if (className != this->nativeClassMap.end())
		{
			auto newClass = std::make_shared<ClassObject>();
			newClass->className = varName;

			// Create a HigherObject as a Class
			auto var = std::make_shared<HigherObject>(newClass);
			var->isForcedConstraint = true;
			var->isConstant         = true;

			// Set variable to global variables scope
			this->variable->variables[varName] = std::make_pair(DataTypeEnum::DATA_OBJECT,
				this->checkVariableConstraint(var, DataTypeEnum::DATA_OBJECT));
			this->allInclude.push_back(varName);
		}
		else
			// If it is not valid include. Return error
			throw std::runtime_error("Runtime Error: Cannot find the thing you want to included.");
	}
}

JDM_DLL
const void Compiler::processCreateFunction(const std::shared_ptr<Instruction> &instruction)
{
	auto createFunc = std::dynamic_pointer_cast<CreateFunction>(instruction);
	std::shared_ptr<FunctionCall> newFunction = std::make_shared<FunctionCall>();
	for (const auto &var : createFunc->parameters)
	{
		if (var->dataType != nullptr)
		{
			auto dataT = JDM::dataTypeMap.at(std::get<0>(var->dataType->token));
			newFunction->parameters[var->varName->returnStringValue()] = this->returnVariable(var->varValue, dataT);
		}
		else
			newFunction->parameters[var->varName->returnStringValue()] = this->returnVariable(var->varValue, DataTypeEnum::DATA_ANY);
	}
	auto functionName                         = createFunc->functionName->returnStringValue();
	newFunction->funcName                     = functionName;
	newFunction->blockWillRun                 = createFunc->blockWillRun;
	newFunction->varNameAccesible             = this->getAllVarName(this->variable->variables);
	this->variable->functionMap[functionName] = newFunction;
}

JDM_DLL
const void Compiler::processCallFunction(const std::shared_ptr<Instruction> &instruction)
{
	auto callFunc    = std::dynamic_pointer_cast<Call>(instruction);
	auto callObjRoot = callFunc->callObj;
	while (callObjRoot->prevObject != nullptr)
		callObjRoot  = callObjRoot->prevObject;

	this->isAssigning = true;
	this->recursivelyCall(callObjRoot, callFunc->expression);
}

JDM_DLL
const std::shared_ptr<HigherObject> Compiler::processIfStatement(const std::shared_ptr<Instruction> &instruction)
{
	if (instruction == nullptr)
		return nullptr;

	auto ifState = std::dynamic_pointer_cast<IfStatement>(instruction);
	if (ifState->condition != nullptr)
	{
		auto condition = this->evaluateExpression(ifState->condition);
		condition->castToBoolean();

		if (!condition->booleanValue)
			return this->processIfStatement(ifState->elseIf);
	}
	return this->compile(ifState->blockWillRun, this->variable->variables, {}, this->variable->functionMap);
}

JDM_DLL
const std::shared_ptr<HigherObject> Compiler::processForStatement(const std::shared_ptr<Instruction> &instruction)
{
	auto forState = std::dynamic_pointer_cast<ForLoopStatement>(instruction);
	// Setup the for loop
	// The start, step, and stop
	std::shared_ptr<HigherObject> start;
	std::shared_ptr<HigherObject> step ;
	std::shared_ptr<HigherObject> stop = this->evaluateExpression(forState->stop);

	if (forState->start != nullptr)
		start = this->evaluateExpression(forState->start);
	if (forState->step  != nullptr)
		step  = this->evaluateExpression(forState->step);
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
	this->loopRunningCount++;
	while (start->integerValue != stop->integerValue)
	{
		// Remember compile return a value, this is useful because sometimes you want to continue in loop or break
		auto returnValue = this->compile(
			forState->blockWillRun,
			this->variable->variables,
			additionalVariables,
			this->variable->functionMap
		);
		if (this->breakLoop)
		{
			this->loopRunningCount--;
			this->breakLoop = false;
			return nullptr;
		}
		if (returnValue != nullptr)
		{
			this->loopRunningCount--;
			return returnValue;
		}
		start->integerValue += step->integerValue;
	}
	this->loopRunningCount--;
	return nullptr;
}

JDM_DLL
const std::shared_ptr<HigherObject> Compiler::processWhileLoop(const std::shared_ptr<Instruction> &instruction)
{
	auto whileState = std::dynamic_pointer_cast<WhileStatement>(instruction);
	auto expr       = this->evaluateExpression(whileState->condition);
	expr->castToBoolean(); bool check = expr->booleanValue;
	this->loopRunningCount++;
	while (check)
	{
		auto returnValue = this->compile( whileState->blockWillRun,
			this->variable->variables, {}, this->variable->functionMap);
		if (this->breakLoop)
		{
			this->breakLoop = false;
			this->loopRunningCount--;
			return nullptr;
		}
		if (returnValue != nullptr)
		{
			this->loopRunningCount--;
			return returnValue;
		}
		auto expr = this->evaluateExpression(whileState->condition);
		expr->castToBoolean();
		check = expr->booleanValue;
	}
	this->loopRunningCount--;
	return nullptr;
}


JDM_DLL
const std::shared_ptr<HigherObject> Compiler::processForEachStatement(const std::shared_ptr<Instruction> &instruction)
{
	auto foreachState = std::dynamic_pointer_cast<ForEachStatement>(instruction);
	std::shared_ptr<HigherObject> realValue = nullptr;
	std::shared_ptr<HigherObject> key       = nullptr;
	std::shared_ptr<HigherObject> value     = nullptr;

	bool isReverse = foreachState->isReverse;
	bool isMap     = foreachState->varVal != nullptr;

	if (foreachState->varToRun != nullptr)
	{
		auto variables = this->variable->variables.find(foreachState->varToRun->returnStringValue());
		if (variables == this->variable->variables.end())
			throw std::runtime_error("Runtime Error: Variable is not declared.");

		realValue = variables->second.second;
		if (realValue->getCurrActive() != ACTIVE_LIST && realValue->getCurrActive() != ACTIVE_MAP)
			throw std::runtime_error("Runtime Error: Variable is not a 'jlist' or 'jmap'.");
	}
	else if (foreachState->expression)
	{
		realValue = this->evaluateExpression(foreachState->expression);
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

	this->loopRunningCount++;
	while (true)
	{
		auto returnValue = this->compile(
			foreachState->blockWillRun,
			this->variable->variables,
			additionalVariables,
			this->variable->functionMap
		);
		if (this->breakLoop)
		{
			this->loopRunningCount--;
			this->breakLoop = false;
			return nullptr;
		}
		if (returnValue != nullptr)
		{
			this->loopRunningCount--;
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
	this->loopRunningCount--;
	return nullptr;
}
