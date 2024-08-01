#pragma once

class Return : public Instruction
{
public:
	std::shared_ptr<Expression > returnValue;

public:
	inline Return(const std::shared_ptr<Expression> &_returnValue = nullptr)
	:
		returnValue(_returnValue),
		Instruction(InstructionType::ReturnInstruction)
	{}
};
