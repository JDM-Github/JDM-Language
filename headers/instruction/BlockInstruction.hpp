#pragma once

class Block : public Instruction
{
public:
	std::vector<std::shared_ptr<Instruction>> instruction;

public:
	inline Block()
	:
		Instruction(InstructionType::BlockInstruction)
	{}
};
