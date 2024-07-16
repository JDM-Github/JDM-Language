#pragma once

class CFunction : public Instruction
{
public:
	CustomFunctionEnum funcType;
	std::shared_ptr<Expression> expression;

public:
	template<class Archive>
	inline void serialize(Archive & archive)
	{
		archive(cereal::base_class<Instruction>(this));
		archive(funcType, expression);
	}

	CFunction() = default;
	inline explicit CFunction(
		CustomFunctionEnum type,
		const std::shared_ptr<Expression> &expr = nullptr
	)
	:
		funcType(type),
		expression(expr),
		Instruction(InstructionType::CFunctionInstruction)
	{}
};
