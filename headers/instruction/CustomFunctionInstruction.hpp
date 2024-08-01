#pragma once

class CFunction : public Instruction
{
public:
	CustomFunctionEnum funcType;
	std::shared_ptr<Expression> expression;

public:

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
