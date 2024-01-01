#pragma once
#include "higherObject.hpp"
#include "library/nativeObject.hpp"
#include "library/listObject.hpp"
#include "library/stringObject.hpp"
#include "library/mapObject.hpp"
#include "library/jdm_math.hpp"

#include "library/classes/fileClassObject.hpp"
// #include "library/console.hpp"

class Compiler {
private:
	std::shared_ptr<Block> __mainBlock;

	std::vector<std::string> allInclude;
	std::unordered_map<std::string, std::shared_ptr<BaseNativeClass>> nativeClassMap = {
		{ "File", std::make_shared<FileClassFunctions>() }
	};
	struct VariableLink {
		std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> variables;
		std::unordered_map<std::string, std::shared_ptr<HigherObject::FunctionCall>> functionMap;
		std::shared_ptr<VariableLink> next;
		std::shared_ptr<VariableLink> prev;
	};
	std::shared_ptr<VariableLink> variable = std::make_shared<VariableLink>();

public:
	Compiler(const std::shared_ptr<Block> &mainBlock)
	 : __mainBlock(mainBlock) {
		this->compile(this->__mainBlock);
	}

	JDM_DLL const std::shared_ptr<HigherObject> compile(
		const std::shared_ptr<Block> &block,
		const std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_variables = {},
		const std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_AdditionalVariables = {},
		const std::unordered_map<std::string, std::shared_ptr<HigherObject::FunctionCall>> &_functionsAvail = {}) {

		// Use for scoping
		if (this->variable == nullptr)
			this->variable = std::make_shared<VariableLink>();
		else {
			this->variable->next = std::make_shared<VariableLink>();
			this->variable->next->prev = this->variable;
			this->variable = this->variable->next;
		}
		this->variable->variables = _variables;
		for (const auto& element : _AdditionalVariables) this->variable->variables[element.first] = element.second;
		this->variable->functionMap = _functionsAvail;

		for (const auto &instruction : block->instruction) {
			if        (instruction->getType() == DeclarationInstruction) {
				auto declare = std::dynamic_pointer_cast<Declaration>(instruction);
				auto dataT   = JDM::dataTypeMap.at (std::get<0>(declare->dataType->token));
				this->addVariable(declare->expression, dataT, declare->varName->returnStringValue(),
					declare->isConst, declare->isForce);

			} else if (instruction->getType() == AssignmentInstruction) {
				auto assign = std::dynamic_pointer_cast<Assignment>(instruction);
				auto var  = this->variable->variables.find(assign->varName->returnStringValue());
				if (var->second.second->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");

				if ( var != this->variable->variables.end() )
					this->addVariable(assign->expression, var->second.first, assign->varName->returnStringValue(),
						var->second.second->isConstant, var->second.second->isForcedConstraint, std::get<0>(assign->operation->token));

				else throw std::runtime_error("Runtime Error: Variable is not declared.");

			} else if (instruction->getType() == LoggerInstruction) {
				auto logger = std::dynamic_pointer_cast<Logger>(instruction);
				for (const auto &expr : logger->expressions) {
					auto resultExpr = this->evaluateExpression(expr);
					if (resultExpr == nullptr) throw std::runtime_error("Runtime Error: Invalid Expression.");
					resultExpr->logValue();
					if (logger->addNewLine) Log << '\n';
				}

			} else if (instruction->getType() == CFunctionInstruction) {
				auto cfunction = std::dynamic_pointer_cast<CFunction>(instruction);
					 if (cfunction->funcType == CUSFUNC_CLEAR) system("cls");
				else if (cfunction->funcType == CUSFUNC_SLEEP) {
					auto timeML = this->evaluateExpression(cfunction->expression);
					timeML->castToInteger();
					std::this_thread::sleep_for(std::chrono::milliseconds(timeML->integerValue));
				}
				else if (cfunction->funcType == CUSFUNC_INCLUDE) {
					auto varName = std::dynamic_pointer_cast<VariableObjects>(cfunction->expression->firstValue)->returnStringValue();
					auto className = this->nativeClassMap.find(varName);
					if (className != this->nativeClassMap.end()) {

						auto newClass = std::make_shared<HigherObject::ClassObject>();
						newClass->className = varName;

						auto var = std::make_shared<HigherObject>(newClass);
						var->isForcedConstraint = true;
						var->isConstant = true;
						this->variable->variables[varName] = std::make_pair(DataTypeEnum::DATA_OBJECT, this->checkVariableConstraint(var, DataTypeEnum::DATA_OBJECT));
						this->allInclude.push_back(varName);

					} else throw std::runtime_error("Runtime Error: Cannot find the thing you want to included.");
				}

			} else if (instruction->getType() == IfStatementInstruction) {
				auto valueToReturn = this->processIfStatement(instruction);
				if (valueToReturn != nullptr) {
					this->setupVariablePrevious();
					return valueToReturn;
				}
			} else if (instruction->getType() == ForLoopStatementInstruction) {
				auto valueToReturn = this->processForStatement(instruction);
				if (valueToReturn != nullptr) {
					this->setupVariablePrevious();
					return valueToReturn;
				}
			} else if (instruction->getType() == WhileStatementInstruction) {
				auto valueToReturn = this->processWhileLoop(instruction);
				if (valueToReturn != nullptr) {
					this->setupVariablePrevious();
					return valueToReturn;
				}
			}
			else if (instruction->getType() == CreateFunctionInstruction) this->processCreateFunction(instruction);
			else if (instruction->getType() == CallInstruction          ) this->processCallFunction(instruction);
			else if (instruction->getType() == ReturnInstruction) {
				if (block == this->__mainBlock) throw std::runtime_error("Runtime Error: Cannot return outside Function.");
				auto returnObj = std::dynamic_pointer_cast<Return>(instruction);
				auto valueToReturn = this->evaluateExpression(returnObj->returnValue);
				this->setupVariablePrevious();
				return valueToReturn;
			}
		}
		this->setupVariablePrevious();
		return nullptr;
	}

	const void setupVariablePrevious() {
		if (this->variable->prev != nullptr) {
			this->variable = this->variable->prev;
			std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> tempMap;
			for (const auto& element : this->variable->variables) {
				if (this->variable->next->variables.count(element.first)) {
					tempMap[element.first] = this->variable->next->variables.at(element.first);
				} else tempMap[element.first] = element.second;
			}
			this->variable->variables = tempMap;
			this->variable->next = nullptr;
		}
	}

	const std::vector<std::string> getAllVarName(
		const std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_variables = {}
	) {
		std::vector<std::string> keys;
		for (const auto& pair : _variables)
		   keys.push_back(pair.first);

		return keys;
	}

	const void processCreateFunction(
		const std::shared_ptr<Instruction> &instruction)
	{
		auto createFunc = std::dynamic_pointer_cast<CreateFunction>(instruction);
		std::shared_ptr<HigherObject::FunctionCall> newFunction = std::make_shared<HigherObject::FunctionCall>();
		for (const auto &var : createFunc->parameters) {
			if (var->dataType != nullptr) {
				auto dataT   = JDM::dataTypeMap.at(std::get<0>(var->dataType->token));
				newFunction->parameters[var->varName->returnStringValue()] = this->returnVariable(var->varValue, dataT);
			} else {
				newFunction->parameters[var->varName->returnStringValue()] = this->returnVariable(var->varValue, DataTypeEnum::DATA_ANY);
			}
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
			callObjRoot  = callObjRoot->prevObject;

		return this->recursivelyCall(callObjRoot, callFunc->expression);
	}

	const std::shared_ptr<HigherObject> runFunction(
		const std::shared_ptr<HigherObject::FunctionCall> &newFunc,
		const std::vector<std::shared_ptr<HigherObject>> &arguments) {

		std::vector<std::shared_ptr<HigherObject>> newArgVec = { arguments.begin(), arguments.end() };
		newArgVec.insert(newArgVec.end(), newFunc->preArgs.begin(), newFunc->preArgs.end());

		if (newFunc->funcName.find("$native-") == 0) {
			auto nativeFunc = NativeFunction::allNativeFunction.find(newFunc->funcName.substr(8));
			if (nativeFunc != NativeFunction::allNativeFunction.end()) {
				return this->handleNativeFunction(nativeFunc->second, newArgVec);
			} else throw std::runtime_error("Runtime Error: Function is not declared.");
		}

		if (newArgVec.size() > newFunc->parameters.size()) throw std::runtime_error("Runtime Error: Too many arguments.");
		std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> scopedVariables;
		for (const auto& varName : newFunc->varNameAccesible)
			scopedVariables[varName] = this->variable->variables.at(varName);

		int index = 0;
		std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> newParam
			= newFunc->parameters;

		for (const auto &param : newFunc->parameters) {
			if (index == newArgVec.size()) break;
			auto var = this->checkVariableConstraint(newArgVec[newArgVec.size()-1-index], param.second.first);
			newParam[param.first] = std::make_pair(param.second.first, var);
			index++;
		}

		return this->compile(newFunc->blockWillRun, scopedVariables, newParam, this->variable->functionMap);
	}

private:
	const std::shared_ptr<HigherObject> handleNativeFunction(
		NativeFunction::NativeFunctionEnum nativeType,
		const std::vector<std::shared_ptr<HigherObject>> &objects) {

		if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_FILTER) {
			if (objects.size() != 2) throw std::runtime_error("Runtime Error: Expecting 2 arguments. Target ITERABLE and Function.");
			if (!objects[1]->isFunc) throw std::runtime_error("Runtime Error: Invalid Function arguments on 'filter'.");

			if (!objects[0]->isList && !objects[0]->isMap) return objects[0];
			else {
				auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();
				newList->listValue.erase(std::remove_if(newList->listValue.begin(), newList->listValue.end(),
					[&](const std::shared_ptr<HigherObject> &obj1) {
						std::shared_ptr<HigherObject> filterResult = this->runFunction(objects[1]->funcValue, { obj1 });
						filterResult->castToBoolean(); return !filterResult->booleanValue;
					}),
				newList->listValue.end());
				return newList;
			}
		} else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_UNIQUE) {
			if (objects.size() != 1) throw std::runtime_error("Runtime Error: Expecting 1 argument. Target ITERABLE");

			if (!objects[0]->isList && !objects[0]->isMap) return objects[0];
			else {
				auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();

				auto resultList = std::make_shared<HigherObject>(static_cast<int64_t>(0)); resultList->castToList();
				resultList->listValue.clear();

				for (int i = 0; i < newList->listValue.size(); i++)
					if (!resultList->isInList(newList->listValue[i]))
						resultList->listValue.push_back(std::make_shared<HigherObject>(newList->listValue[i]));
				return resultList;
			}
		} else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_REDUCE) {
			if (objects.size() != 2) throw std::runtime_error("Runtime Error: Expecting 2 arguments. Target ITERABLE and Function.");
			if (!objects[1]->isFunc) throw std::runtime_error("Runtime Error: Invalid Function arguments on 'reduce'.");

			if (!objects[0]->isList && !objects[0]->isMap) return objects[0];
			else {
				auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();
				if (newList->listValue.empty()) throw std::runtime_error("Runtime Error: Using 'reduce' on empty ITERABLE.");
				auto result = newList->listValue[0];
				for (int i = 1; i < newList->listValue.size(); i++)
					result = this->runFunction(objects[1]->funcValue, { result, newList->listValue[i] });

				return result;
			}
		} else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_MAP) {
			if (objects.size() != 2) throw std::runtime_error("Runtime Error: Expecting 2 arguments. Target ITERABLE and Function.");
			if (!objects[1]->isFunc) throw std::runtime_error("Runtime Error: Invalid Function arguments on 'reduce'.");

			if (!objects[0]->isList && !objects[0]->isMap) return objects[0];
			else {
				auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();

				for (int i = 0; i < newList->listValue.size(); i++)
					newList->listValue[i] = this->runFunction(objects[1]->funcValue, { newList->listValue[i] });
				return newList;
			}
		} else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_SORT_IF) {
			if (objects.size() != 2) throw std::runtime_error("Runtime Error: Expecting 2 arguments. Target ITERABLE and Function.");
			if (!objects[1]->isFunc) throw std::runtime_error("Runtime Error: Invalid Function arguments on 'reduce'.");

			if (!objects[0]->isList && !objects[0]->isMap) return objects[0];
			else {
				auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();
				std::sort(newList->listValue.begin(), newList->listValue.end(),
					[&](const std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2) {
						std::shared_ptr<HigherObject> conditionResult = this->runFunction(objects[1]->funcValue, { obj1, obj2 });
						conditionResult->castToBoolean(); return conditionResult->booleanValue;
					});
				return newList;
			}
		} else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_PARTIAL) {
			if (objects.size() < 1 ) throw std::runtime_error("Runtime Error: Expecting atleast 1 argument. Target Function.");
			if (!objects[0]->isFunc) throw std::runtime_error("Runtime Error: Invalid Function arguments on 'partial'.");
			auto newFunction              = std::make_shared<HigherObject::FunctionCall>();
			newFunction->funcName         = objects[0]->funcValue->funcName;
			newFunction->varNameAccesible = objects[0]->funcValue->varNameAccesible;
			newFunction->blockWillRun     = objects[0]->funcValue->blockWillRun;
			newFunction->parameters       = objects[0]->funcValue->parameters;
			newFunction->preArgs          = { objects.begin()+1, objects.end() };
			return std::make_shared<HigherObject>(newFunction);

		} else return NativeFunction::manageFunction( nativeType, objects );
		return nullptr;
	}

private:
	const std::shared_ptr<HigherObject> recursivelyCall(
		const std::shared_ptr<CallObjects> &callObj,
		const std::shared_ptr<Expression> &expressionAssign = nullptr) {

		auto tok = callObj->currObject->getToken();
		std::shared_ptr<HigherObject> returnValue;

		if (tok == JDM::TokenType::FUNCTIONS) {
			auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(callObj->currObject);
			std::shared_ptr<HigherObject::FunctionCall> newFunc;

			auto variables = this->variable->variables  .find(functionObj->returnStringValue());
			auto function  = this->variable->functionMap.find(functionObj->returnStringValue());

			     if (function  != this->variable->functionMap.end()) newFunc = function->second;
			// Used to run a lambda
			else if (variables != this->variable->variables  .end() && variables->second.second->isFunc) newFunc = variables->second.second->funcValue;
			else if (variables != this->variable->variables  .end() && variables->second.second->isObject) {
				for (auto &e : this->allInclude) {
					if (e.compare(variables->second.second->objectValue->className) == 0) {
						if (!variables->second.second->objectValue->fromMainSource)
							throw std::runtime_error("Runtime Error: This object is not callable.");

						auto newObj = std::make_shared<HigherObject>();
						newObj->castToObject();
						newObj->objectValue->className = variables->second.second->objectValue->className;
						newObj->objectValue->members   = variables->second.second->objectValue->members;
						newObj->objectValue->methods   = variables->second.second->objectValue->methods;
						newObj->objectValue->fromMainSource = false;

						returnValue = this->nativeClassMap[e]->constructor(
							newObj, this->getVectorHigherObject(functionObj->arguments));
						return this->manageEndCall(callObj, returnValue, expressionAssign);
					}
				}

				throw std::runtime_error("Runtime Error: No CONSTRUCTOR implemented yet.");
			}
			else {
				auto nativeFunc = NativeFunction::allNativeFunction.find(functionObj->returnStringValue());
				if (nativeFunc != NativeFunction::allNativeFunction.end()) {
					returnValue = this->handleNativeFunction(nativeFunc->second, this->getVectorHigherObject(functionObj->arguments));
					return this->manageEndCall( callObj, returnValue, expressionAssign );
				} else throw std::runtime_error("Runtime Error: Function is not declared.");
			}
			if (newFunc == nullptr) throw std::runtime_error("Runtime Error: Invalid Function.");
			returnValue = this->runFunction(newFunc, this->getVectorHigherObject(functionObj->arguments));

		} else if (tok == JDM::TokenType::EXPRESSION) {
			auto exprObj = std::dynamic_pointer_cast<ExpressionObjects>(callObj->currObject);

			auto variables = this->variable->variables.find(exprObj->returnStringValue());
			if (variables == this->variable->variables.end()) throw std::runtime_error("Runtime Error: Variable is not declared.");

			auto varList = variables->second.second;
			if (varList->isList || varList->isMap || varList->isString)
				returnValue = this->manageCallBrackets(callObj, varList, expressionAssign);
			else throw std::runtime_error("Runtime Error: Variable is not a String, List or a Map.");

		} else if (tok == JDM::TokenType::VARIABLE) {
			auto var = std::dynamic_pointer_cast<VariableObjects>(callObj->currObject);
			auto variables = this->variable->variables.find(var->returnStringValue());

			if (variables == this->variable->variables.end()) {
				throw std::runtime_error("Runtime Error: Variable is not declared.");
			}
			returnValue = variables->second.second;
		}
		else if (tok == JDM::TokenType::STRING) {
			returnValue = std::make_shared<HigherObject>(callObj->currObject->returnStringValue());
		}
		return this->manageEndCall(callObj, returnValue, expressionAssign);
	}

	// This is the return value on what you call. Example the return value of function is list.
	// Then you can still do operation because. you know, you can?.
	const std::shared_ptr<HigherObject> manageEndCall(
		const std::shared_ptr<CallObjects > &callObj,
		std::shared_ptr<HigherObject> &returnVal,
		const std::shared_ptr<Expression  > &expressionAssign = nullptr) {

		std::shared_ptr<HigherObject> newReturn;
		if (callObj->nextObject != nullptr) {
			if (returnVal == nullptr) throw std::runtime_error("Runtime Error: Return Value cannot be called or act like class.");

			auto newCallObj = callObj->nextObject;
			auto tok = newCallObj->currObject->getToken();

			if (returnVal->isFunc) {
				if (tok != JDM::TokenType::FUNCTIONS) throw std::runtime_error("Runtime Error: Return Value is Function. Not a class.");
				auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(newCallObj->currObject);
				if (functionObj->returnStringValue() != "(") throw std::runtime_error("Runtime Error: Return Value is Function. Not a class.");
				return this->runFunction(returnVal->funcValue, this->getVectorHigherObject(functionObj->arguments));

			} else if (returnVal->isObject) {
				if (tok == JDM::TokenType::FUNCTIONS) {
					auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(newCallObj->currObject);

						auto classFunc = this->nativeClassMap.find(returnVal->objectValue->className);
						if (classFunc != this->nativeClassMap.end()) {
							// Will never be true, as recursively call, will handle it, but just in case.
							if (functionObj->returnStringValue() == "(")
								newReturn = classFunc->second->constructor( returnVal,
									this->getVectorHigherObject(functionObj->arguments));
							else {
								auto func = classFunc->second->mapFunctions.find(functionObj->returnStringValue());
								if (func == classFunc->second->mapFunctions.end())
									throw std::runtime_error("Runtime Error: This method is not a member of class '" + returnVal->objectValue->className + "'.");
								newReturn = classFunc->second->manageFunction(func->second, returnVal, this->getVectorHigherObject(functionObj->arguments));
							}
						} else {

						}
				} else if (tok == JDM::TokenType::VARIABLE) {
					
				}
				return this->manageEndCall(newCallObj, newReturn);

			} else if (returnVal->isString) {
			 	if (tok == JDM::TokenType::EXPRESSION) newReturn = this->manageCallBrackets(newCallObj, returnVal, expressionAssign);
				else if (tok == JDM::TokenType::FUNCTIONS) {
					auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(newCallObj->currObject);
					if (functionObj->returnStringValue() == "(") throw std::runtime_error("Runtime Error: A List or Map cannot be called.");

					auto func = StringHigherFunctions::stringFunctions.find(functionObj->returnStringValue());
					if (func == StringHigherFunctions::stringFunctions.end()) throw std::runtime_error("Runtime Error: This function is not a member of class 'jstring'.");
					newReturn = StringHigherFunctions::manageFunction(func->second, returnVal, this->getVectorHigherObject(functionObj->arguments));
				}
				return this->manageEndCall(newCallObj, newReturn);

			} else if (returnVal->isList || returnVal->isMap) {
				if (tok == JDM::TokenType::EXPRESSION) newReturn = this->manageCallBrackets(newCallObj, returnVal, expressionAssign);
				else if (tok == JDM::TokenType::FUNCTIONS) {
					auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(newCallObj->currObject);
					if (functionObj->returnStringValue() == "(") throw std::runtime_error("Runtime Error: A List or Map cannot be called.");

					if (returnVal->isList) {
						// jlist native function. Acts like a class.
						auto func = ListHigherFunctions::listFunctions.find(functionObj->name);						
						if (func == ListHigherFunctions::listFunctions.end()) throw std::runtime_error("Runtime Error: This function is not a member of class 'jlist'.");
						newReturn = ListHigherFunctions::manageFunction(func->second, returnVal, this->getVectorHigherObject(functionObj->arguments));

					} else if (returnVal->isMap) {
						// jmap native function. Acts like a class.
						auto func = MapHigherFunctions::mapFunctions.find(functionObj->name);						
						if (func == MapHigherFunctions::mapFunctions.end()) throw std::runtime_error("Runtime Error: This function is not a member of class 'jmap'.");
						newReturn = MapHigherFunctions::manageFunction(func->second, returnVal, this->getVectorHigherObject(functionObj->arguments));

					} else throw std::runtime_error("Runtime Error: This function is not a member of Map.");

				}
				return this->manageEndCall(newCallObj, newReturn);
			}
			else throw std::runtime_error("Runtime Error: Return Value cannot be called or act like class.");
		}
		return returnVal;
	}

	const std::shared_ptr<HigherObject> manageCallBrackets(
		const std::shared_ptr<CallObjects > &next,
		const std::shared_ptr<HigherObject> &returnVal,
		const std::shared_ptr<Expression  > &expressionAssign = nullptr) {

		auto exprObj = std::dynamic_pointer_cast<ExpressionObjects>(next->currObject);
		auto expr    = this->evaluateExpression(exprObj->expression);

		if (returnVal->isList) {
			expr->castToInteger();
			int index = expr->integerValue;
			if (index < 0) index += returnVal->listValue.size();
			if (index < 0 || index >= returnVal->listValue.size())
				throw std::runtime_error("Runtime Error: Index out of bounds.");

			if (next->nextObject == nullptr && expressionAssign != nullptr) {
				if (returnVal->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
				auto exprResult = this->evaluateExpression(expressionAssign);
				auto oldValue   = returnVal->listValue[index];
					 if (next->operation == "+=") exprResult = this->newOperatedObject(oldValue, "+", exprResult);
				else if (next->operation == "-=") exprResult = this->newOperatedObject(oldValue, "-", exprResult);
				else if (next->operation == "*=") exprResult = this->newOperatedObject(oldValue, "*", exprResult);
				else if (next->operation == "/=") exprResult = this->newOperatedObject(oldValue, "/", exprResult);
				else if (next->operation == "%=") exprResult = this->newOperatedObject(oldValue, "%", exprResult);

				returnVal->listValue[index] = exprResult;
				returnVal->isListSorted     = false;
			}
			return returnVal->listValue[index];

		} else if (returnVal->isString) {
			expr->castToInteger();
			int index = expr->integerValue;
			if (index < 0) index += returnVal->stringValue.size();
			if (index < 0 || index >= returnVal->stringValue.size())
				throw std::runtime_error("Runtime Error: String index out of bounds.");
			return std::make_shared<HigherObject>(std::string(1, returnVal->stringValue[index]));

		} else if (returnVal->isMap) {

			if (next->nextObject == nullptr && expressionAssign != nullptr) {
				if (returnVal->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
				auto key = returnVal->isInMap(expr, true);
				auto exprResult = this->evaluateExpression(expressionAssign);
				if (key != nullptr) {
					auto oldValue   = returnVal->mapValue.at(key);
						 if (next->operation == "+=") exprResult = this->newOperatedObject(oldValue, "+", exprResult);
					else if (next->operation == "-=") exprResult = this->newOperatedObject(oldValue, "-", exprResult);
					else if (next->operation == "*=") exprResult = this->newOperatedObject(oldValue, "*", exprResult);
					else if (next->operation == "/=") exprResult = this->newOperatedObject(oldValue, "/", exprResult);
					else if (next->operation == "%=") exprResult = this->newOperatedObject(oldValue, "%", exprResult);
				} else if (next->operation != "=")
					throw std::runtime_error("Runtime Error: Key in map doesn't exist. Cannot do operation.");

				returnVal->mapValue[((key != nullptr) ? key : expr)] = exprResult;
				return exprResult;

			} else {
				auto returnValue = returnVal->isInMap(expr);
				if (returnValue == nullptr) throw std::runtime_error("Runtime Error: Map key is not found");
				return returnValue;
			}
		}
		return nullptr;
	}

	const std::vector<std::shared_ptr<HigherObject>> getVectorHigherObject(
		const std::vector<std::shared_ptr<Expression>> &expr) {
		std::vector<std::shared_ptr<HigherObject>> result;
		for (const auto &ex : expr) result.push_back(this->evaluateExpression(ex));
		return result;
	}

	const void addVariable(
		const std::shared_ptr<Expression> &expr,
		DataTypeEnum dataT, const std::string &varName,
		bool isConst = false, bool isForce = false,
		const std::string &operation = "=") {

		auto var = this->evaluateExpression(expr);
			 if (operation == "+=") var = this->newOperatedObject(this->variable->variables[varName].second, "+", var);
		else if (operation == "-=") var = this->newOperatedObject(this->variable->variables[varName].second, "-", var);
		else if (operation == "*=") var = this->newOperatedObject(this->variable->variables[varName].second, "*", var);
		else if (operation == "/=") var = this->newOperatedObject(this->variable->variables[varName].second, "/", var);
		else if (operation == "%=") var = this->newOperatedObject(this->variable->variables[varName].second, "%", var);

		var->isConstant         = isConst;
		var->isForcedConstraint = isForce;

		this->variable->variables[varName] = std::make_pair(dataT, this->checkVariableConstraint(var, dataT));
	}

	const std::pair<DataTypeEnum, std::shared_ptr<HigherObject>> returnVariable(
		const std::shared_ptr<Expression> &expr, DataTypeEnum dataT) {
		auto var = this->castVariable(expr, dataT);
		return std::make_pair(dataT, var);
	}

	const std::shared_ptr<HigherObject> castVariable(
		const std::shared_ptr<Expression> &expr, DataTypeEnum dataT, bool checkConstraint = true) {
		auto var = this->evaluateExpression(expr);
		return this->checkVariableConstraint(var, dataT, checkConstraint);
	}

	const std::shared_ptr<HigherObject> checkVariableConstraint(
		const std::shared_ptr<HigherObject> &var, DataTypeEnum dataT, bool checkConstraint = true) {

		if (var->isForcedConstraint && dataT != DataTypeEnum::DATA_ANY && checkConstraint) {
			DataTypeEnum varType = var->getDatatypeEnum();
			if (dataT == DataTypeEnum::DATA_STRING) {
				if (dataT != varType) throw std::runtime_error("Runtime Error: Due to force constraint. Value is not a String.");
			} else if (dataT == DataTypeEnum::DATA_INTEGER
					|| dataT == DataTypeEnum::DATA_DOUBLE
					|| dataT == DataTypeEnum::DATA_BOOLEAN) {
				if (varType != DataTypeEnum::DATA_INTEGER
				 && varType != DataTypeEnum::DATA_DOUBLE
				 && varType != DataTypeEnum::DATA_BOOLEAN) 
					throw std::runtime_error("Runtime Error: Due to force constraint. Value is not a Number.");
			} else if (dataT == DataTypeEnum::DATA_LAMBDA) {
				if (dataT != varType) throw std::runtime_error("Runtime Error: Due to force constraint. Value is not a Function or Lambda.");
			} else if (dataT == DataTypeEnum::DATA_OBJECT) {
				if (dataT != varType) throw std::runtime_error("Runtime Error: Due to force constraint. Value is not a Object.");
			} else if (dataT == DataTypeEnum::DATA_LIST) {
				if (dataT != varType) throw std::runtime_error("Runtime Error: Due to force constraint. Value is not a List.");
			} else if (dataT == DataTypeEnum::DATA_MAP) {
				if (dataT != varType) throw std::runtime_error("Runtime Error: Due to force constraint. Value is not a Map.");
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
		if (instruction == nullptr) return nullptr;
		auto ifState = std::dynamic_pointer_cast<IfStatement>(instruction);
		if (ifState->condition != nullptr) {
			auto condition = this->evaluateExpression(ifState->condition);
			condition->castToBoolean();

			if (!condition->booleanValue) return this->processIfStatement(ifState->elseIf);
		}
		return this->compile(ifState->blockWillRun, this->variable->variables, {}, this->variable->functionMap);
	}

	const std::shared_ptr<HigherObject> processForStatement(
		const std::shared_ptr<Instruction> &instruction)
	{
		auto forState = std::dynamic_pointer_cast<ForLoopStatement>(instruction);

		std::shared_ptr<HigherObject> start;
		std::shared_ptr<HigherObject> step ;
		std::shared_ptr<HigherObject> stop    = this->evaluateExpression(forState->stop);
		if (forState->start != nullptr) start = this->evaluateExpression(forState->start);
		if (forState->step  != nullptr) step  = this->evaluateExpression(forState->step);
		if (start == nullptr) start = std::make_shared<HigherObject>(static_cast<int64_t>(0));
		if (step  == nullptr) step  = std::make_shared<HigherObject>(static_cast<int64_t>(1));

		start->castToInteger();
		step ->castToInteger();
		stop ->castToInteger();

		std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> additionalVariables;
		additionalVariables[forState->variable->returnStringValue()] = std::make_pair(DataTypeEnum::DATA_INTEGER, start);
		while (start->integerValue != stop->integerValue) {			
			auto returnValue = this->compile(
				forState->blockWillRun,
				this->variable->variables,
				additionalVariables,
				this->variable->functionMap);
			if (returnValue != nullptr) return returnValue;
			start->integerValue += step->integerValue;
		}
		return nullptr;
	}

	const std::shared_ptr<HigherObject> processWhileLoop(
		const std::shared_ptr<Instruction> &instruction)
	{
		auto whileState = std::dynamic_pointer_cast<WhileStatement>(instruction);
		auto expr       = this->evaluateExpression(whileState->condition);
		expr->castToBoolean(); bool check = expr->booleanValue;

		while (check) {
			auto returnValue = this->compile( whileState->blockWillRun,
				this->variable->variables, {}, this->variable->functionMap);

			if (returnValue != nullptr) return returnValue;
			auto expr = this->evaluateExpression(whileState->condition);
			expr->castToBoolean();
			check = expr->booleanValue;
		}
		return nullptr;
	}

	JDM_DLL std::shared_ptr<HigherObject> newOperatedObject(
		std::shared_ptr<HigherObject> &firstVal,
		const std::string &operation = "",
		const std::shared_ptr<HigherObject> &secondVal = nullptr)
	{
		if (operation == "") return firstVal;
		if (secondVal != nullptr) {

			if (firstVal->isString) {
				if (operation == "+") {
					secondVal->castToString();
					firstVal->stringValue += secondVal->stringValue;
				}
				else if (operation == "*") {
					if (!secondVal->isInteger && !secondVal->isDecimal)
						throw std::runtime_error("Runtime Error: Must be a Number to multiply to String.");

					secondVal->castToInteger();
					if (secondVal->integerValue < 0)
						throw std::runtime_error("Runtime Error: Integer must be greater than 0.");

					std::string temporary = firstVal->stringValue;
					for (int i = 1; i < secondVal->integerValue; i++)
						firstVal->stringValue += temporary;

				} else {
					if (!secondVal->isString) throw std::runtime_error("Runtime Error: Cannot do operation on String and not String.");

					bool result = false;
					bool firstBool  = !firstVal ->stringValue.empty();
					bool secondBool = !secondVal->stringValue.empty();

						 if (operation == "&&") result = firstBool && secondBool;
					else if (operation == "||") result = firstBool || secondBool;
					else if (operation == "==") result = firstVal->stringValue == secondVal->stringValue;
					else if (operation == "!=") result = firstVal->stringValue != secondVal->stringValue;
					else if (operation == "<" ) result = firstVal->stringValue.size() <  secondVal->stringValue.size();
					else if (operation == ">" ) result = firstVal->stringValue.size() >  secondVal->stringValue.size();
					else if (operation == "<=") result = firstVal->stringValue.size() <= secondVal->stringValue.size();
					else if (operation == ">=") result = firstVal->stringValue.size() >= secondVal->stringValue.size();
					else throw std::runtime_error("Runtime Error: This operation is not allowed on String.");

					firstVal->castToBoolean();
					firstVal->booleanValue = result;
				}

			} else if (firstVal->isList) {
				if (operation == "+") {
					secondVal->castToList();
					firstVal->listValue.insert(
						firstVal ->listValue.end(),
						secondVal->listValue.begin(),
						secondVal->listValue.end()
					);
				} else {
					if (!secondVal->isList) throw std::runtime_error("Runtime Error: Cannot do operation on List and not List.");

					bool result = false;
					bool firstBool  = !firstVal ->listValue.empty();
					bool secondBool = !secondVal->listValue.empty();

						 if (operation == "&&") result = firstBool && secondBool;
					else if (operation == "||") result = firstBool || secondBool;
					else if (operation == "==") result =  firstVal->compareHigherObject(secondVal);
					else if (operation == "!=") result = !firstVal->compareHigherObject(secondVal);
					else if (operation == "<" ) result = firstVal->listValue.size() <  secondVal->listValue.size();
					else if (operation == ">" ) result = firstVal->listValue.size() >  secondVal->listValue.size();
					else if (operation == "<=") result = firstVal->listValue.size() <= secondVal->listValue.size();
					else if (operation == ">=") result = firstVal->listValue.size() >= secondVal->listValue.size();
					else throw std::runtime_error("Runtime Error: This operation is not allowed on List.");

					firstVal->castToBoolean();
					firstVal->booleanValue = result;
				}

			} else if (firstVal->isMap) {
				if (!secondVal->isMap) throw std::runtime_error("Runtime Error: Cannot do operation on 'jmap' and not 'jmap'.");
				if (operation == "+") {
					for (const auto& element : secondVal->mapValue) {
						auto isInKey = firstVal->isInMap(element.first, true);
						if (isInKey) firstVal->mapValue[isInKey] = element.second;
						else firstVal->mapValue[element.first] = element.second;
					}
				} else {
					bool result = false;
					bool firstBool  = !firstVal ->mapValue.empty();
					bool secondBool = !secondVal->mapValue.empty();

						 if (operation == "&&") result = firstBool && secondBool;
					else if (operation == "||") result = firstBool || secondBool;
					else if (operation == "==") result =  firstVal->compareHigherObject(secondVal);
					else if (operation == "!=") result = !firstVal->compareHigherObject(secondVal);
					else if (operation == "<" ) result = firstVal->mapValue.size() <  secondVal->mapValue.size();
					else if (operation == ">" ) result = firstVal->mapValue.size() >  secondVal->mapValue.size();
					else if (operation == "<=") result = firstVal->mapValue.size() <= secondVal->mapValue.size();
					else if (operation == ">=") result = firstVal->mapValue.size() >= secondVal->mapValue.size();
					else throw std::runtime_error("Runtime Error: This operation is not allowed on 'jmap'.");

					firstVal->castToBoolean();
					firstVal->booleanValue = result;
				}

			} else if (firstVal->isFunc) {
				if (!secondVal->isFunc) throw std::runtime_error("Runtime Error: Cannot do operation on 'jfunc' and not 'jfunc'.");
				bool result = false;
				bool firstBool  = firstVal ->funcValue != nullptr;
				bool secondBool = secondVal->funcValue != nullptr;
					 if (operation == "&&") result = firstBool && secondBool;
				else if (operation == "||") result = firstBool || secondBool;
				else if (operation == "==") result =  firstVal->compareHigherObject(secondVal);
				else if (operation == "!=") result = !firstVal->compareHigherObject(secondVal);
				else throw std::runtime_error("Runtime Error: This operation is not allowed on 'jfunc'.");

				firstVal->castToBoolean();
				firstVal->booleanValue = result;

			} else if (firstVal->isObject) {
				if (!secondVal->isObject) throw std::runtime_error("Runtime Error: Cannot do operation on 'jobject' and not 'jobject'.");
				bool result = false;
				bool firstBool  = firstVal ->objectValue != nullptr;
				bool secondBool = secondVal->objectValue != nullptr;
					 if (operation == "&&") result = firstBool && secondBool;
				else if (operation == "||") result = firstBool || secondBool;
				else if (operation == "==") result =  firstVal->compareHigherObject(secondVal);
				else if (operation == "!=") result = !firstVal->compareHigherObject(secondVal);
				else throw std::runtime_error("Runtime Error: This operation is not allowed on 'class'.");

				firstVal->castToBoolean();
				firstVal->booleanValue = result;

			} else if (firstVal->isInteger || firstVal->isDecimal || firstVal->isBoolean) {
				if (secondVal->isString) throw std::runtime_error("Runtime Error: Cannot do operation on Number and 'jstring'.");
				if (secondVal->isFunc  ) throw std::runtime_error("Runtime Error: Cannot do operation on Number and 'func'.");
				if (secondVal->isList  ) throw std::runtime_error("Runtime Error: Cannot do operation on Number and 'jlist.");
				if (secondVal->isMap   ) throw std::runtime_error("Runtime Error: Cannot do operation on Number and 'jmap'.");

				firstVal ->castToDecimal();
				secondVal->castToDecimal();

				bool result = false;
				bool firstBool  = firstVal ->doubleValue > 0.0;
				bool secondBool = secondVal->doubleValue > 0.0;

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
				else {
						 if (operation == "&&") result = firstBool && secondBool;
					else if (operation == "||") result = firstBool || secondBool;
					else if (operation == "==") result = firstVal->doubleValue == secondVal->doubleValue;
					else if (operation == "!=") result = firstVal->doubleValue != secondVal->doubleValue;
					else if (operation == "<" ) result = firstVal->doubleValue <  secondVal->doubleValue;
					else if (operation == ">" ) result = firstVal->doubleValue >  secondVal->doubleValue;
					else if (operation == "<=") result = firstVal->doubleValue <= secondVal->doubleValue;
					else if (operation == ">=") result = firstVal->doubleValue >= secondVal->doubleValue;
					firstVal->castToBoolean();
					firstVal->booleanValue = result;
				}
			}
		} else {
			if (operation == "!" ) {
				firstVal->castToBoolean();
				firstVal->booleanValue = !firstVal->booleanValue;
			}
		}
		return firstVal;
	}

	JDM_DLL std::shared_ptr<HigherObject> getHigherObject(
		const std::shared_ptr<VarObjects> &Value,
		const std::shared_ptr<Expression> &Expression)
	{
		std::shared_ptr<HigherObject> value;
		if ( Value ) {
			JDM::TokenType type = Value->getToken();
			if (type == JDM::TokenType::VARIABLE)
			{
				auto varName = std::dynamic_pointer_cast<VariableObjects>(Value)->returnStringValue();
				auto var  = this->variable->variables.  find(varName);
				auto func = this->variable->functionMap.find(varName);
				     if (var  != this->variable->variables  .end()) value = std::make_shared<HigherObject>(var->second.second);
				else if (func != this->variable->functionMap.end()) value = std::make_shared<HigherObject>(func->second);
				else {
					auto nativeFunc = NativeFunction::allNativeFunction.find(varName);
					if (nativeFunc != NativeFunction::allNativeFunction.end()) {

						auto newFunction = std::make_shared<HigherObject::FunctionCall>();
						newFunction->funcName = "$native-" + varName;
						value = std::make_shared<HigherObject>(newFunction);

					} else throw std::runtime_error("Runtime Error: Variable or Function is not declared.");
				}
			}
			else if (type == JDM::TokenType::STRING ) value = std::make_shared<HigherObject>(std::dynamic_pointer_cast<StringObjects >(Value)->returnValue());
			else if (type == JDM::TokenType::INTEGER) value = std::make_shared<HigherObject>(std::dynamic_pointer_cast<IntegerObjects>(Value)->returnValue());
			else if (type == JDM::TokenType::DOUBLE) value = std::make_shared<HigherObject>(std::dynamic_pointer_cast<DoubleObjects >(Value)->returnValue());
			else if (type == JDM::TokenType::LIST)
			{
				std::vector<std::shared_ptr<HigherObject>> vecList;
				auto exprList = std::dynamic_pointer_cast<ListObject>(Value)->returnValue();
				for (const auto &expr : exprList) vecList.push_back( this->evaluateExpression(expr) );
				value = std::make_shared<HigherObject>(vecList);
			}
			else if (type == JDM::TokenType::MAP)
			{
				std::unordered_map<std::shared_ptr<HigherObject>, std::shared_ptr<HigherObject>> mapList;
				auto mapStruct = std::dynamic_pointer_cast<MapObject>(Value)->returnValue();
				for (const auto &expr : mapStruct) mapList[this->evaluateExpression(expr->key)] = this->evaluateExpression(expr->value);
				value = std::make_shared<HigherObject>(mapList);
			}
			else if (type == JDM::TokenType::CALL_OBJ)
			{
				auto callObjRoot = std::dynamic_pointer_cast<CallObjects>(Value);
				while (callObjRoot->prevObject != nullptr)
					callObjRoot = callObjRoot->prevObject;

				auto result = this->recursivelyCall(callObjRoot);
				if (result == nullptr) value = nullptr;
				else value = std::make_shared<HigherObject>(result);
			}
			else if (type == JDM::TokenType::LAMBDA)
			{
				auto lambdaObj   = std::dynamic_pointer_cast<LambdaObjects>(Value);
				auto newFunction = std::make_shared<HigherObject::FunctionCall>();

				for (const auto &var : lambdaObj->parameters) {
					if (var->dataType != nullptr) {
						auto dataT   = JDM::dataTypeMap.at(std::get<0>(var->dataType->token));
						newFunction->parameters[var->varName->returnStringValue()] = this->returnVariable(var->varValue, dataT);
					} else newFunction->parameters[var->varName->returnStringValue()] = this->returnVariable(var->varValue, DataTypeEnum::DATA_ANY);
				}
				newFunction->blockWillRun      = std::dynamic_pointer_cast<Block>(lambdaObj->blockWillRun);
				newFunction->varNameAccesible  = this->getAllVarName(this->variable->variables);
				if (lambdaObj->willCall) value = this->runFunction(newFunction, this->getVectorHigherObject(lambdaObj->arguments));
				else value = std::make_shared<HigherObject>(newFunction);
			}
			else if (type == JDM::TokenType::CAST)
			{
				auto castObj = std::dynamic_pointer_cast<CastObjects>(Value);
				value = this->castVariable(castObj->expression, castObj->datTypeToTurn, false);
			}
		} else value = this->evaluateExpression(Expression);
		return value;
	}

	JDM_DLL std::shared_ptr<HigherObject> evaluateExpression(
		const std::shared_ptr<Expression> &expr)
	{
		if (!expr) return std::make_shared<HigherObject>(static_cast<int64_t>(0));
		auto firstVal = this->getHigherObject(expr->firstValue, expr->firstExpression);
		if (firstVal == nullptr) throw std::runtime_error("Runtime Error: Invalid Expression. Return null.");

		if (expr->opWillUse) {
			if (expr->secondValue || expr->secondExpression) {
				auto second = this->getHigherObject  (expr->secondValue, expr->secondExpression);
				firstVal    = this->newOperatedObject(firstVal, std::get<0>(expr->opWillUse->token), second);
			} else firstVal = this->newOperatedObject(firstVal, std::get<0>(expr->opWillUse->token), nullptr);
		}
		return firstVal;
	}
};
