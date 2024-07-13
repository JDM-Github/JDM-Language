#include "Compiler.hpp"

JDM_DLL
const std::vector<std::shared_ptr<HigherObject>> Compiler::getVectorHigherObject(
	const std::vector<std::shared_ptr<Expression>> &expr)
{
	// Evaluate all expression in vector
	std::vector<std::shared_ptr<HigherObject>> result;
	for (const auto &ex : expr)
	{
		auto exRes = this->evaluateExpression(ex);
		result.push_back(exRes);
	}
	return result;
}

JDM_DLL
const void Compiler::setupVariablePrevious()
{
	if (this->variable->prev != nullptr)
	{
		this->variable = this->variable->prev;
		std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> tempMap;
		for (const auto& element : this->variable->variables)
		{
			if (this->variable->next->variables.count(element.first))
				tempMap[element.first] = this->variable->next->variables.at(element.first);
			else
				tempMap[element.first] = element.second;
		}
		this->variable->variables = tempMap;
		this->variable->next = nullptr;
	}
}

JDM_DLL
const std::vector<std::string> Compiler::getAllVarName(
	const std::unordered_map<std::string,
	std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_variables)
{
	std::vector<std::string> keys;
	for (const auto& pair : _variables)
	   keys.push_back(pair.first);
	return keys;
}

JDM_DLL
const void Compiler::addVariable(
	const std::shared_ptr<Expression> &expr,
	DataTypeEnum dataT,
	const std::string &varName,
	bool isConst,
	bool isForce,
	const std::string &operation)
{
	this->isAssigning = true;
	auto var = this->evaluateExpression(expr);
	if (var->getIsReferenced())
	{
		if (operation != "=")
			throw std::runtime_error("Runtime Error: Referenced cannot be used in operation.");
	}
	if      (operation == "+=") var = this->newOperatedObject(this->variable->variables[varName].second, "+", var);
	else if (operation == "-=") var = this->newOperatedObject(this->variable->variables[varName].second, "-", var);
	else if (operation == "*=") var = this->newOperatedObject(this->variable->variables[varName].second, "*", var);
	else if (operation == "/=") var = this->newOperatedObject(this->variable->variables[varName].second, "/", var);
	else if (operation == "%=") var = this->newOperatedObject(this->variable->variables[varName].second, "%", var);

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
		var = this->checkVariableConstraint(var, dataT);
	}

	if (operation != "=") return;

	// Get the old Variable and just set it's new value to this
	auto oldVar  = this->variable->variables.find(varName);
	if (oldVar != this->variable->variables.end())
	{
		// Use the setHigherObject to set the value of variable to var
		if (oldVar->second.second->getIsReferenced() && var->getIsReferenced())
			throw std::runtime_error("Runtime Error: Variable " + varName + " is already a reference.");

		oldVar->second.second->setHigherObject(var);
		this->variable->variables[varName] = std::make_pair(dataT, oldVar->second.second);
		return;
	}
	if (var->isForcedConstraint && dataT == DataTypeEnum::DATA_ANY)
		this->variable->variables[varName] = std::make_pair(var->getDatatypeEnum(), var);
	else
		this->variable->variables[varName] = std::make_pair(dataT, var);
}

JDM_DLL
const std::pair<DataTypeEnum, std::shared_ptr<HigherObject>> Compiler::returnVariable(
	const std::shared_ptr<Expression> &expr,
	DataTypeEnum dataT)
{
	auto var = this->castVariable(expr, dataT);
	return std::make_pair(dataT, var);
}

JDM_DLL
const std::shared_ptr<HigherObject> Compiler::castVariable(
	const std::shared_ptr<Expression> &expr,
	DataTypeEnum dataT,
	bool checkConstraint)
{
	auto var = this->evaluateExpression(expr);
	if (var->getIsReferenced())
		throw std::runtime_error("Runtime Error: Cannot cast when declaring referenced.");

	return this->checkVariableConstraint(var, dataT, checkConstraint);
}

JDM_DLL
const std::shared_ptr<HigherObject> Compiler::checkVariableConstraint(
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
