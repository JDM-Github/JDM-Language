#pragma once

class Logger : public Instruction
{
public:
	bool addNewLine = false;
	std::vector<std::shared_ptr<Expression>> expressions;
	Logger(
		const std::vector<std::shared_ptr<Expression>> &_expressions = {},
		const bool _addNewLine = false
	)
	:
		expressions(_expressions),
		addNewLine (_addNewLine ),
		Instruction(InstructionType::LoggerInstruction)
	{}
};
