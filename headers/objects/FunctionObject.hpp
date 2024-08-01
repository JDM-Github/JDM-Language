#pragma once

class FunctionObjects : public VarObjects
{
public:
	std::string name;
	std::vector<std::shared_ptr<Expression>> arguments;

public:
	inline FunctionObjects(
		const std::shared_ptr<TokenStruct>             &tok        = nullptr,
		const std::vector<std::shared_ptr<Expression>> &_arguments = {}
	)
	:
		name(std::get<0>(tok->token)),
		VarObjects(JDM::TokenType::FUNCTIONS, std::get<2>(tok->token), std::get<3>(tok->token)),
		arguments (_arguments)
	{}

	inline const std::string returnStringValue()
	{
		return this->name;
	}
};