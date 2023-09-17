#pragma once
#include "baseInsObj.hpp"

class ListObject : public VarObjects {
public:
	std::vector<std::shared_ptr<Expression>> list;

public:
	ListObject(const std::vector<std::shared_ptr<Expression>> &_list)
	: list (_list), VarObjects(TokenType::LIST) { }

	inline const char *returnStringValue(const std::string &space = "") { return "LIST"; }
};

class MapObject : public VarObjects {
public:
	std::vector<std::shared_ptr<MapStruct>> map;

public:
	MapObject(const std::vector<std::shared_ptr<MapStruct>> &_map)
	: map (_map), VarObjects(TokenType::MAP) { }
	inline const char *returnStringValue(const std::string &space = "") { return "MAP"; }
};


class LambdaObjects : public VarObjects {
public:
	bool wilLCall = false;
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
	VarObjects(TokenType::FUNCTIONS),
	wilLCall     (_willCall    ),
	arguments    (_arguments   ),
	parameters   (_parameters  ),
	blockWillRun (_blockWillRun) {}

	inline const char *returnStringValue() { return "LAMBDA"; }
};

class FunctionObjects : public VarObjects {
public:
	const char *name;
	std::vector<std::shared_ptr<Expression>> arguments;

public:
	FunctionObjects(
		const std::shared_ptr<TokenStruct>             &tok        = nullptr,
		const std::vector<std::shared_ptr<Expression>> &_arguments = {}
		) :
		name(std::get<0>(tok->token).c_str()),
		VarObjects(TokenType::FUNCTIONS, std::get<2>(tok->token), std::get<3>(tok->token)),
		arguments (_arguments) {}

	inline const char *returnStringValue() { return this->name; }
};

class StringObjects : public VarObjects {
public:
	std::string value;
public:
	StringObjects(const std::shared_ptr<TokenStruct> &tok)
		: VarObjects(TokenType::STRING, std::get<2>(tok->token), std::get<3>(tok->token)) {
		this->value = std::get<0>(tok->token);
	}
	inline const char *returnStringValue() { return this->value.c_str(); }
};

class IntegerObjects : public VarObjects {
public:
	int64_t value;
public:
	IntegerObjects(const std::shared_ptr<TokenStruct> &tok)
		: VarObjects(TokenType::INTEGER, std::get<2>(tok->token), std::get<3>(tok->token)) {
		this->value = std::stoll(std::get<0>(tok->token));
	}
	inline const int64_t returnStringValue() { return this->value; }
};

class DoubleObjects : public VarObjects {
public:
	long double value;
public:
	DoubleObjects(const std::shared_ptr<TokenStruct> &tok)
		: VarObjects(TokenType::DECIMAL, std::get<2>(tok->token), std::get<3>(tok->token)) {
		this->value = std::stod(std::get<0>(tok->token));
	}
	inline const long double returnStringValue() { return this->value; }
};

namespace JDM {
	const void checkAndCallReturnStringValue(const std::shared_ptr<VarObjects>& varObj) {
		if (varObj) {
			switch (varObj->getToken()) {
				case TokenType::FUNCTIONS:
					if (auto derivedObj = std::dynamic_pointer_cast<FunctionObjects>(varObj))
						std::cout << derivedObj->returnStringValue();
					break;
				case TokenType::VARIABLE:
					if (auto derivedObj = std::dynamic_pointer_cast<VariableObjects>(varObj))
						std::cout << derivedObj->returnStringValue();
					break;
				case TokenType::STRING:
					if (auto derivedObj = std::dynamic_pointer_cast<StringObjects>(varObj))
						std::cout <<  derivedObj->returnStringValue();
					break;
				case TokenType::INTEGER:
					if (auto derivedObj = std::dynamic_pointer_cast<IntegerObjects>(varObj))
						std::cout <<  derivedObj->returnStringValue();
					break;
				case TokenType::DECIMAL:
					if (auto derivedObj = std::dynamic_pointer_cast<DoubleObjects>(varObj))
						std::cout <<  derivedObj->returnStringValue();
					break;
				default:
					std::cout << "Unsupported TokenType" << std::endl;
					break;
			}
		}
	}
}