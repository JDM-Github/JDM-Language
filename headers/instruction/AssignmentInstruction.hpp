#pragma once

class Assignment : public Instruction
{
public:
	std::shared_ptr<VariableObjects> varName;
	std::shared_ptr<TokenStruct    > operation;
	std::shared_ptr<Expression     > expression;

public:
	inline Assignment(
		const std::shared_ptr<VariableObjects> &_varName    = nullptr,
		const std::shared_ptr<Expression     > &_expression = nullptr
	)
	:
		varName    (_varName   ),
		expression (_expression),
		Instruction(InstructionType::AssignmentInstruction)
	{}

	inline Assignment(
		const std::shared_ptr<VariableObjects> &_varName    = nullptr,
		const std::shared_ptr<TokenStruct    > &_operation  = nullptr,
		const std::shared_ptr<Expression     > &_expression = nullptr
	)
	:
		varName    (_varName   ),
		operation  (_operation ),
		expression (_expression),
		Instruction(InstructionType::AssignmentInstruction)
	{}
};