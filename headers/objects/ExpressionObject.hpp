#pragma once

class ExpressionObjects : public VarObjects
{
public:
	std::string name;
	std::shared_ptr<Expression> expression;

public:
	ExpressionObjects(
		const std::shared_ptr<TokenStruct> &tok  = nullptr,
		const std::shared_ptr<Expression > &expr = nullptr
	)
	:
		name(std::get<0>(tok->token)),
		expression(expr),
		VarObjects(JDM::TokenType::EXPRESSION, std::get<2>(tok->token), std::get<3>(tok->token))
	{}

	inline const std::string returnStringValue()
	{
		return this->name;
	}
};
