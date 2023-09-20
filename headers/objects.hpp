#pragma once
#include "baseInsObj.hpp"

class ListObject : public VarObjects {
public:
	std::vector<std::shared_ptr<Expression>> list;

public:
	ListObject(const std::vector<std::shared_ptr<Expression>> &_list)
	: list (_list), VarObjects(TokenType::LIST) { }

	inline const std::string returnStringValue() { return "LIST"; }
};

class MapObject : public VarObjects {
public:
	std::vector<std::shared_ptr<MapStruct>> map;

public:
	MapObject(const std::vector<std::shared_ptr<MapStruct>> &_map)
	: map (_map), VarObjects(TokenType::MAP) { }
	inline const std::string returnStringValue() { return "MAP"; }
};


class LambdaObjects : public VarObjects {
public:
	bool willCall = false;
	std::vector<std::shared_ptr<Expression>> arguments;
	std::vector<std::shared_ptr<Variable  >> parameters;
	std::shared_ptr<Instruction            > blockWillRun;

public:
	LambdaObjects(
		bool _willCall = false,
		const std::vector<std::shared_ptr<Expression>> &_arguments    = {},
		const std::vector<std::shared_ptr<Variable  >> &_parameters   = {},
		const std::shared_ptr<Instruction            > &_blockWillRun = nullptr
	) :
	VarObjects(TokenType::LAMBDA),
	willCall     (_willCall    ),
	arguments    (_arguments   ),
	parameters   (_parameters  ),
	blockWillRun (_blockWillRun) {}

	inline const std::string returnStringValue() { return "LAMBDA"; }
};

class ClassObjects : public VarObjects {
public:
	std::string name;

public:
	ClassObjects(const std::shared_ptr<TokenStruct> &tok)
		: VarObjects(TokenType::CLASS, std::get<2>(tok->token), std::get<3>(tok->token)),
		name(std::get<0>(tok->token)) {
	}
	inline const std::string returnStringValue() { return this->name; }
};

class FunctionObjects : public VarObjects {
public:
	std::string name;
	std::vector<std::shared_ptr<Expression>> arguments;

public:
	FunctionObjects(
		const std::shared_ptr<TokenStruct>             &tok        = nullptr,
		const std::vector<std::shared_ptr<Expression>> &_arguments = {}
		) :
		name(std::get<0>(tok->token)),
		VarObjects(TokenType::FUNCTIONS, std::get<2>(tok->token), std::get<3>(tok->token)),
		arguments (_arguments) {}

	inline const std::string returnStringValue() { return this->name; }
};

class CallObjects : public VarObjects {
public:
	std::shared_ptr<VarObjects > currObject;
	std::shared_ptr<CallObjects> prevObject;

public:
	CallObjects(
		const std::shared_ptr<VarObjects > &_currObject = nullptr,
		const std::shared_ptr<CallObjects> &_prevObject = nullptr
		) :
		VarObjects(TokenType::CALL_OBJ),
		currObject(_currObject),
		prevObject(_prevObject) {}

	inline const std::string returnStringValue() { return "CALL"; }
};

class CastObjects : public VarObjects {
public:
	DataTypeEnum datTypeToTurn;
	std::shared_ptr<Expression > expression;

public:
	CastObjects(
		DataTypeEnum _datTypeToTurn,
		const std::shared_ptr<Expression > &_expression = nullptr
		) :
		VarObjects(TokenType::CAST),
		datTypeToTurn(_datTypeToTurn),
		expression   (_expression) {}

	inline const std::string returnStringValue() { return "CAST"; }
};

class StringObjects : public VarObjects {
public:
	std::string value;
public:
	StringObjects(const std::shared_ptr<TokenStruct> &tok)
		: VarObjects(TokenType::STRING, std::get<2>(tok->token), std::get<3>(tok->token)) {
		this->value = std::get<0>(tok->token);
	}
	inline const std::string returnStringValue() { return this->value; }
};

class IntegerObjects : public VarObjects {
public:
	int64_t value;
public:
	IntegerObjects(const std::shared_ptr<TokenStruct> &tok)
		: VarObjects(TokenType::INTEGER, std::get<2>(tok->token), std::get<3>(tok->token)) {
		this->value = std::stoll(std::get<0>(tok->token));
	}
	inline const std::string returnStringValue() { return std::to_string(this->value); }
};

class DoubleObjects : public VarObjects {
public:
	long double value;
public:
	DoubleObjects(const std::shared_ptr<TokenStruct> &tok)
		: VarObjects(TokenType::DECIMAL, std::get<2>(tok->token), std::get<3>(tok->token)) {
		this->value = std::stod(std::get<0>(tok->token));
	}
	inline const std::string returnStringValue() { return std::to_string(this->value); }
};
