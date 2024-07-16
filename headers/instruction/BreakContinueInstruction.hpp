#pragma once

class Break : public Instruction
{
public:
	template<class Archive>
	inline void serialize(Archive & archive)
	{
		archive(cereal::base_class<Instruction>(this));
	}

	inline Break()
	:
		Instruction(InstructionType::BreakInstruction)
	{}
};

class Continue : public Instruction
{
public:
	template<class Archive>
	inline void serialize(Archive & archive)
	{
		archive(cereal::base_class<Instruction>(this));
	}

	inline Continue()
	:
		Instruction(InstructionType::ContinueInstruction)
	{}
};