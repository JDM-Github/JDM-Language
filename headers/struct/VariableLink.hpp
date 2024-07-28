#pragma once

struct VariableLink
{
	std::unordered_map<std::string, std::pair<DataTypeEnum, SharedHigherObject>> variables;
	std::unordered_map<std::string, std::shared_ptr<FunctionCall>> functionMap;
	std::shared_ptr<VariableLink> next;
	std::shared_ptr<VariableLink> prev;
};
