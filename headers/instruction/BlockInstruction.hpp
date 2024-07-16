#pragma once

class Block : public Instruction
{
public:
	std::vector<std::shared_ptr<Instruction>> instruction;

public:
	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(cereal::base_class<Instruction>(this), instruction);
	}

	inline Block()
	:
		Instruction(InstructionType::BlockInstruction)
	{}
};
