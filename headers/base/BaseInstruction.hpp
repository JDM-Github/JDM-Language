#pragma once
#include "enums/InstructionType.hpp"

class Instruction
{
protected:
	InstructionType _instructType;
	Instruction(InstructionType type)
		: _instructType(type) {}
	virtual ~Instruction() {}

public:
	inline const InstructionType getType()
	{
		return this->_instructType;
	}
};
