#pragma once

namespace JDM
{
	enum TokenType
	{
		UNDEFINED,
		DATA_TYPE, 
		CONTROL_FLOW,
		CUSTOM_KEYWORD,
		CUSTOM_FUNCTIONS,
		FUNCTIONS,
		OPEN_CASES,
		CLOSE_CASES,
		DOT_OPERATOR,
		ARROW_OPERATOR,
		COMMA_OPERATOR,
		INCREMENT_DECREMENT_OPERATOR,
		ASSIGNMENT_OPERATOR,
		ARITHMETIC_OPERATOR,
		RELATIONAL_OPERATOR,
		BITWISE_OPERATOR,
		STRING,
		INTEGER,
		DOUBLE,
		BOOLED,
		VARIABLE,
		LIST,
		MAP,
		LAMBDA,
		CALL_OBJ,
		CLASS,
		CAST,
		EXPRESSION
	};
};

enum DataTypeEnum
{
	DATA_ANY,
	DATA_MAP,
	DATA_LIST,
	DATA_STRING,
	DATA_DOUBLE,
	DATA_INTEGER,
	DATA_BOOLEAN,
	DATA_LAMBDA,
	DATA_OBJECT,
	DATA_CONST,
	DATA_FORCE,
	DATA_CFORCE
};

enum ControlFlowEnum
{
	CONTROL_IF,
	CONTROL_ELSEIF,
	CONTROL_ELSE,
	CONTROL_FOR,
	CONTROL_FOREACH,
	CONTROL_WHILE,
	CONTROL_SWITCH,
	CONTROL_CASE,
	CONTROL_FUNC,
	CONTROL_RETURN,
	CONTROL_THEN
};

enum CustomKeywordEnum
{
	KEYWORD_REVERSE,
	KEYWORD_BREAK,
	KEYWORD_CONTINUE,
	KEYWORD_DEFAULT,
	KEYWORD_TRUE,
	KEYWORD_FALSE
};

enum CustomFunctionEnum
{
	CUSFUNC_LOG,
	CUSFUNC_LOGN,
	CUSFUNC_CLEAR,
	CUSFUNC_SLEEP,
	CUSFUNC_INCLUDE
};

