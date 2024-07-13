#pragma once

class ClassObjects : public VarObjects
{
public:
	std::string name;

public:
	inline ClassObjects(const std::shared_ptr<TokenStruct> &tok)
	:
		VarObjects(JDM::TokenType::CLASS, std::get<2>(tok->token),
		std::get<3>(tok->token)),
		name(std::get<0>(tok->token))
	{}

	inline const std::string returnStringValue()
	{
		return this->name;
	}
};
