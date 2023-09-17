#pragma once
#include "defines.hpp"

enum InstructionType {
	DefaultInstruction,
	DeclarationInstruction,
	AssignmentInstruction,
	BlockInstruction,
	IfStatementInstruction,
	CreateFunctionInstruction,
	CallFunctionInstruction,
	ForLoopStatementInstruction,
	WhileStatementInstruction,
	ForEachListStatementInstruction,
	ForEachMapStatementInstruction,
};

class Instruction {
protected:
	InstructionType _instructType;
	Instruction(InstructionType type) : _instructType(type) {}
	virtual ~Instruction() {}

public:
	inline const InstructionType getType() {
		return this->_instructType;
	}
};

class VarObjects {
protected:
	size_t row, col;
	TokenType varType = TokenType::UNDEFINED;

protected:
	VarObjects(TokenType type, size_t r = 0, size_t c = 0)
		: varType(type), row(r), col(c) {}
	virtual ~VarObjects() {}

public:
	inline const char* getStringToken() {
		return JDM::tokenTypeToString(this->varType);
	}
	inline const TokenType getToken() {
		return this->varType;
	}
};

class VariableObjects : public VarObjects {
public:
	const char      *name;

public:
	VariableObjects(const std::shared_ptr<TokenStruct> &tok)
		: VarObjects(TokenType::VARIABLE, std::get<2>(tok->token), std::get<3>(tok->token)),
		name(std::get<0>(tok->token).c_str()) {
	}
	inline const char *returnStringValue() { return this->name; }
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
