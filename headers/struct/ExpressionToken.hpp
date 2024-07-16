#pragma once

struct ExpressionToken
{
	std::shared_ptr<Expression> expression;
	std::shared_ptr<TokenStruct> token;
	ExpressionToken(
		const std::shared_ptr<Expression> &exp = nullptr,
		const std::shared_ptr<TokenStruct> &tok = nullptr)
	:
		expression(exp), token(tok)
	{}

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(expression, token);
	}
};
