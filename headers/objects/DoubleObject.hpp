#pragma once

class DoubleObjects : public VarObjects
{
public:
	long double value;

public:
	DoubleObjects() = default;
	inline explicit DoubleObjects(const std::shared_ptr<TokenStruct> &tok)
	:
		VarObjects(JDM::TokenType::DOUBLE, std::get<2>(tok->token),
		std::get<3>(tok->token))
	{
		this->value = std::stod(std::get<0>(tok->token));
	}

	inline const std::string returnStringValue()
	{
		return std::to_string(this->value);
	}

	inline const long double returnValue()
	{
		return this->value;
	}
};
