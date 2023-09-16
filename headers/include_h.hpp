#pragma once
#include <iostream>
#include <tuple>
#include <memory>
#include <vector>
#include <string>

#define J_LINE_BREAKER     ';'
#define J_INPUT_STREAMS    const std::string
#define J_CTOKENSR         const std::string&
#define J_TOKENS           std::string
#define J_LOG_STREAMS      std::cout

#define JDM_DLL __declspec(dllexport)

// #define scast  static_cast
// #define dcast  dynamic_cast
// #define rcast  reinterpret_cast
// #define dpcast std::dynamic_pointer_cast

// template <class T> using sptr = std::shared_ptr<T>;
// #define msptr std::make_shared

// template <class T> using uptr = std::unique_ptr<T>;
// #define muptr std::make_unique

// #define ccharptr const char *
// #define cstrref const std::string &

// std::string         typedef str;
// const std::string   typedef cstr;
// const void          typedef cvoid;

// template <class T> using vecsptr  = std::vector<std::shared_ptr<T>>;
// template <class T> using cvecsptr = const std::vector<std::shared_ptr<T>>;
// template <class T> using csptr    = const std::shared_ptr<T>;
// template <class T> using csptrref = const std::shared_ptr<T> &;

enum TokenType {
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
	DECIMAL,
	VARIABLE,
	LIST,
	MAP
};

enum DataTypeEnum {
	DATA_ANY,
	DATA_MAP,
	DATA_LIST,
	DATA_STRING,
	DATA_DOUBLE,
	DATA_INTEGER,
	DATA_BOOLEAN,
	DATA_LAMBDA
};

enum ControlFlowEnum {
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

enum CustomKeywordEnum {
	KEYWORD_REVERSE,
	KEYWORD_BREAK,
	KEYWORD_CONTINUE,
	KEYWORD_DEFAULT,
	KEYWORD_TRUE,
	KEYWORD_FALSE
};

enum CustomFunctionEnum {
	CUSFUNC_LOG,
	CUSFUNC_CAST,
	CUSFUNC_GETTYPE,
	CUSFUNC_SORT
};

struct TokenStruct {
	std::tuple<J_TOKENS, TokenType, size_t, size_t> token =
		std::make_tuple("", TokenType::UNDEFINED, 0, 0);
	std::vector<std::shared_ptr<TokenStruct>> child = {};
};

namespace JDM {
	const std::map<std::string, ControlFlowEnum> controlFlowMap = {
        {"jif",      ControlFlowEnum::CONTROL_IF},
        {"jelseif",  ControlFlowEnum::CONTROL_ELSEIF},
        {"jelse",    ControlFlowEnum::CONTROL_ELSE},
        {"jfor",     ControlFlowEnum::CONTROL_FOR},
        {"jforeach", ControlFlowEnum::CONTROL_FOREACH},
        {"jwhile",   ControlFlowEnum::CONTROL_WHILE},
        {"jswitch",  ControlFlowEnum::CONTROL_SWITCH},
        {"jcase",    ControlFlowEnum::CONTROL_CASE},
        {"jfunc",    ControlFlowEnum::CONTROL_FUNC},
        {"jreturn",  ControlFlowEnum::CONTROL_RETURN},
        {"jthen",    ControlFlowEnum::CONTROL_THEN}
    };

    const std::map<std::string, DataTypeEnum> dataTypeMap = {
        {"jany",     DataTypeEnum::DATA_ANY},
        {"jmap",     DataTypeEnum::DATA_MAP},
        {"jlist",    DataTypeEnum::DATA_LIST},
        {"jstring",  DataTypeEnum::DATA_STRING},
        {"jdouble",  DataTypeEnum::DATA_DOUBLE},
        {"jint",     DataTypeEnum::DATA_INTEGER},
        {"jboolean", DataTypeEnum::DATA_BOOLEAN},
        {"jlambda",  DataTypeEnum::DATA_LAMBDA}
    };

    const std::map<std::string, CustomKeywordEnum> customKeywordMap = {
        {"jreverse",  CustomKeywordEnum::KEYWORD_REVERSE},
        {"jbreak",    CustomKeywordEnum::KEYWORD_BREAK},
        {"jcontinue", CustomKeywordEnum::KEYWORD_CONTINUE},
        {"jdefault",  CustomKeywordEnum::KEYWORD_DEFAULT},
        {"jtrue",     CustomKeywordEnum::KEYWORD_TRUE},
        {"jfalse",    CustomKeywordEnum::KEYWORD_FALSE}
    };

    const std::map<std::string, CustomFunctionEnum> customFunctionMap = {
        {"$log",     CUSFUNC_LOG},
        {"$cast",    CUSFUNC_CAST},
        {"$gettype", CUSFUNC_GETTYPE},
        {"$sort",    CUSFUNC_SORT}
        // {"$gettype", CUSFUNC_GETTYPE}
    };

	constexpr const char *tokenTypeToString(const TokenType type) {
		switch (type) {
			case TokenType::DATA_TYPE                    : return "DATA_TYPE";
			case TokenType::CONTROL_FLOW                 : return "CONTROL_FLOW";
			case TokenType::CUSTOM_KEYWORD               : return "CUSTOM_KEYWORD";
			case TokenType::CUSTOM_FUNCTIONS             : return "CUSTOM_FUNCTIONS";
			case TokenType::FUNCTIONS                    : return "FUNCTIONS";
			case TokenType::STRING                       : return "STRING";
			case TokenType::OPEN_CASES                   : return "OPEN_CASES";
			case TokenType::CLOSE_CASES                  : return "CLOSE_CASES";
			case TokenType::ARROW_OPERATOR               : return "ARROW_OPERATOR";
			case TokenType::COMMA_OPERATOR               : return "COMMA_OPERATOR";
			case TokenType::DOT_OPERATOR                 : return "DOT_OPERATOR";
			case TokenType::INCREMENT_DECREMENT_OPERATOR : return "INCREMENT_DECREMENT_OPERATOR";
			case TokenType::ASSIGNMENT_OPERATOR          : return "ASSIGNMENT_OPERATOR";
			case TokenType::ARITHMETIC_OPERATOR          : return "ARITHMETIC_OPERATOR";
			case TokenType::RELATIONAL_OPERATOR          : return "RELATIONAL_OPERATOR";
			case TokenType::BITWISE_OPERATOR             : return "BITWISE_OPERATOR";
			case TokenType::INTEGER                      : return "INTEGER";
			case TokenType::DECIMAL                      : return "DECIMAL";
			case TokenType::LIST                         : return "LIST";
			case TokenType::MAP                          : return "MAP";
			case TokenType::VARIABLE                     : return "VARIABLE";
			default                                      : return "UNDEFINED";
		}
	}
};

