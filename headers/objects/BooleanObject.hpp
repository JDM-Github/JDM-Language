#pragma once

class BooleanObjects : public VarObjects
{
public:
	bool value;

public:
	BooleanObjects(const std::shared_ptr<TokenStruct> &tok)
	:
		VarObjects(JDM::TokenType::BOOLED, std::get<2>(tok->token),
		std::get<3>(tok->token))
	{
		auto tokenValue = std::get<0>(tok->token);
		value = tokenValue == "jtrue";
	}

	inline const std::string returnStringValue()
	{
		return std::to_string(this->value);
	}

	inline const bool returnValue()
	{
		return this->value;
	}
};
