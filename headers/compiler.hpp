#pragma once
#include "higherObject.hpp"
#include "defines.hpp"

class Compiler {
private:
	std::shared_ptr<Block> __mainBlock;
	struct FunctionCall {
		std::vector<std::string> varNameAccesible;
		std::shared_ptr<Block> blockWillRun;
		std::map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> parameters;
	};

	struct VariableLink {
		std::map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> variables;
		// std::map<std::string, std::shared_ptr<FunctionCall>> functionMap;
		std::shared_ptr<VariableLink> next;
		std::shared_ptr<VariableLink> prev;
	};
	std::shared_ptr<VariableLink> variable = std::make_shared<VariableLink>();

public:
	Compiler(const std::shared_ptr<Block> &mainBlock)
	 : __mainBlock(mainBlock) {
	 	this->compile(this->__mainBlock);
	}

	JDM_DLL const void compile(
		const std::shared_ptr<Block> &block,
		const std::map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_variables = {},
		const std::map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_AdditionalVariables = {},
		const std::map<std::string, std::shared_ptr<FunctionCall>> &_functionsAvail = {}) {

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
				this->addVariable(declare->expression, dataT, declare->varName->returnStringValue());

			} else if (instruction->getType() == AssignmentInstruction) {
				auto assign = std::dynamic_pointer_cast<Assignment>(instruction);
				auto var  = this->variable->variables.find(assign->varName->returnStringValue());

				if ( var != this->variable->variables.end())
					this->addVariable(assign->expression, var->second.first, assign->varName->returnStringValue());
				else throw std::runtime_error("Runtime Error: Variable is not declared.");

			} else if (instruction->getType() == LoggerInstruction) {
				auto logger = std::dynamic_pointer_cast<Logger>(instruction);
				for (const auto &expr : logger->expressions) {
					this->evaluateExpression(expr)->logValue();
					if (logger->addNewLine) Log << '\n';
				}

			} else if (instruction->getType() == IfStatementInstruction) {
				this->processIfStatement(instruction);
			} else if (instruction->getType() == ForLoopStatementInstruction) {
				this->processForStatement(instruction);
			} else if (instruction->getType() == CreateFunctionInstruction) {
				this->processCreateFunction(instruction);
			} else if (instruction->getType() == CallInstruction) {
				this->processCallFunction(instruction);
			}
		}

		if (this->variable->prev != nullptr) {
			this->variable = this->variable->prev;
			std::map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> tempMap;
			for (const auto& element : this->variable->variables)
    			tempMap[element.first] = this->variable->next->variables.at(element.first);

			this->variable->variables = tempMap;
			this->variable->next = nullptr;
		}
	}

	const std::vector<std::string> getAllVarName(
		const std::map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_variables = {}
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

		std::shared_ptr<FunctionCall> newFunction = std::make_shared<FunctionCall>();
		for (const auto &var : createFunc->parameters) {
			if (var->dataType != nullptr) {
				auto dataT   = JDM::dataTypeMap.at(std::get<0>(var->dataType->token));
				newFunction->parameters[var->varName->returnStringValue()] = this->returnVariable(var->varValue, dataT);
			} else {
				newFunction->parameters[var->varName->returnStringValue()] = this->returnVariable(var->varValue, DataTypeEnum::DATA_ANY);
			}
		}
		newFunction->blockWillRun = createFunc->blockWillRun;
		newFunction->varNameAccesible = this->getAllVarName(this->variable->variables);
		this->variable->functionMap[createFunc->functionName->returnStringValue()] = newFunction;
	}

	const void processCallFunction(
		const std::shared_ptr<Instruction> &instruction)
	{
		auto callFunc = std::dynamic_pointer_cast<Call>(instruction);
		auto callObjRoot = callFunc->callObj;
		while (callObjRoot->prevObject != nullptr) {
			callObjRoot = callObjRoot->prevObject;
		}
		this->recursivelyCall(callObjRoot);

	}

	const void recursivelyCall(const std::shared_ptr<CallObjects> &callObj) {
		auto tok = callObj->currObject->getToken();
		if (tok == TokenType::FUNCTIONS) {
			auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(callObj->currObject);

			auto function = this->variable->functionMap.find(functionObj->returnStringValue());
    		if (function != this->variable->functionMap.end()) {
    			if (functionObj->arguments.size() > function->second->parameters.size())
    				throw std::runtime_error("Runtime Error: Too many arguments.");

    			std::map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> scopedVariables;
				for (const auto& varName : function->second->varNameAccesible)
    				scopedVariables[varName] = this->variable->variables.at(varName);

    			int index = 0;
    			std::map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> newParam
    				= function->second->parameters;

    			for (const auto &param : function->second->parameters) {
    				if (index == functionObj->arguments.size()) break;
					auto var     = this->evaluateExpression(functionObj->arguments[index++]);
					if      (param.second.first == DataTypeEnum::DATA_INTEGER) var->castToInteger();
					else if (param.second.first == DataTypeEnum::DATA_STRING ) var->castToString ();
					else if (param.second.first == DataTypeEnum::DATA_BOOLEAN) var->castToBoolean();
					else if (param.second.first == DataTypeEnum::DATA_DOUBLE ) var->castToDecimal();
					newParam[param.first] = std::make_pair(param.second.first, var);
    			}

				this->compile(function->second->blockWillRun,
					scopedVariables, newParam, this->variable->functionMap);

    		} else {
        		throw std::runtime_error("Runtime Error: Function is not declared.");
    		}
			
		}
	}

	const void processIfStatement(
		const std::shared_ptr<Instruction> &instruction)
	{
		if (instruction == nullptr) return;

		auto ifState = std::dynamic_pointer_cast<IfStatement>(instruction);
		if (ifState->condition != nullptr) {
			auto condition = this->evaluateExpression(ifState->condition);
			condition->castToBoolean();

			if (!condition->booleanValue) {
				this->processIfStatement(ifState->elseIf);
				return;
			}
		}
		this->compile(ifState->blockWillRun, this->variable->variables, {}, this->variable->functionMap);
	}

	const void processForStatement(
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

		std::map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> additionalVariables;
		additionalVariables[forState->variable->returnStringValue()] = std::make_pair(DataTypeEnum::DATA_INTEGER, start);
		while (start->integerValue != stop->integerValue) {			
			this->compile(forState->blockWillRun, this->variable->variables, additionalVariables, this->variable->functionMap);
			start->integerValue += step->integerValue;
		}
	}

	const void addVariable(
		const std::shared_ptr<Expression> &expr,
		DataTypeEnum dataT,
		const std::string &varName) {

		auto var     = this->evaluateExpression(expr);
		if      (dataT == DataTypeEnum::DATA_INTEGER) var->castToInteger();
		else if (dataT == DataTypeEnum::DATA_STRING ) var->castToString ();
		else if (dataT == DataTypeEnum::DATA_BOOLEAN) var->castToBoolean();
		else if (dataT == DataTypeEnum::DATA_DOUBLE ) var->castToDecimal();
		this->variable->variables[varName] = std::make_pair(dataT, var);
	}

	const std::pair<DataTypeEnum, std::shared_ptr<HigherObject>> returnVariable(
		const std::shared_ptr<Expression> &expr,
		DataTypeEnum dataT) {

		auto var     = this->evaluateExpression(expr);
		if      (dataT == DataTypeEnum::DATA_INTEGER) var->castToInteger();
		else if (dataT == DataTypeEnum::DATA_STRING ) var->castToString ();
		else if (dataT == DataTypeEnum::DATA_BOOLEAN) var->castToBoolean();
		else if (dataT == DataTypeEnum::DATA_DOUBLE ) var->castToDecimal();
		return std::make_pair(dataT, var);
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
					return firstVal;
				}
				else if (operation == "*") {
					if (!secondVal->isInteger && !secondVal->isDecimal) throw std::runtime_error("Runtime Error: Must be a Number to multiply to String.");

					secondVal->castToInteger();
					if (secondVal->integerValue < 0) throw std::runtime_error("Runtime Error: Integer must be greater than 0.");

					std::string temporary = firstVal->stringValue;
					for (int i = 0; i < secondVal->integerValue; i++)
						firstVal->stringValue += temporary;
					return firstVal;

				} else throw std::runtime_error("Runtime Error: This operation is not allowed on String.");

			} else if (firstVal->isInteger || firstVal->isDecimal) {
				if (secondVal->isString) throw std::runtime_error("Runtime Error: Cannot do operation on Number and String.");
				firstVal ->castToDecimal();
				secondVal->castToDecimal();

				if      (operation == "+" ) firstVal->doubleValue += secondVal->doubleValue;
				else if (operation == "-" ) firstVal->doubleValue -= secondVal->doubleValue;
				else if (operation == "*" ) firstVal->doubleValue *= secondVal->doubleValue;
				else if (operation == "/" ) firstVal->doubleValue /= secondVal->doubleValue;
				else if (operation == "//") firstVal->doubleValue = static_cast<int64_t>(firstVal->doubleValue / secondVal->doubleValue);

				return firstVal;
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
    			auto var = this->variable->variables.find(std::dynamic_pointer_cast<VariableObjects>(Value)->returnStringValue());
    			if (var != this->variable->variables.end()) {
			        value = std::make_shared<HigherObject>(var->second.second);
    			} else {
        			throw std::runtime_error("Runtime Error: Variable is not declared.");
    			}
			} else if (type == TokenType::STRING) {
			    value = std::make_shared<HigherObject>(std::dynamic_pointer_cast<StringObjects >(Value)->returnValue());
			} else if (type == TokenType::INTEGER) {
			    value = std::make_shared<HigherObject>(std::dynamic_pointer_cast<IntegerObjects>(Value)->returnValue());
			} else if (type == TokenType::DECIMAL) {
			    value = std::make_shared<HigherObject>(std::dynamic_pointer_cast<DoubleObjects >(Value)->returnValue());
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
