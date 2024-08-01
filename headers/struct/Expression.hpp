#pragma once

struct Expression
{
	std::shared_ptr<VarObjects > firstValue;
	std::shared_ptr<VarObjects > secondValue;
	std::shared_ptr<Expression > firstExpression;
	std::shared_ptr<TokenStruct> opWillUse;
	std::shared_ptr<Expression > secondExpression;
};

