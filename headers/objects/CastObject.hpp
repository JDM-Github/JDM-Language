#pragma once

class CastObjects : public VarObjects
{
public:
	DataTypeEnum datTypeToTurn;
	std::shared_ptr<Expression> expression;

public:
	template<class Archive>
	inline void serialize(Archive & archive)
	{
		archive(cereal::base_class<VarObjects>(this));
		archive(datTypeToTurn, expression);
	}

	CastObjects() = default;
	inline explicit CastObjects(
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