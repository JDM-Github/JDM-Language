 #include "compiler.hpp"

JDM_DLL
const std::shared_ptr<HigherObject> Compiler::runFunction(
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