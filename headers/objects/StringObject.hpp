#pragma once

class StringObjects : public VarObjects
{
public:
	std::string value;

public:
	StringObjects() = default;
	inline explicit StringObjects(const std::shared_ptr<TokenStruct> &tok)
	:
		VarObjects(JDM::TokenType::STRING, std::get<2>(tok->token),
		std::get<3>(tok->token)),
		value(std::get<0>(tok->token))
	{}

	inline const std::string returnStringValue()
	{
		return this->value;
	}

	inline const std::string returnValue()
	{
		return this->value;
	}
};
