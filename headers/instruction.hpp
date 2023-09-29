#pragma once
#include "objects.hpp"

// DONE
class Declaration : public Instruction {
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
	) :
	dataType   (_dataType  ),
	varName    (_varName   ),
	expression (_expression),
	isConst    (_isConst   ),
	isForce    (_isForce   ),
	Instruction(InstructionType::DeclarationInstruction) {}
};

// DONE
class Assignment : public Instruction {
public:
	std::shared_ptr<VariableObjects> varName;
	std::shared_ptr<TokenStruct    > operation;
	std::shared_ptr<Expression     > expression;

public:
	Assignment(
		const std::shared_ptr<VariableObjects> &_varName    = nullptr,
		const std::shared_ptr<Expression     > &_expression = nullptr
	) :
	varName    (_varName   ),
	expression (_expression),
	Instruction(InstructionType::AssignmentInstruction) {}

	Assignment(
		const std::shared_ptr<VariableObjects> &_varName    = nullptr,
		const std::shared_ptr<TokenStruct    > &_operation  = nullptr,
		const std::shared_ptr<Expression     > &_expression = nullptr
	) :
	varName    (_varName   ),
	operation  (_operation ),
	expression (_expression),
	Instruction(InstructionType::AssignmentInstruction) {}
};

// DONE
class Logger : public Instruction {
public:
	bool addNewLine = false;
	std::vector<std::shared_ptr<Expression>> expressions;
	Logger(
		const std::vector<std::shared_ptr<Expression>> &_expressions = {},
		const bool _addNewLine = false
	) :
	expressions(_expressions),
	addNewLine (_addNewLine ),
	Instruction(InstructionType::LoggerInstruction) {}
};

// DONE
class CFunction : public Instruction {
public:
	CustomFunctionEnum funcType;
	std::shared_ptr<Expression> expression;
	CFunction(
		CustomFunctionEnum type,
		const std::shared_ptr<Expression> &expr = nullptr
	) :
	funcType(type),
	expression(expr),
	Instruction(InstructionType::CFunctionInstruction) {}
};

class Block : public Instruction {
public:
	std::vector<std::shared_ptr<Instruction>> instruction;

public:
	Block() : Instruction(InstructionType::BlockInstruction) { }
};

// DONE
class IfStatement : public Instruction {
public:
	std::shared_ptr<Block      > blockWillRun;
	std::shared_ptr<Expression > condition;
	std::shared_ptr<IfStatement> elseIf;

public:
	IfStatement(
		const std::shared_ptr<Block     > &_blockWillRun = nullptr,
		const std::shared_ptr<Expression> &_condition    = nullptr
	) :
	blockWillRun(_blockWillRun),
	condition   (_condition   ),
	Instruction (InstructionType::IfStatementInstruction) {}
};

class Return : public Instruction {
public:
	std::shared_ptr<Expression > returnValue;
	Return(const std::shared_ptr<Expression> &_returnValue = nullptr) :
	returnValue(_returnValue),
	Instruction(InstructionType::ReturnInstruction) {}
};

class CreateFunction : public Instruction {
public:
	std::shared_ptr<Block                > blockWillRun;
	std::shared_ptr<VariableObjects      > functionName;
	std::vector<std::shared_ptr<Variable>> parameters;

public:
	CreateFunction(
		const std::shared_ptr<Block                > &_blockWillRun = nullptr,
		const std::shared_ptr<VariableObjects      > &_functionName = nullptr,
		const std::vector<std::shared_ptr<Variable>> &_parameters   = {}
	) :
	blockWillRun (_blockWillRun),
	functionName (_functionName),
	parameters   (_parameters),
	Instruction  (InstructionType::CreateFunctionInstruction) {}
};

class Call : public Instruction {
public:
	bool isAssigning = false;
	std::string operation;
	std::shared_ptr<CallObjects> callObj;
	std::shared_ptr<Expression > expression;

public:
	Call(
		bool _isAssigning = false,
		const std::shared_ptr<CallObjects> &_callObj    = nullptr,
		const std::shared_ptr<Expression > &_expression = nullptr,
		const std::string &_operation = ""
	) :
	isAssigning(_isAssigning),
	callObj    (_callObj),
	expression (_expression),
	operation  (_operation),
	Instruction(InstructionType::CallInstruction) { this->callObj->operation = operation; }
};

// DONE
class ForLoopStatement : public Instruction {
public:
	std::shared_ptr<Block          > blockWillRun;
	std::shared_ptr<VariableObjects> variable;
	std::shared_ptr<Expression     > start; // Must be a INTEGER
	std::shared_ptr<Expression     > stop;  // Must be a INTEGER
	std::shared_ptr<Expression     > step;  // Must be a INTEGER

public:
	ForLoopStatement(
		const std::shared_ptr<Block          > &_blockWillRun = nullptr,
		const std::shared_ptr<VariableObjects> &_variable     = nullptr,
		const std::shared_ptr<Expression     > &_start        = nullptr,
		const std::shared_ptr<Expression     > &_stop         = nullptr,
		const std::shared_ptr<Expression     > &_step         = nullptr
	) :
	blockWillRun(_blockWillRun),
	variable    (_variable    ),
	start       (_start       ),
	stop        (_stop        ),
	step        (_step        ),
	Instruction(InstructionType::ForLoopStatementInstruction) {}
};

class WhileStatement : public Instruction {
public:
	std::shared_ptr<Block     > blockWillRun;
	std::shared_ptr<Expression> condition;

public:
	WhileStatement(
		const std::shared_ptr<Block     > &_blockWillRun = nullptr,
		const std::shared_ptr<Expression> &_condition    = nullptr
	) :
	blockWillRun(_blockWillRun),
	condition   (_condition   ),
	Instruction (InstructionType::WhileStatementInstruction) {}
};

class ForEachListStatement : public Instruction {
public:
	bool isReverse = false;
	std::shared_ptr<Block          > blockWillRun;
	std::shared_ptr<VariableObjects> variable;
	std::shared_ptr<ListObject     > listToRun;
	std::shared_ptr<VariableObjects> varToRun;

public:
	ForEachListStatement(
		const std::shared_ptr<Block          > &_blockWillRun = nullptr,
		const std::shared_ptr<VariableObjects> &_variable     = nullptr,
		const std::shared_ptr<ListObject     > &_listToRun    = nullptr,
		const bool _reverse = false
	) :
	blockWillRun(_blockWillRun),
	variable    (_variable    ),
	listToRun   (_listToRun   ),
	isReverse   (_reverse     ),
	Instruction(InstructionType::ForEachListStatementInstruction) {}

	ForEachListStatement(
		const std::shared_ptr<Block          > &_blockWillRun = nullptr,
		const std::shared_ptr<VariableObjects> &_variable     = nullptr,
		const std::shared_ptr<VariableObjects> &_varToRun     = nullptr,
		const bool _reverse = false
	) :
	blockWillRun(_blockWillRun),
	variable    (_variable    ),
	varToRun    (_varToRun    ),
	isReverse   (_reverse     ),
	Instruction(InstructionType::ForEachListStatementInstruction) {}
};

class ForEachMapStatement : public Instruction {
public:
	bool isReverse = false;
	std::shared_ptr<Block          > blockWillRun;
	std::shared_ptr<VariableObjects> varKey;
	std::shared_ptr<VariableObjects> varVal;
	std::shared_ptr<MapObject      > mapToRun;
	std::shared_ptr<VariableObjects> varToRun;

public:
	ForEachMapStatement(
		const std::shared_ptr<Block          > &_blockWillRun = nullptr,
		const std::shared_ptr<VariableObjects> &_varKey       = nullptr,
		const std::shared_ptr<VariableObjects> &_varVal       = nullptr,
		const std::shared_ptr<MapObject      > &_mapToRun     = nullptr,
		const bool _reverse = false
	) :
	blockWillRun(_blockWillRun),
	varKey      (_varKey      ),
	varVal      (_varVal      ),
	mapToRun    (_mapToRun    ),
	isReverse   (_reverse     ),
	Instruction(InstructionType::ForEachMapStatementInstruction) {}

	ForEachMapStatement(
		const std::shared_ptr<Block          > &_blockWillRun = nullptr,
		const std::shared_ptr<VariableObjects> &_varKey       = nullptr,
		const std::shared_ptr<VariableObjects> &_varVal       = nullptr,
		const std::shared_ptr<VariableObjects> &_varToRun     = nullptr,
		const bool _reverse = false
	) :
	blockWillRun(_blockWillRun),
	varKey      (_varKey      ),
	varVal      (_varVal      ),
	varToRun    (_varToRun    ),
	isReverse   (_reverse     ),
	Instruction(InstructionType::ForEachMapStatementInstruction) {}
};