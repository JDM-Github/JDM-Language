#pragma once

class Return : public Instruction
{
public:
	std::shared_ptr<Expression > returnValue;

public:
	template<class Archive>
	inline void serialize(Archive & archive)
	{
		archive(cereal::base_class<Instruction>(this));
		archive(returnValue);
	}

	inline Return(const std::shared_ptr<Expression> &_returnValue = nullptr)
	:
		returnValue(_returnValue),
		Instruction(InstructionType::ReturnInstruction)
	{}
};
