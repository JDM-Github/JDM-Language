#pragma once

class ForEachStatement : public Instruction
{
public:
	bool isReverse = false;
	std::shared_ptr<Block          > blockWillRun;
	std::shared_ptr<VariableObjects> varKey;
	std::shared_ptr<VariableObjects> varVal;
	std::shared_ptr<VariableObjects> varToRun;
	std::shared_ptr<Expression     > expression;

public:
	ForEachStatement(
		const std::shared_ptr<Block          > &_blockWillRun = nullptr,
		const std::shared_ptr<VariableObjects> &_varKey       = nullptr,
		const std::shared_ptr<VariableObjects> &_varVal       = nullptr,
		const std::shared_ptr<Expression     > &_expression   = nullptr,
		const bool _reverse = false
	)
	:
		blockWillRun(_blockWillRun),
		varKey      (_varKey      ),
		varVal      (_varVal      ),
		expression  (_expression  ),
		isReverse   (_reverse     ),
		Instruction(InstructionType::ForEachStatementInstruction)
	{}

	ForEachStatement(
		const std::shared_ptr<Block          > &_blockWillRun = nullptr,
		const std::shared_ptr<VariableObjects> &_varKey       = nullptr,
		const std::shared_ptr<VariableObjects> &_varVal       = nullptr,
		const std::shared_ptr<VariableObjects> &_varToRun     = nullptr,
		const bool _reverse = false
	)
	:
		blockWillRun(_blockWillRun),
		varKey      (_varKey      ),
		varVal      (_varVal      ),
		varToRun    (_varToRun    ),
		isReverse   (_reverse     ),
		Instruction(InstructionType::ForEachStatementInstruction)
	{}
};
