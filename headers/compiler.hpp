#pragma once
#include "higherObject.hpp"
#include "library/nativeObject.hpp"
#include "library/listObject.hpp"
#include "library/stringObject.hpp"
#include "library/mapObject.hpp"
#include "library/jdm_math.hpp"

#include "utils/comparator.hpp"

#include "library/classes/fileClassObject.hpp"
#include "library/classes/consoleClassObject.hpp"

class Compiler
{
private:
	std::shared_ptr<Block> __mainBlock;

	std::vector<std::string> allInclude;
	std::unordered_map<std::string, std::shared_ptr<BaseNativeClass>> nativeClassMap =
	{
		{ "File"   , std::make_shared<FileClassFunctions>() },
		{ "Console", std::make_shared<ConsoleClass>() }
	};
	struct VariableLink
	{
		std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> variables;
		std::unordered_map<std::string, std::shared_ptr<HigherObject::FunctionCall>> functionMap;
		std::shared_ptr<VariableLink> next;
		std::shared_ptr<VariableLink> prev;
	};
	std::shared_ptr<VariableLink> variable = std::make_shared<VariableLink>();
	bool breakLoop = false;
	int loopRunningCount = 0;

public:
	Compiler(const std::shared_ptr<Block> &mainBlock)
	 : __mainBlock(mainBlock)
	{
		this->compile(this->__mainBlock);
	}

	JDM_DLL const std::shared_ptr<HigherObject> compile(
		const std::shared_ptr<Block> &block,
		const std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_variables = {},
		const std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_AdditionalVariables = {},
		const std::unordered_map<std::string, std::shared_ptr<HigherObject::FunctionCall>> &_functionsAvail = {})
	{
		// Use for scoping
		if (this->variable == nullptr)
			this->variable = std::make_shared<VariableLink>();
		else
		{
			this->variable->next = std::make_shared<VariableLink>();
			this->variable->next->prev = this->variable;
			this->variable = this->variable->next;
		}
		this->variable->variables = _variables;
		for (const auto& element : _AdditionalVariables)
		{
			this->variable->variables[element.first] = element.second;
		}
		this->variable->functionMap = _functionsAvail;

		for (const auto &instruction : block->instruction)
		{
			// Reset the break and continue
			this->breakLoop    = false;
			if (instruction->getType() == DeclarationInstruction)
			{
				auto declare = std::dynamic_pointer_cast<Declaration>(instruction);
				auto dataT   = JDM::dataTypeMap.at(std::get<0>(declare->dataType->token));
				this->addVariable(
					declare->expression,
					dataT,
					declare->varName->returnStringValue(),
					declare->isConst, declare->isForce
				);
			}
			else if (instruction->getType() == AssignmentInstruction)
			{
				auto assign = std::dynamic_pointer_cast<Assignment>(instruction);
				auto var    = this->variable->variables.find(assign->varName->returnStringValue());

				if (var->second.second->isConstant)
				{
					throw std::runtime_error("Runtime Error: Variable is Constant.");
				}

				if ( var != this->variable->variables.end() )
				{
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
				{
					throw std::runtime_error("Runtime Error: Variable is not declared.");
				}
			}
			else if (instruction->getType() == LoggerInstruction)
			{
				auto logger = std::dynamic_pointer_cast<Logger>(instruction);
				for (const auto &expr : logger->expressions)
				{
					auto resultExpr = this->evaluateExpression(expr);
					if (resultExpr == nullptr)
					{
						throw std::runtime_error("Runtime Error: Invalid Expression.");
					}
					resultExpr->logValue();
					if (logger->addNewLine)
					{
						Log << '\n';
					}
				}
			}
			else if (instruction->getType() == CFunctionInstruction)
			{
				auto cfunction = std::dynamic_pointer_cast<CFunction>(instruction);
				if (cfunction->funcType == CUSFUNC_CLEAR)
				{
					system("cls");
				}
				else if (cfunction->funcType == CUSFUNC_SLEEP)
				{
					// Evaluate and then sleep
					auto timeML = this->evaluateExpression(cfunction->expression);
					timeML->castToInteger();
					std::this_thread::sleep_for(std::chrono::milliseconds(timeML->integerValue));
				}
				// else if (cfunction->funcType == CUSFUNC_GETTYPE)
				// {
				// 	auto expressionType = this->evaluateExpression(cfunction->expression);
				// 	return std::make_shared<HigherObject>(expressionType->getType());
				// }

				// TO DO WILL CREATE INCLDUE ON STRING
				// FOR PATH, IT MIGHT BECOME IMPORT BUT WHO KNOWS
				else if (cfunction->funcType == CUSFUNC_INCLUDE)
				{
					// Since we don't know the class name of variable
					// My code first treat it as a random variable
					auto varName   = std::dynamic_pointer_cast<VariableObjects>(cfunction->expression->firstValue)->returnStringValue();
					auto className = this->nativeClassMap.find(varName);
					if (className != this->nativeClassMap.end())
					{
						auto newClass = std::make_shared<HigherObject::ClassObject>();
						newClass->className = varName;

						// Create a HigherObject as a Class
						auto var = std::make_shared<HigherObject>(newClass);
						var->isForcedConstraint = true;
						var->isConstant         = true;

						// Set variable to global variables scope
						this->variable->variables[varName] = std::make_pair(DataTypeEnum::DATA_OBJECT, this->checkVariableConstraint(var, DataTypeEnum::DATA_OBJECT));
						this->allInclude.push_back(varName);
					}
					else
					{
						// If it is not valid include. Return error
						throw std::runtime_error("Runtime Error: Cannot find the thing you want to included.");
					}
				}

			}
			else if (instruction->getType() == IfStatementInstruction)
			{
				auto valueToReturn = this->processIfStatement(instruction);
				if (valueToReturn != nullptr)
				{
					this->setupVariablePrevious();
					return valueToReturn;
				}
			}
			else if (instruction->getType() == ForLoopStatementInstruction)
			{
				auto valueToReturn = this->processForStatement(instruction);
				if (valueToReturn != nullptr)
				{
					this->setupVariablePrevious();
					return valueToReturn;
				}
			}
			else if (instruction->getType() == WhileStatementInstruction)
			{
				auto valueToReturn = this->processWhileLoop(instruction);
				if (valueToReturn != nullptr)
				{
					this->setupVariablePrevious();
					return valueToReturn;
				}
			}
			else if (instruction->getType() == CreateFunctionInstruction)
			{
				this->processCreateFunction(instruction);
			}
			else if (instruction->getType() == CallInstruction)
			{
				this->processCallFunction(instruction);
			}
			else if (instruction->getType() == BreakInstruction)
			{
				if (this->loopRunningCount > 0)
				{
					this->breakLoop = true;
					return nullptr;
				}
			}
			else if (instruction->getType() == ContinueInstruction)
			{
				if (this->loopRunningCount > 0)
				{
					return nullptr;
				}
			}
			else if (instruction->getType() == ReturnInstruction)
			{
				if (block == this->__mainBlock)
				{
					throw std::runtime_error("Runtime Error: Cannot return outside Function.");
				}
				auto returnObj     = std::dynamic_pointer_cast<Return>(instruction);
				auto valueToReturn = this->evaluateExpression(returnObj->returnValue);
				this->setupVariablePrevious();
				return valueToReturn;
			}
		}
		this->setupVariablePrevious();
		return nullptr;
	}

	const void setupVariablePrevious()
	{
		if (this->variable->prev != nullptr)
		{
			this->variable = this->variable->prev;
			std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> tempMap;
			for (const auto& element : this->variable->variables)
			{
				if (this->variable->next->variables.count(element.first))
				{
					tempMap[element.first] = this->variable->next->variables.at(element.first);
				}
				else
				{
					tempMap[element.first] = element.second;
				}
			}
			this->variable->variables = tempMap;
			this->variable->next = nullptr;
		}
	}

	const std::vector<std::string> getAllVarName(
		const std::unordered_map<std::string,
		std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_variables = {})
	{
		std::vector<std::string> keys;
		for (const auto& pair : _variables)
		{
		   keys.push_back(pair.first);
		}
		return keys;
	}

	const void processCreateFunction(
		const std::shared_ptr<Instruction> &instruction)
	{
		auto createFunc = std::dynamic_pointer_cast<CreateFunction>(instruction);
		std::shared_ptr<HigherObject::FunctionCall> newFunction = std::make_shared<HigherObject::FunctionCall>();
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

	const std::shared_ptr<HigherObject> processCallFunction(
		const std::shared_ptr<Instruction> &instruction)
	{
		auto callFunc    = std::dynamic_pointer_cast<Call>(instruction);
		auto callObjRoot = callFunc->callObj;
		while (callObjRoot->prevObject != nullptr)
		{
			callObjRoot  = callObjRoot->prevObject;
		}
		return this->recursivelyCall(callObjRoot, callFunc->expression);
	}

	const std::shared_ptr<HigherObject> runFunction(
		const std::shared_ptr<HigherObject::FunctionCall> &newFunc,
		const std::vector<std::shared_ptr<HigherObject>> &arguments)
	{
		std::vector<std::shared_ptr<HigherObject>> newArgVec = { arguments.begin(), arguments.end() };
		newArgVec.insert(newArgVec.end(), newFunc->preArgs.begin(), newFunc->preArgs.end());

		if (newFunc->funcName.find("$native-") == 0)
		{
			auto nativeFunc = NativeFunction::allNativeFunction.find(newFunc->funcName.substr(8));
			if (nativeFunc != NativeFunction::allNativeFunction.end())
				return this->handleNativeFunction(nativeFunc->second, newArgVec);
			else
				throw std::runtime_error("Runtime Error: Function is not declared.");
		}

		if (newArgVec.size() > newFunc->parameters.size()) throw std::runtime_error("Runtime Error: Too many arguments.");
		std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> scopedVariables;
		for (const auto& varName : newFunc->varNameAccesible)
		{
			scopedVariables[varName] = this->variable->variables.at(varName);
		}

		int index = 0;
		std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> newParam
			= newFunc->parameters;

		for (const auto &param : newFunc->parameters)
		{
			if (index == newArgVec.size())
				break;

			auto var = this->checkVariableConstraint(newArgVec[newArgVec.size()-1-index], param.second.first);
			newParam[param.first] = std::make_pair(param.second.first, var);
			index++;
		}

		return this->compile(newFunc->blockWillRun, scopedVariables, newParam, this->variable->functionMap);
	}

private:
	const std::shared_ptr<HigherObject> handleNativeFunction(
		NativeFunction::NativeFunctionEnum nativeType,
		const std::vector<std::shared_ptr<HigherObject>> &objects)
	{
		if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_FILTER)
		{
			if (objects.size() != 2) throw std::runtime_error("Runtime Error: Expecting 2 arguments. Target ITERABLE and Function.");
			if (!objects[1]->isFunc) throw std::runtime_error("Runtime Error: Invalid Function arguments on 'filter'.");

			if (!objects[0]->isList && !objects[0]->isMap)
				return objects[0];
			else
			{
				auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();
				newList->listValue.erase(std::remove_if(newList->listValue.begin(), newList->listValue.end(),
					[&](const std::shared_ptr<HigherObject> &obj1)
					{
						std::shared_ptr<HigherObject> filterResult = this->runFunction(objects[1]->funcValue, { obj1 });
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

			if (!objects[0]->isList && !objects[0]->isMap)
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
			if (!objects[1]->isFunc)
				throw std::runtime_error("Runtime Error: Invalid Function arguments on 'reduce'.");

			if (!objects[0]->isList && !objects[0]->isMap)
				return objects[0];
			else
			{
				auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();
				if (newList->listValue.empty())
					throw std::runtime_error("Runtime Error: Using 'reduce' on empty ITERABLE.");

				auto result = newList->listValue[0];
				for (int i = 1; i < newList->listValue.size(); i++)
					result = this->runFunction(objects[1]->funcValue, { result, newList->listValue[i] });

				return result;
			}
		}
		else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_MAP)
		{
			if (objects.size() != 2) throw std::runtime_error("Runtime Error: Expecting 2 arguments. Target ITERABLE and Function.");
			if (!objects[1]->isFunc) throw std::runtime_error("Runtime Error: Invalid Function arguments on 'reduce'.");

			if (!objects[0]->isList && !objects[0]->isMap)
				return objects[0];
			else
			{
				auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();

				for (int i = 0; i < newList->listValue.size(); i++)
					newList->listValue[i] = this->runFunction(objects[1]->funcValue, { newList->listValue[i] });
				return newList;
			}
		}
		else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_SORT_IF)
		{
			if (objects.size() != 2) throw std::runtime_error("Runtime Error: Expecting 2 arguments. Target ITERABLE and Function.");
			if (!objects[1]->isFunc) throw std::runtime_error("Runtime Error: Invalid Function arguments on 'reduce'.");

			if (!objects[0]->isList && !objects[0]->isMap)
				return objects[0];
			else
			{
				auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();
				std::sort(newList->listValue.begin(), newList->listValue.end(),
					[&](const std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2)
					{
						std::shared_ptr<HigherObject> conditionResult = this->runFunction(objects[1]->funcValue, { obj1, obj2 });
						conditionResult->castToBoolean(); return conditionResult->booleanValue;
					});
				return newList;
			}
		}
		else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_PARTIAL)
		{
			if (objects.size() < 1 ) throw std::runtime_error("Runtime Error: Expecting atleast 1 argument. Target Function.");
			if (!objects[0]->isFunc) throw std::runtime_error("Runtime Error: Invalid Function arguments on 'partial'.");

			auto newFunction              = std::make_shared<HigherObject::FunctionCall>();
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

private:
	const std::shared_ptr<HigherObject> recursivelyCall(
		const std::shared_ptr<CallObjects> &callObj,
		const std::shared_ptr<Expression> &expressionAssign = nullptr)
	{
		auto tok = callObj->currObject->getToken();
		std::shared_ptr<HigherObject> returnValue;

		if (tok == JDM::TokenType::FUNCTIONS)
		{
			auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(callObj->currObject);
			std::shared_ptr<HigherObject::FunctionCall> newFunc;

			auto variables = this->variable->variables  .find(functionObj->returnStringValue());
			auto function  = this->variable->functionMap.find(functionObj->returnStringValue());

			if (function != this->variable->functionMap.end())
			{
				newFunc = function->second;
			}

			// Used to run a lambda
			else if (variables != this->variable->variables.end() && variables->second.second->isFunc)
			{
				newFunc = variables->second.second->funcValue;
			}

			else if (variables != this->variable->variables.end() && variables->second.second->isObject)
			{
				for (auto &e : this->allInclude)
				{
					if (e.compare(variables->second.second->objectValue->className) == 0)
					{
						if (!variables->second.second->objectValue->fromMainSource)
						{
							throw std::runtime_error("Runtime Error: This object is not callable.");
						}

						auto newObj = std::make_shared<HigherObject>();
						newObj->castToObject();
						newObj->objectValue->className = variables->second.second->objectValue->className;
						newObj->objectValue->members   = variables->second.second->objectValue->members;
						newObj->objectValue->methods   = variables->second.second->objectValue->methods;
						newObj->objectValue->fromMainSource = false;

						returnValue = this->nativeClassMap[e]->constructor(newObj, this->getVectorHigherObject(functionObj->arguments));
						return this->manageEndCall(callObj, returnValue, expressionAssign);
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
						if (functionObj->arguments.size() != 1)
							throw std::runtime_error("Runtime Error: Reference expect L value.");

						if (callObj->nextObject != nullptr)
							throw std::runtime_error("Runtime Error: Reference can't have recursive call.");

						returnValue = getVariableObject(functionObj->arguments[0]);
					}
					else
						returnValue = this->handleNativeFunction(nativeFunc->second, this->getVectorHigherObject(functionObj->arguments));
					return this->manageEndCall( callObj, returnValue, expressionAssign );
				}
				else throw std::runtime_error("Runtime Error: Function is not declared.");
			}
			if (newFunc == nullptr)
			{
				throw std::runtime_error("Runtime Error: Invalid Function.");
			}
			returnValue = this->runFunction(newFunc, this->getVectorHigherObject(functionObj->arguments));
		}
		else if (tok == JDM::TokenType::EXPRESSION)
		{
			// Get the expression object
			auto exprObj = std::dynamic_pointer_cast<ExpressionObjects>(callObj->currObject);

			auto variables = this->variable->variables.find(exprObj->returnStringValue());
			if (variables == this->variable->variables.end())
				throw std::runtime_error("Runtime Error: Variable is not declared.");

			auto varList = variables->second.second;
			if (varList->isList || varList->isMap || varList->isString)
				returnValue = this->manageCallBrackets(callObj, varList, expressionAssign);
			else
				throw std::runtime_error("Runtime Error: Variable is not a String, List or a Map.");
		}
		else if (tok == JDM::TokenType::VARIABLE)
		{
			auto var = std::dynamic_pointer_cast<VariableObjects>(callObj->currObject);
			auto variables = this->variable->variables.find(var->returnStringValue());

			if (variables == this->variable->variables.end())
				throw std::runtime_error("Runtime Error: Variable is not declared.");

			returnValue = variables->second.second;
		}
		else if (tok == JDM::TokenType::STRING)
		{
			returnValue = std::make_shared<HigherObject>(callObj->currObject->returnStringValue());
		}
		return this->manageEndCall(callObj, returnValue, expressionAssign);
	}


	// This is the return value on what you call. Example the return value of function is list.
	// Then you can still do operation because. you know, you can?.
	const std::shared_ptr<HigherObject> manageEndCall(
		const std::shared_ptr<CallObjects> &callObj,
		std::shared_ptr<HigherObject> &returnVal,
		const std::shared_ptr<Expression> &expressionAssign = nullptr)
	{
		// Setup new return
		std::shared_ptr<HigherObject> newReturn;
		auto newCallObj = callObj->nextObject;

		// If there is no call object, just return the value
		if (newCallObj == nullptr)
		{
			return returnVal;
		}

		// The chance of this happening is low
		// because If my lexer and parser do their job properly, it will work flawlessly.
		if (returnVal == nullptr)
		{
			throw std::runtime_error("Runtime Error: Return Value cannot be called or act like class.");
		}

		// Get the next object
		auto newCurrObj = newCallObj->currObject;
		auto tok        = newCurrObj->getToken();

		if (returnVal->isFunc)
		{
			if (tok != JDM::TokenType::FUNCTIONS)
			{
				// This will happen because, example a function has been return
				// the you do this
				/*
				jfunc test() => {
					jreturn () => { $logn => "TEST"; }; 
				}
				$logn => test().test();
				*/
				throw std::runtime_error("Runtime Error: Return Value is Function. Not a class.");
			}
			// Get the function object of the newCallObject
			auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(newCurrObj);
			if (functionObj->returnStringValue() != "(")
			{
				throw std::runtime_error("Runtime Error: Return Value is Function. Not a class.");
			}
			return this->runFunction(returnVal->funcValue, this->getVectorHigherObject(functionObj->arguments));
		}

		// This thing is still not properly coded or implemented.
		// I will do this bit later or next time
		// TO DO
		else if (returnVal->isObject)
		{
			if (tok == JDM::TokenType::FUNCTIONS)
			{
				auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(newCurrObj);
				auto classFunc   = this->nativeClassMap.find(returnVal->objectValue->className);
				if (classFunc != this->nativeClassMap.end())
				{
					// Will never be true, as recursively call, will handle it, but just in case.
					if (functionObj->returnStringValue() == "(")
					{
						newReturn = classFunc->second->constructor(returnVal, this->getVectorHigherObject(functionObj->arguments));
					}
					else
					{
						// This still not really cannot do anything.
						auto func = classFunc->second->mapFunctions.find(functionObj->returnStringValue());
						if (func == classFunc->second->mapFunctions.end())
						{
							throw std::runtime_error("Runtime Error: This method is not a member of class '" + returnVal->objectValue->className + "'.");
						}
						newReturn = classFunc->second->manageFunction(func->second, returnVal, this->getVectorHigherObject(functionObj->arguments));
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
		else if (returnVal->isString)
		{
			if (tok == JDM::TokenType::EXPRESSION)
			{
				// If the next token is like this
				/*
				jfunc test() => {
					jreturn "TEST"; 
				}
				$log => test()[0];
				*/
				newReturn = this->manageCallBrackets(newCallObj, returnVal, expressionAssign);
			}
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
				{
					// If the return value is string then you call it.
					/*
					jfunc test() => {
						jreturn "test";
					}
					$logn => test()();
					*/
					throw std::runtime_error("Runtime Error: A String cannot be called.");
				}

				auto func = StringHigherFunctions::stringFunctions.find(functionObj->returnStringValue());
				if (func == StringHigherFunctions::stringFunctions.end())
				{
					// If the return value is string then you call it and the function is not member or valid func.
					/*
					jfunc test() => {
						jreturn "test";
					}
					$logn => test().test();
					*/
					throw std::runtime_error("Runtime Error: This function is not a member of class 'jstring'.");
				}
				newReturn = StringHigherFunctions::manageFunction(
					func->second,
					returnVal,
					this->getVectorHigherObject(functionObj->arguments)
				);
			}
		}
		else if (returnVal->isList || returnVal->isMap)
		{
			if (tok == JDM::TokenType::EXPRESSION)
			{
				// This will manage when you want to know the index of something
				newReturn = this->manageCallBrackets(newCallObj, returnVal, expressionAssign);
			}
			else if (tok == JDM::TokenType::FUNCTIONS)
			{
				auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(newCurrObj);
				if (returnVal->isList)
				{
					if (functionObj->returnStringValue() == "(")
					{
						throw std::runtime_error("Runtime Error: A 'jlist' cannot be called.");
					}
					// jlist native function. Acts like a class.
					auto func = ListHigherFunctions::listFunctions.find(functionObj->name);						
					if (func == ListHigherFunctions::listFunctions.end())
					{
						// If the function is not a member of jlist
						/*
						jfunc test() => {
							jreturn [1, 2, 3, 4, 5]; 
						}
						$log => test().test();
						*/
						throw std::runtime_error("Runtime Error: This function is not a member of class 'jlist'.");
					}
					newReturn = ListHigherFunctions::manageFunction(
						func->second,
						returnVal,
						this->getVectorHigherObject(functionObj->arguments)
					);
				}
				else // Return Value is Map
				{
					if (functionObj->returnStringValue() == "(")
					{
						throw std::runtime_error("Runtime Error: A 'jmap' cannot be called.");
					}
					// jmap native function. Acts like a class.
					auto func = MapHigherFunctions::mapFunctions.find(functionObj->name);						
					if (func == MapHigherFunctions::mapFunctions.end())
					{
						// If the function is not a member of jmap
						/*
						jfunc test() => {
							jreturn { "test" => 10 }; 
						}
						$log => test().test();
						*/
						throw std::runtime_error("Runtime Error: This function is not a member of class 'jmap'.");
					}
					newReturn = MapHigherFunctions::manageFunction(
						func->second,
						returnVal,
						this->getVectorHigherObject(functionObj->arguments)
					);
				}
			}
		}
		else
		{
			// When the return value is not what you expect.
			// This soon will be remove, because after I add native for other data type
			// Example I add on integer, float or boolean
			throw std::runtime_error("Runtime Error: Return Value cannot be called or act like class.");
		}

		// Return the new Value calculated above
		return this->manageEndCall(newCallObj, newReturn);
	}

	const std::shared_ptr<HigherObject> manageCallBrackets(
		const std::shared_ptr<CallObjects > &next,
		const std::shared_ptr<HigherObject> &returnVal,
		const std::shared_ptr<Expression  > &expressionAssign = nullptr)
	{

		auto exprObj = std::dynamic_pointer_cast<ExpressionObjects>(next->currObject);
		auto expr    = this->evaluateExpression(exprObj->expression); // Evaluate the expression

		if (returnVal->isList)
		{
			expr->castToInteger();
			int index = expr->integerValue;
			if (index < 0)
			{
				index += returnVal->listValue.size();
			}
			if (index < 0 || index >= returnVal->listValue.size())
			{
				throw std::runtime_error("Runtime Error: Index out of bounds.");
			}

			if (next->nextObject == nullptr && expressionAssign != nullptr)
			{
				if (returnVal->isConstant)
				{
					throw std::runtime_error("Runtime Error: Variable is Constant.");
				}
				auto exprResult = this->evaluateExpression(expressionAssign);
				auto oldValue   = returnVal->listValue[index];
				if (next->operation == "+=")
				{
					exprResult = this->newOperatedObject(oldValue, "+", exprResult);
				}
				else if (next->operation == "-=")
				{
					exprResult = this->newOperatedObject(oldValue, "-", exprResult);
				}
				else if (next->operation == "*=")
				{
					exprResult = this->newOperatedObject(oldValue, "*", exprResult);
				}
				else if (next->operation == "/=")
				{
					exprResult = this->newOperatedObject(oldValue, "/", exprResult);
				}
				else if (next->operation == "%=")
				{
					exprResult = this->newOperatedObject(oldValue, "%", exprResult);
				}
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
			{
				index += returnVal->stringValue.size();
			}
			if (index < 0 || index >= returnVal->stringValue.size())
			{
				throw std::runtime_error("Runtime Error: String index out of bounds.");
			}

			if (next->nextObject == nullptr && expressionAssign != nullptr)
			{
				if (returnVal->isConstant)
				{
					throw std::runtime_error("Runtime Error: Variable is Constant.");
				}
				auto exprResult = this->evaluateExpression(expressionAssign);
				exprResult->castToString();

				if (exprResult->stringValue.size() != 1)
				{
					// No operation is valid in this.
					throw std::runtime_error("Runtime Error: Expected single character assignment. Use insert instead.");
				}
				returnVal->stringValue[index] = exprResult->stringValue[0];
			}
			return std::make_shared<HigherObject>(std::string(1, returnVal->stringValue[index]));
		}
		else if (returnVal->isMap)
		{
			if (next->nextObject == nullptr && expressionAssign != nullptr)
			{
				if (returnVal->isConstant)
				{
					throw std::runtime_error("Runtime Error: Variable is Constant.");
				}
				auto key = returnVal->isInMap(expr, true);
				auto exprResult = this->evaluateExpression(expressionAssign);
				if (key != nullptr)
				{
					auto oldValue = returnVal->mapValue.at(key);
					if (next->operation == "+=")
					{
						exprResult = this->newOperatedObject(oldValue, "+", exprResult);
					}
					else if (next->operation == "-=")
					{
						exprResult = this->newOperatedObject(oldValue, "-", exprResult);
					}
					else if (next->operation == "*=")
					{
						exprResult = this->newOperatedObject(oldValue, "*", exprResult);
					}
					else if (next->operation == "/=")
					{
						exprResult = this->newOperatedObject(oldValue, "/", exprResult);
					}
					else if (next->operation == "%=")
					{
						exprResult = this->newOperatedObject(oldValue, "%", exprResult);
					}
				}
				else if (next->operation != "=")
				{
					throw std::runtime_error("Runtime Error: Key in map doesn't exist. Cannot do operation.");
				}
				returnVal->mapValue[((key != nullptr) ? key : expr)] = exprResult;
				return exprResult;
			}
			else
			{
				auto returnValue = returnVal->isInMap(expr);
				if (returnValue == nullptr)
				{
					throw std::runtime_error("Runtime Error: Map key is not found");
				}
				return returnValue;
			}
		}
		return nullptr;
	}

	// Useful for function arguments
	const std::vector<std::shared_ptr<HigherObject>> getVectorHigherObject(
		const std::vector<std::shared_ptr<Expression>> &expr)
	{
		// Evaluate all expression in vector
		std::vector<std::shared_ptr<HigherObject>> result;
		for (const auto &ex : expr)
		{
			result.push_back(this->evaluateExpression(ex));
		}
		return result;
	}

	const void addVariable(
		const std::shared_ptr<Expression> &expr,
		DataTypeEnum dataT,
		const std::string &varName,
		bool isConst = false,
		bool isForce = false,
		const std::string &operation = "=")
	{
		auto var = this->evaluateExpression(expr);
		if (operation == "+=")
		{
			var = this->newOperatedObject(this->variable->variables[varName].second, "+", var);
		}
		else if (operation == "-=")
		{
			var = this->newOperatedObject(this->variable->variables[varName].second, "-", var);
		}
		else if (operation == "*=")
		{
			var = this->newOperatedObject(this->variable->variables[varName].second, "*", var);
		}
		else if (operation == "/=")
		{
			var = this->newOperatedObject(this->variable->variables[varName].second, "/", var);
		}
		else if (operation == "%=")
		{
			var = this->newOperatedObject(this->variable->variables[varName].second, "%", var);
		}
		var->isConstant         = isConst;
		var->isForcedConstraint = isForce;
		var = this->checkVariableConstraint(var, dataT);

		// Get the old Variable and just set it's new value to this
		auto oldVar  = this->variable->variables.find(varName);
		if (oldVar != this->variable->variables.end())
		{
			// Use the setHigherObject to set the value of variable to var
			oldVar->second.second->setHigherObject(var);
			this->variable->variables[varName] = std::make_pair(dataT, oldVar->second.second);
			return;
		}
		this->variable->variables[varName] = std::make_pair(dataT, var);
	}

	const std::pair<DataTypeEnum, std::shared_ptr<HigherObject>> returnVariable(
		const std::shared_ptr<Expression> &expr, DataTypeEnum dataT) {
		auto var = this->castVariable(expr, dataT);
		return std::make_pair(dataT, var);
	}

	const std::shared_ptr<HigherObject> castVariable(
		const std::shared_ptr<Expression> &expr,
		DataTypeEnum dataT,
		bool checkConstraint = true)
	{
		auto var = this->evaluateExpression(expr);
		return this->checkVariableConstraint(var, dataT, checkConstraint);
	}

	const std::shared_ptr<HigherObject> checkVariableConstraint(
		const std::shared_ptr<HigherObject> &var,
		DataTypeEnum dataT,
		bool checkConstraint = true)
	{
		if (var->isForcedConstraint && dataT != DataTypeEnum::DATA_ANY && checkConstraint)
		{
			DataTypeEnum varType = var->getDatatypeEnum();
			if (dataT != varType)
			{
				if (dataT == DataTypeEnum::DATA_STRING)
				{
					throw std::runtime_error("Runtime Error: Due to force constraint. Value must be String.");
				}
				else if (dataT == DataTypeEnum::DATA_INTEGER
					  || dataT == DataTypeEnum::DATA_DOUBLE
					  || dataT == DataTypeEnum::DATA_BOOLEAN)
				{
					throw std::runtime_error("Runtime Error: Due to force constraint. Value must be Number.");
				}
				else if (dataT == DataTypeEnum::DATA_LAMBDA)
				{
					throw std::runtime_error("Runtime Error: Due to force constraint. Value must be Function or Lambda.");
				}
				else if (dataT == DataTypeEnum::DATA_OBJECT)
				{
					throw std::runtime_error("Runtime Error: Due to force constraint. Value must be Object.");
				}
				else if (dataT == DataTypeEnum::DATA_LIST)
				{
					throw std::runtime_error("Runtime Error: Due to force constraint. Value must be List.");
				}
				else if (dataT == DataTypeEnum::DATA_MAP)
				{
					throw std::runtime_error("Runtime Error: Due to force constraint. Value must be Map.");
				}
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

	const std::shared_ptr<HigherObject> processIfStatement(
		const std::shared_ptr<Instruction> &instruction)
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

	const std::shared_ptr<HigherObject> processForStatement(
		const std::shared_ptr<Instruction> &instruction)
	{
		auto forState = std::dynamic_pointer_cast<ForLoopStatement>(instruction);

		// Setup the for loop
		// The start, step, and stop
		std::shared_ptr<HigherObject> start;
		std::shared_ptr<HigherObject> step ;
		std::shared_ptr<HigherObject> stop    = this->evaluateExpression(forState->stop);
		if (forState->start != nullptr) start = this->evaluateExpression(forState->start);
		if (forState->step  != nullptr) step  = this->evaluateExpression(forState->step);
		if (start == nullptr)           start = std::make_shared<HigherObject>(static_cast<int64_t>(0));
		if (step  == nullptr)           step  = std::make_shared<HigherObject>(static_cast<int64_t>(1));

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

	const std::shared_ptr<HigherObject> processWhileLoop(
		const std::shared_ptr<Instruction> &instruction)
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

	// Function to create a new operated object based on the provided operation and operands
	JDM_DLL std::shared_ptr<HigherObject> newOperatedObject(
		std::shared_ptr<HigherObject> &firstVal,
		const std::string &operation = "",
		const std::shared_ptr<HigherObject> &secondVal = nullptr);
	JDM_DLL void handleNewOperatedString(
		std::shared_ptr<HigherObject> &firstVal,
		const std::string &operation = "",
		const std::shared_ptr<HigherObject> &secondVal = nullptr);
	JDM_DLL void handleNewOperatedList(
		std::shared_ptr<HigherObject> &firstVal,
		const std::string &operation = "",
		const std::shared_ptr<HigherObject> &secondVal = nullptr);
	JDM_DLL void handleNewOperatedMap(
		std::shared_ptr<HigherObject> &firstVal,
		const std::string &operation = "",
		const std::shared_ptr<HigherObject> &secondVal = nullptr);
	JDM_DLL void handleNewOperatedFunc(
		std::shared_ptr<HigherObject> &firstVal,
		const std::string &operation = "",
		const std::shared_ptr<HigherObject> &secondVal = nullptr);
	JDM_DLL void handleNewOperatedNumber(
		std::shared_ptr<HigherObject> &firstVal,
		const std::string &operation = "",
		const std::shared_ptr<HigherObject> &secondVal = nullptr);
	JDM_DLL void handleNewOperatedObject(
		std::shared_ptr<HigherObject> &firstVal,
		const std::string &operation = "",
		const std::shared_ptr<HigherObject> &secondVal = nullptr);

	// Used to get the HigherObject
	// This will return value of variable or expression
	JDM_DLL std::shared_ptr<HigherObject> getHigherObject(
		const std::shared_ptr<VarObjects> &Value,
		const std::shared_ptr<Expression> &Expression)
	{
		if ( !Value )
		{
			// If Value is nullptr then just return the value of expression
			return this->evaluateExpression(Expression);
		}

		// Get the value from Variable
		JDM::TokenType type = Value->getToken();

		if (type == JDM::TokenType::VARIABLE)
		{
			// Get the variable name of the current Variable
			auto varName = std::dynamic_pointer_cast<VariableObjects>(Value)->returnStringValue();

			// Check if it is a variable and get it's value from pair
			auto var  = this->variable->variables.find(varName);
			if (var != this->variable->variables.end())
			{
				return std::make_shared<HigherObject>(var->second.second);
			}

			// Check if it is a function
			auto func = this->variable->functionMap.find(varName);
			if (func != this->variable->functionMap.end())
			{
				return std::make_shared<HigherObject>(func->second);
			}

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
		{
			return std::make_shared<HigherObject>(std::dynamic_pointer_cast<StringObjects>(Value)->returnValue());
		}
		if (type == JDM::TokenType::INTEGER)
		{
			return std::make_shared<HigherObject>(std::dynamic_pointer_cast<IntegerObjects>(Value)->returnValue());
		}
		if (type == JDM::TokenType::DOUBLE)
		{
			return std::make_shared<HigherObject>(std::dynamic_pointer_cast<DoubleObjects>(Value)->returnValue());
		}
		if (type == JDM::TokenType::LIST)
		{
			std::vector<std::shared_ptr<HigherObject>> vecList;

			// Evaluate all the list in this list
			auto exprList = std::dynamic_pointer_cast<ListObject>(Value)->returnValue();
			for (const auto &expr : exprList)
			{
				// [1+2, 3+4] -> [3, 7]
				vecList.push_back( this->evaluateExpression(expr) );
			}
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
				// Used to create key and value using evaluateExpression
				mapList[this->evaluateExpression(expr->key)] = this->evaluateExpression(expr->value);
			}
			return std::make_shared<HigherObject>(mapList);
		}

		// If the expression is Call Object, or function
		if (type == JDM::TokenType::CALL_OBJ)
		{
			// Get the root of CallObjects and get the result using recursivelyCall
			auto callObjRoot = std::dynamic_pointer_cast<CallObjects>(Value);
			while (callObjRoot->prevObject != nullptr)
			{
				callObjRoot = callObjRoot->prevObject;
			}
			auto result = this->recursivelyCall(callObjRoot);
			if (result == nullptr)
			{
				return std::make_shared<HigherObject>(static_cast<int64_t>(0));
			}
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
				{
					newFunction->parameters[var->varName->returnStringValue()] = this->returnVariable(var->varValue, DataTypeEnum::DATA_ANY);
				}
			}
			newFunction->blockWillRun      = std::dynamic_pointer_cast<Block>(lambdaObj->blockWillRun);
			newFunction->varNameAccesible  = this->getAllVarName(this->variable->variables);
			if (lambdaObj->willCall)
			{
				return this->runFunction(newFunction, this->getVectorHigherObject(lambdaObj->arguments));
			}
			else
			{
				return std::make_shared<HigherObject>(newFunction);
			}
		}
		if (type == JDM::TokenType::CAST)
		{
			auto castObj = std::dynamic_pointer_cast<CastObjects>(Value);
			return this->castVariable(castObj->expression, castObj->datTypeToTurn, false);
		}
		return nullptr;
	}

	JDM_DLL std::shared_ptr<HigherObject> &getVariableObject(const std::shared_ptr<Expression> &expr)
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
		{
			return var->second.second;
		}
		throw std::runtime_error("Runtime Error: Variable is not declared.");
	}

	JDM_DLL std::shared_ptr<HigherObject> evaluateExpression(
		const std::shared_ptr<Expression> &expr)
	{
		if (!expr)
		{
			return std::make_shared<HigherObject>(static_cast<int64_t>(0));
		}
		auto firstVal = this->getHigherObject(expr->firstValue, expr->firstExpression);
		if (firstVal == nullptr)
		{
			throw std::runtime_error("Runtime Error: Invalid Expression. Return null.");
		}

		if (expr->opWillUse)
		{
			if (expr->secondValue || expr->secondExpression)
			{
				auto second = this->getHigherObject  (expr->secondValue, expr->secondExpression);
				firstVal    = this->newOperatedObject(firstVal, std::get<0>(expr->opWillUse->token), second);
			}
			else
			{
				firstVal = this->newOperatedObject(firstVal, std::get<0>(expr->opWillUse->token), nullptr);
			}
		}
		return firstVal;
	}
};
