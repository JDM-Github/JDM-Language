#pragma once
#include "enums/InstructionType.hpp"

class Instruction
{
public:
	Instruction() = default;

	InstructionType _instructType;
	explicit Instruction(InstructionType type)
	:
		_instructType(type)
	{}
	virtual ~Instruction() {}

public:
	template<class Archive>
	inline void serialize(Archive & archive)
	{
		archive(_instructType);
	}

	inline const InstructionType getType()
	{
		return this->_instructType;
	}
};
