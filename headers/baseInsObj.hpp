#pragma once
#include "defines.hpp"

enum InstructionType {
	DefaultInstruction,
	LoggerInstruction,
	DeclarationInstruction,
	AssignmentInstruction,
	BlockInstruction,
	IfStatementInstruction,
	CreateFunctionInstruction,
	CallInstruction,
	ForLoopStatementInstruction,
	WhileStatementInstruction,
	ForEachListStatementInstruction,
	ForEachMapStatementInstruction,
	ReturnInstruction,
	CFunctionInstruction
};

class Instruction {
protected:
	InstructionType _instructType;
	Instruction(InstructionType type) : _instructType(type) {}
	virtual ~Instruction() {}

public:
	inline const InstructionType getType() { return this->_instructType; }
};

class VarObjects {
protected:
	size_t row, col;
	JDM::TokenType varType = JDM::TokenType::UNDEFINED;

protected:
	VarObjects(JDM::TokenType type, size_t r = 0, size_t c = 0) : varType(type), row(r), col(c) {}
	virtual ~VarObjects() {}

public:
	inline const char* getStringToken() { return JDM::tokenTypeToString(this->varType); }
	inline const void setToken(JDM::TokenType varType) { this->varType = varType; }
	inline const JDM::TokenType getToken() { return this->varType; }
	virtual const std::string returnStringValue() { return "INVALID"; };
};

class VariableObjects : public VarObjects {
public:
	std::string name;

public:
	VariableObjects(const std::shared_ptr<TokenStruct> &tok)
		: VarObjects(JDM::TokenType::VARIABLE, std::get<2>(tok->token), std::get<3>(tok->token)), name(std::get<0>(tok->token)) {}
	inline const std::string returnStringValue() { return this->name; }
};

struct Expression {
	std::shared_ptr<VarObjects > firstValue;
	std::shared_ptr<VarObjects > secondValue;
	std::shared_ptr<Expression > firstExpression;
	std::shared_ptr<TokenStruct> opWillUse;
	std::shared_ptr<Expression > secondExpression;
};

struct MapStruct {
	std::shared_ptr<Expression> key;
	std::shared_ptr<Expression> value;
};

struct Variable {
	std::shared_ptr<TokenStruct    > dataType;
	std::shared_ptr<VariableObjects> varName;
	std::shared_ptr<Expression     > varValue;
};

struct ExpressionToken {
	std::shared_ptr<Expression> expression;
	std::shared_ptr<TokenStruct> token;
	ExpressionToken(
		const std::shared_ptr<Expression> &exp = nullptr,
		const std::shared_ptr<TokenStruct> &tok = nullptr)
	: expression(exp), token(tok) {}
};
