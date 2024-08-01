#pragma once

class Call : public Instruction
{
public:
	bool isAssigning = false;
	std::string operation = "";
	std::shared_ptr<CallObjects> callObj;
	std::shared_ptr<Expression > expression;

public:
	inline Call(
		bool _isAssigning = false,
		const std::shared_ptr<CallObjects> &_callObj    = nullptr,
		const std::shared_ptr<Expression > &_expression = nullptr,
		const std::string &_operation = ""
	)
	:
		isAssigning(_isAssigning),
		callObj    (_callObj),
		expression (_expression),
		operation  (_operation),
		Instruction(InstructionType::CallInstruction)
	{}
};
