#pragma once

class VariableObjects : public VarObjects
{
public:
	std::string name;

public:
	inline VariableObjects(const std::shared_ptr<TokenStruct> &tok)
	:
		VarObjects(JDM::TokenType::VARIABLE, std::get<2>(tok->token),
		std::get<3>(tok->token)),
		name(std::get<0>(tok->token))
	{}
	inline const std::string returnStringValue() { return this->name; }
};
