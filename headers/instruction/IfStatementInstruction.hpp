#pragma once

class IfStatement : public Instruction
{
public:
	std::shared_ptr<Block      > blockWillRun;
	std::shared_ptr<Expression > condition;
	std::shared_ptr<IfStatement> elseIf;

public:
	inline IfStatement(
		const std::shared_ptr<Block     > &_blockWillRun = nullptr,
		const std::shared_ptr<Expression> &_condition    = nullptr
	)
	:
		blockWillRun(_blockWillRun),
		condition   (_condition   ),
		Instruction (InstructionType::IfStatementInstruction)
	{}
};
