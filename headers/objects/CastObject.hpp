#pragma once

class CastObjects : public VarObjects
{
public:
	DataTypeEnum datTypeToTurn;
	std::shared_ptr<Expression> expression;

public:
	inline CastObjects(
		DataTypeEnum _datTypeToTurn,
		const std::shared_ptr<Expression > &_expression = nullptr
	)
	:
		VarObjects   (JDM::TokenType::CAST),
		datTypeToTurn(_datTypeToTurn ),
		expression   (_expression    )
	{}

	inline const std::string returnStringValue()
	{
		return "CAST";
	}
};