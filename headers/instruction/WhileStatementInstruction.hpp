#pragma once

class WhileStatement : public Instruction
{
public:
	std::shared_ptr<Block     > blockWillRun;
	std::shared_ptr<Expression> condition;

public:
	inline WhileStatement(
		const std::shared_ptr<Block     > &_blockWillRun = nullptr,
		const std::shared_ptr<Expression> &_condition    = nullptr
	)
	:
		blockWillRun(_blockWillRun),
		condition   (_condition   ),
		Instruction (InstructionType::WhileStatementInstruction)
	{}
};
