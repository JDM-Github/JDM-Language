#pragma once

class Break : public Instruction
{
public:
	Break()
	:
		Instruction(InstructionType::BreakInstruction)
	{}
};

class Continue : public Instruction
{
public:
	Continue()
	:
		Instruction(InstructionType::ContinueInstruction)
	{}
};