#pragma once

struct Variable
{
	std::shared_ptr<TokenStruct    > dataType;
	std::shared_ptr<VariableObjects> varName;
	std::shared_ptr<Expression     > varValue;

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(dataType, varName, varValue);
	}
};
