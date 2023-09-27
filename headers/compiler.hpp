#pragma once
#include <cmath>
#include <chrono>
#include <thread>
#include "higherObject.hpp"

class Compiler {
private:
	std::shared_ptr<Block> __mainBlock;

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
		for (const auto& element : _AdditionalVariables)
    		this->variable->variables[element.first] = element.second;
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

				if ( var != this->variable->variables.end())
					this->addVariable(assign->expression, var->second.first, assign->varName->returnStringValue(),
						var->second.second->isConstant, var->second.second->isForcedConstraint);

				else throw std::runtime_error("Runtime Error: Variable is not declared.");

			} else if (instruction->getType() == LoggerInstruction) {
				auto logger = std::dynamic_pointer_cast<Logger>(instruction);
				for (const auto &expr : logger->expressions) {
					this->evaluateExpression(expr)->logValue();
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
			} else if (instruction->getType() == CreateFunctionInstruction) {
				this->processCreateFunction(instruction);

			} else if (instruction->getType() == CallInstruction) {
				this->processCallFunction(instruction);

			} else if (instruction->getType() == ReturnInstruction) {
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
    	for (const auto& pair : _variables) {
     	   keys.push_back(pair.first);
    	}
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

	const std::shared_ptr<HigherObject> recursivelyCall(
		const std::shared_ptr<CallObjects> &callObj,
		const std::shared_ptr<Expression> &expressionAssign = nullptr) {

		auto tok = callObj->currObject->getToken();
		std::shared_ptr<HigherObject> returnValue;

		if (tok == TokenType::FUNCTIONS) {
			auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(callObj->currObject);
			auto nativeFunc = allNativeFunction.find(functionObj->returnStringValue());

			if (nativeFunc != allNativeFunction.end()) {
				if (nativeFunc->second == NativeFunction::NATIVE_INPUT) {
					for (const auto &param : functionObj->arguments)
						this->evaluateExpression(param)->logValue();

					std::string input;
    				std::getline(std::cin, input);
    				returnValue = std::make_shared<HigherObject>(input);
					return this->manageEndCall(callObj, returnValue, expressionAssign);
				}
			}

			auto function = this->variable->functionMap.find(functionObj->returnStringValue());

			if (function == this->variable->functionMap.end())
				throw std::runtime_error("Runtime Error: Function is not declared.");
    		if (functionObj->arguments.size() > function->second->parameters.size())
    			throw std::runtime_error("Runtime Error: Too many arguments.");

    		std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> scopedVariables;
			for (const auto& varName : function->second->varNameAccesible)
    			scopedVariables[varName] = this->variable->variables.at(varName);

    		int index = 0;
    		std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> newParam
    			= function->second->parameters;

    		for (const auto &param : function->second->parameters) {
    			if (index == functionObj->arguments.size()) break;
				auto var = this->castVariable(functionObj->arguments[index++], param.second.first);
				newParam[param.first] = std::make_pair(param.second.first, var);
    		}
			returnValue = this->compile(function->second->blockWillRun,
				scopedVariables, newParam, this->variable->functionMap);

		} else if (tok == TokenType::EXPRESSION) {
			auto exprObj = std::dynamic_pointer_cast<ExpressionObjects>(callObj->currObject);

			auto variables = this->variable->variables.find(exprObj->returnStringValue());
			if (variables == this->variable->variables.end())
				throw std::runtime_error("Runtime Error: Variable is not declared.");

			auto varList = variables->second.second;
			if (!varList->isList && !varList->isMap)
				throw std::runtime_error("Runtime Error: Variable is not a List or a Map.");
			auto expr = this->evaluateExpression(exprObj->expression);

			if (varList->isList) {
				expr->castToInteger();
				int index = expr->integerValue;
				if (index < 0) index += varList->listValue.size();
				if (index < 0 || index >= varList->listValue.size())
    				throw std::runtime_error("Runtime Error: Index out of bounds.");

				if (callObj->nextObject == nullptr && expressionAssign != nullptr) {
					if (varList->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
					varList->listValue[index] = this->evaluateExpression(expressionAssign);
					return varList->listValue[index];
				} else returnValue = varList->listValue[index];
			} else if (varList->isMap) {

				if (callObj->nextObject == nullptr && expressionAssign != nullptr) {
					if (varList->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
					auto exprResult = this->evaluateExpression(expressionAssign);
					auto key = varList->isInMap(expr, true);
					varList->mapValue[((key != nullptr) ? key : expr)] = exprResult;
					return exprResult;
				} else {
					returnValue = varList->isInMap(expr);
					if (returnValue == nullptr) throw std::runtime_error("Runtime Error: Map key is not found");
				}
			}

		} else if (tok == TokenType::VARIABLE) {
			auto var = std::dynamic_pointer_cast<VariableObjects>(callObj->currObject);
			auto variables = this->variable->variables.find(var->returnStringValue());
			if (variables == this->variable->variables.end())
				throw std::runtime_error("Runtime Error: Variable is not declared.");

			returnValue = variables->second.second;
		}
		return this->manageEndCall(callObj, returnValue, expressionAssign);
	}

	const std::shared_ptr<HigherObject> manageEndCall(
		const std::shared_ptr<CallObjects > &callObj,
		std::shared_ptr<HigherObject> &returnVal,
		const std::shared_ptr<Expression  > &expressionAssign = nullptr) {

		std::shared_ptr<HigherObject> newReturn;
		if (callObj->nextObject != nullptr) {
			if (returnVal == nullptr)
				throw std::runtime_error("Runtime Error: Return Value cannot be called or act like class.");

			auto newCallObj = callObj->nextObject;
			auto tok = newCallObj->currObject->getToken();

			if (returnVal->isFunc) {
				if (tok != TokenType::FUNCTIONS)
					throw std::runtime_error("Runtime Error: Return Value is Function. Not a class.");

				auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(newCallObj->currObject);
				if (functionObj->returnStringValue() != "(")
					throw std::runtime_error("Runtime Error: Return Value is Function. Not a class.");

				functionObj->name = returnVal->funcValue->funcName;
				return this->recursivelyCall(newCallObj);

			// LIST and MAP first, next is INTEGER, DOUBLE, STRING. but for now. List and Map
			} else if (returnVal->isList || returnVal->isMap) {
				if (tok == TokenType::EXPRESSION) {
					newReturn = this->manageCallBrackets(newCallObj, returnVal, expressionAssign);

				} else if (tok == TokenType::FUNCTIONS) {
					auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(newCallObj->currObject);
					if (functionObj->returnStringValue() == "(")
						throw std::runtime_error("Runtime Error: A List or Map cannot be called.");

					// CUSTOM LIST FUNCTION, like at. or something
					if (returnVal->isList) {
						auto func = ListHigherFunctions::listFunctions.find(functionObj->name);
						if (func == ListHigherFunctions::listFunctions.end())
							throw std::runtime_error("Runtime Error: This function is not a member of class 'jlist'.");

						if (func->second == ListHigherFunctions::ListFunction::list_sort) {
							if (!functionObj->arguments.empty()) throw std::runtime_error("Runtime Error: Expecting 0 arguments.");
							ListHigherFunctions::sort(returnVal);

						} else if (func->second == ListHigherFunctions::ListFunction::list_search) {
							if (functionObj->arguments.size() != 1) throw std::runtime_error("Runtime Error: Expecting 2 arguments.");
							newReturn = std::make_shared<HigherObject>(
								static_cast<int64_t>(ListHigherFunctions::search(returnVal,
									this->evaluateExpression(functionObj->arguments[0])
								)));

						} else if (func->second == ListHigherFunctions::ListFunction::list_insert) {
							if (functionObj->arguments.size() < 1 && functionObj->arguments.size() > 2) throw std::runtime_error("Runtime Error: Expecting 1 or 2 arguments.");
							int index = -1;
							if (functionObj->arguments.size() == 2) {
								auto indexValue = this->evaluateExpression(functionObj->arguments[1]);
								indexValue->castToInteger();
								index = indexValue->integerValue;
							}
							ListHigherFunctions::insert(returnVal, this->evaluateExpression(functionObj->arguments[0]), index);

						} else if (func->second == ListHigherFunctions::ListFunction::list_delete) {
							if (functionObj->arguments.size() != 1) throw std::runtime_error("Runtime Error: Expecting 1 arguments.");

							auto indexValue = this->evaluateExpression(functionObj->arguments[0]);
							indexValue->castToInteger();
							ListHigherFunctions::del(returnVal, indexValue->integerValue);

						} else if (func->second == ListHigherFunctions::ListFunction::list_at) {
							if (functionObj->arguments.size() != 1) throw std::runtime_error("Runtime Error: Expecting 1 arguments.");

							auto indexValue = this->evaluateExpression(functionObj->arguments[0]);
							indexValue->castToInteger();
							newReturn = ListHigherFunctions::at(returnVal, indexValue->integerValue);

						} else if (func->second == ListHigherFunctions::ListFunction::list_push_back) {
							if (functionObj->arguments.size() != 1) throw std::runtime_error("Runtime Error: Expecting 1 arguments.");
							ListHigherFunctions::push_back(returnVal, this->evaluateExpression(functionObj->arguments[0]));

						} else if (func->second == ListHigherFunctions::ListFunction::list_push_front) {
							if (functionObj->arguments.size() != 1) throw std::runtime_error("Runtime Error: Expecting 1 arguments.");
							ListHigherFunctions::push_front(returnVal, this->evaluateExpression(functionObj->arguments[0]));

						} else if (func->second == ListHigherFunctions::ListFunction::list_pop_back) {
							if (!functionObj->arguments.empty()) throw std::runtime_error("Runtime Error: Expecting 0 arguments.");
							ListHigherFunctions::pop_back(returnVal);

						} else if (func->second == ListHigherFunctions::ListFunction::list_pop_front) {
							if (!functionObj->arguments.empty()) throw std::runtime_error("Runtime Error: Expecting 0 arguments.");
							ListHigherFunctions::pop_front(returnVal);

						} else {
							throw std::runtime_error("Runtime Error: This function is not a member of class 'jlist'.");
						}

					// MAP is work in progress
					} else throw std::runtime_error("Runtime Error: This function is not a member of Map.");

				}
				return this->manageEndCall(newCallObj, newReturn);

			} else throw std::runtime_error("Runtime Error: Return Value cannot be called or act like class.");
		}
		return returnVal;
	}

	const std::shared_ptr<HigherObject> manageCallBrackets(
		const std::shared_ptr<CallObjects> &next,
		const std::shared_ptr<HigherObject> &returnVal,
		const std::shared_ptr<Expression> &expressionAssign = nullptr) {

		auto exprObj = std::dynamic_pointer_cast<ExpressionObjects>(next->currObject);
		auto expr = this->evaluateExpression(exprObj->expression);

		if (returnVal->isList) {
			expr->castToInteger();
			int index = expr->integerValue;
			if (index < 0) index += returnVal->listValue.size();

			if (index < 0 || index >= returnVal->listValue.size())
    			throw std::runtime_error("Runtime Error: Index out of bounds.");

    		if (next->nextObject == nullptr && expressionAssign != nullptr) {
    			if (returnVal->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
				returnVal->listValue[index] = this->evaluateExpression(expressionAssign);
    		}

			return returnVal->listValue[index];

		} else if (returnVal->isMap) {
			if (next->nextObject == nullptr && expressionAssign != nullptr) {
				if (returnVal->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
				auto exprResult = this->evaluateExpression(expressionAssign);
				auto key = returnVal->isInMap(expr, true);
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

		auto expr = this->evaluateExpression(whileState->condition);
		expr->castToBoolean();
		bool check = expr->booleanValue;
		while (check) {
			auto returnValue = this->compile(
				whileState->blockWillRun,
				this->variable->variables, {},
				this->variable->functionMap);
			if (returnValue != nullptr) return returnValue;

			auto expr = this->evaluateExpression(whileState->condition);
			expr->castToBoolean();
			check = expr->booleanValue;
		}
		return nullptr;
	}

	const void addVariable(
		const std::shared_ptr<Expression> &expr,
		DataTypeEnum dataT, const std::string &varName,
		bool isConst = false, bool isForce = false) {

		auto var = this->evaluateExpression(expr);
		var->isConstant         = isConst;
		var->isForcedConstraint = isForce;

		this->variable->variables[varName] = std::make_pair(
			dataT, this->checkVariableConstraint(var, dataT));
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
		else if (dataT == DataTypeEnum::DATA_LIST   ) var->castToList    ();
		else if (dataT == DataTypeEnum::DATA_MAP    ) var->castToMap     ();
		return var;
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

			} else if (firstVal->isInteger || firstVal->isDecimal || firstVal->isBoolean) {
				if (secondVal->isString) throw std::runtime_error("Runtime Error: Cannot do operation on Number and String.");
				if (secondVal->isFunc  ) throw std::runtime_error("Runtime Error: Cannot do operation on Number and Function.");
				if (secondVal->isList  ) throw std::runtime_error("Runtime Error: Cannot do operation on Number and List.");
				if (secondVal->isMap   ) throw std::runtime_error("Runtime Error: Cannot do operation on Number and Map.");

				firstVal ->castToDecimal();
				secondVal->castToDecimal();

				bool result = false;
				bool firstBool  = firstVal ->doubleValue > 0.0;
				bool secondBool = secondVal->doubleValue > 0.0;

				if      (operation == "+" ) firstVal->doubleValue += secondVal->doubleValue;
				else if (operation == "%" ) firstVal->doubleValue =  static_cast<int>(firstVal->doubleValue) % static_cast<int>(secondVal->doubleValue);
				else if (operation == "-" ) firstVal->doubleValue -= secondVal->doubleValue;
				else if (operation == "*" ) firstVal->doubleValue *= secondVal->doubleValue;
				else if (operation == "**") firstVal->doubleValue = pow(firstVal->doubleValue, secondVal->doubleValue);

				else if (operation == "/" ) firstVal->doubleValue /= secondVal->doubleValue;
				else if (operation == "//") firstVal->doubleValue = static_cast<int64_t>(firstVal->doubleValue / secondVal->doubleValue);

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
			TokenType type = Value->getToken();
			if (type == TokenType::VARIABLE) {
				auto varName = std::dynamic_pointer_cast<VariableObjects>(Value)->returnStringValue();
    			auto var  = this->variable->variables.  find(varName);
    			auto func = this->variable->functionMap.find(varName);

    			if (var != this->variable->variables.end()) {
			        value = std::make_shared<HigherObject>(var->second.second);
			    } else if (func != this->variable->functionMap.end()) {
			        value = std::make_shared<HigherObject>(func->second);
    			} else {
        			throw std::runtime_error("Runtime Error: Variable is not declared.");
    			}

			} else if (type == TokenType::STRING) {
			    value = std::make_shared<HigherObject>(std::dynamic_pointer_cast<StringObjects >(Value)->returnValue());
			} else if (type == TokenType::INTEGER) {
			    value = std::make_shared<HigherObject>(std::dynamic_pointer_cast<IntegerObjects>(Value)->returnValue());
			} else if (type == TokenType::DECIMAL) {
			    value = std::make_shared<HigherObject>(std::dynamic_pointer_cast<DoubleObjects >(Value)->returnValue());
			} else if (type == TokenType::LIST) {

				std::vector<std::shared_ptr<HigherObject>> vecList;
				auto exprList = std::dynamic_pointer_cast<ListObject>(Value)->returnValue();
				for (const auto &expr : exprList) {
					vecList.push_back( this->evaluateExpression(expr) );
				}
			    value = std::make_shared<HigherObject>(vecList);

			} else if (type == TokenType::MAP) {
				std::unordered_map<std::shared_ptr<HigherObject>, std::shared_ptr<HigherObject>> mapList;
				auto mapStruct = std::dynamic_pointer_cast<MapObject>(Value)->returnValue();
				for (const auto &expr : mapStruct) {
					mapList[this->evaluateExpression(expr->key)] = this->evaluateExpression(expr->value);
				}
			    value = std::make_shared<HigherObject>(mapList);

			} else if (type == TokenType::CALL_OBJ) {

				auto callObjRoot = std::dynamic_pointer_cast<CallObjects>(Value);
				while (callObjRoot->prevObject != nullptr)
					callObjRoot = callObjRoot->prevObject;

			    value = std::make_shared<HigherObject>(this->recursivelyCall(callObjRoot));
			} else if (type == TokenType::CAST) {
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

		if (expr->opWillUse) {
			if (expr->secondValue || expr->secondExpression) {
				auto second = this->getHigherObject  (expr->secondValue, expr->secondExpression);
				firstVal    = this->newOperatedObject(firstVal, std::get<0>(expr->opWillUse->token), second);
			} else firstVal = this->newOperatedObject(firstVal, std::get<0>(expr->opWillUse->token), nullptr);
		}
		return firstVal;
	}
};
