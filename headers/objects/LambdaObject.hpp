#pragma once

class LambdaObjects : public VarObjects
{
public:
	bool willCall = false;
	std::vector<std::shared_ptr<Expression>> arguments;
	std::vector<std::shared_ptr<Variable  >> parameters;
	std::shared_ptr<Instruction            > blockWillRun;

public:
	inline LambdaObjects(
		bool _willCall = false,
		const std::vector<std::shared_ptr<Expression>> &_arguments    = {},
		const std::vector<std::shared_ptr<Variable  >> &_parameters   = {},
		const std::shared_ptr<Instruction            > &_blockWillRun = nullptr
	)
	:
		VarObjects(JDM::TokenType::LAMBDA),
		willCall     (_willCall    ),
		arguments    (_arguments   ),
		parameters   (_parameters  ),
		blockWillRun (_blockWillRun)
	{}

	inline const std::string returnStringValue()
	{
		return "LAMBDA";
	}
};
