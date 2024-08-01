#pragma once

class Break : public Instruction
{
public:

	inline Break()
	:
		Instruction(InstructionType::BreakInstruction)
	{}
};

class Continue : public Instruction
{
public:
	inline Continue()
	:
		Instruction(InstructionType::ContinueInstruction)
	{}
};