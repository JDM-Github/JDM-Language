#pragma once

class IntegerObjects : public VarObjects
{
public:
	int64_t value;

public:

	IntegerObjects() = default;
	inline explicit IntegerObjects(const std::shared_ptr<TokenStruct> &tok)
	:
		VarObjects(JDM::TokenType::INTEGER, std::get<2>(tok->token),
		std::get<3>(tok->token))
	{
		auto tokenValue = std::get<0>(tok->token);
		if (tokenValue == "jtrue" || tokenValue == "jfalse")
    		 this->value = static_cast<int64_t>(tokenValue == "jtrue");
		else
			this->value = std::stoll(tokenValue);
	}

	inline const std::string returnStringValue()
	{
		return std::to_string(this->value);
	}

	inline const int64_t returnValue()
	{
		return this->value;
	}
};
