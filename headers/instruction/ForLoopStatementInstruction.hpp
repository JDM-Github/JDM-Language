#pragma once

class ForLoopStatement : public Instruction
{
public:
	std::shared_ptr<Block          > blockWillRun;
	std::shared_ptr<VariableObjects> variable;
	std::shared_ptr<Expression     > start; // Must be a INTEGER
	std::shared_ptr<Expression     > stop;  // Must be a INTEGER
	std::shared_ptr<Expression     > step;  // Must be a INTEGER

public:
	template<class Archive>
	inline void serialize(Archive & archive)
	{
		archive(cereal::base_class<Instruction>(this));
		archive(blockWillRun, variable, start, stop, step);
	}

	inline ForLoopStatement(
		const std::shared_ptr<Block          > &_blockWillRun = nullptr,
		const std::shared_ptr<VariableObjects> &_variable     = nullptr,
		const std::shared_ptr<Expression     > &_start        = nullptr,
		const std::shared_ptr<Expression     > &_stop         = nullptr,
		const std::shared_ptr<Expression     > &_step         = nullptr
	)
	:
		blockWillRun(_blockWillRun),
		variable    (_variable    ),
		start       (_start       ),
		stop        (_stop        ),
		step        (_step        ),
		Instruction(InstructionType::ForLoopStatementInstruction)
	{}
};
