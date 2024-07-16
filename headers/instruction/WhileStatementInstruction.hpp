#pragma once

class WhileStatement : public Instruction
{
public:
	std::shared_ptr<Block     > blockWillRun;
	std::shared_ptr<Expression> condition;

public:
	template<class Archive>
	inline void serialize(Archive & archive)
	{
		archive(cereal::base_class<Instruction>(this));
		archive(blockWillRun, condition);
	}

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
