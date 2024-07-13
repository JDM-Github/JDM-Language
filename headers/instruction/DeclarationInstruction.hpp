#pragma once

class Declaration : public Instruction
{
public:
	bool isConst = true;
	bool isForce = false;
	std::shared_ptr<TokenStruct    > dataType;
	std::shared_ptr<VariableObjects> varName;
	std::shared_ptr<Expression     > expression;

public:
	Declaration(
		const std::shared_ptr<TokenStruct    > &_dataType   = nullptr,
		const std::shared_ptr<VariableObjects> &_varName    = nullptr,
		const std::shared_ptr<Expression     > &_expression = nullptr,
		const bool _isConst = false, const bool _isForce = false
	)
	:
		dataType   (_dataType  ),
		varName    (_varName   ),
		expression (_expression),
		isConst    (_isConst   ),
		isForce    (_isForce   ),
		Instruction(InstructionType::DeclarationInstruction)
	{}
};
