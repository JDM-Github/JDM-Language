#pragma once
#include "objects.hpp"

class HigherFunctionObjects : public FunctionObjects {
public:
	std::shared_ptr<Block> blockToExecute;
	std::vector<std::shared_ptr<VariableObjects>> parameters;

public:
	HigherFunctionObjects(
		const std::shared_ptr<TokenStruct    > &_tok,
		const std::shared_ptr<Block          > &_blockToExecute,
		const std::vector<std::shared_ptr<VariableObjects>> &_parameters = {}
	) :
	blockToExecute (_blockToExecute),
	parameters     (_parameters    ),
	FunctionObjects(_tok           ) {}
};

class HigherVariableObjects : public VariableObjects {
public:
	bool isString   = false;
	bool isInteger  = false;
	bool isDecimal  = false;
	bool isBoolean  = false;
	bool isFunction = false;

	const char      *name;
	std::string      str_value;
	int64_t          int_value;
	long double      double_value;

	std::shared_ptr<HigherFunctionObjects> function;

public:
	HigherVariableObjects(const std::shared_ptr<TokenStruct> &tok) : VarObjects(tok) {
	}
};