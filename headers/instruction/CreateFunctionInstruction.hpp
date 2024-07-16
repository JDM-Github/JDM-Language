#pragma once

class CreateFunction : public Instruction
{
public:
	std::shared_ptr<Block                > blockWillRun;
	std::shared_ptr<VariableObjects      > functionName;
	std::vector<std::shared_ptr<Variable>> parameters;

public:
	template<class Archive>
	inline void serialize(Archive & archive)
	{
		archive(cereal::base_class<Instruction>(this));
		archive(blockWillRun, functionName, parameters);
	}

	inline CreateFunction(
		const std::shared_ptr<Block                > &_blockWillRun = nullptr,
		const std::shared_ptr<VariableObjects      > &_functionName = nullptr,
		const std::vector<std::shared_ptr<Variable>> &_parameters   = {}
	)
	:
		blockWillRun (_blockWillRun),
		functionName (_functionName),
		parameters   (_parameters),
		Instruction  (InstructionType::CreateFunctionInstruction)
	{}
};
