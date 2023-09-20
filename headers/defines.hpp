#pragma once
#include <iostream>
#include <tuple>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "enums.hpp"

#define JDM_DLL            __declspec(dllexport)
#define EndLine            ';'
#define FileStream         const std::string
#define InputStream        const std::string
#define Log                std::cout

#define CVoid              const void
#define CBool              const bool

#define SizeT              size_t
#define CSizeT             const size_t
#define CTokenType         const TokenType

#define SpaceString        const std::string&

#define TokenStr           std::string
#define CTokenStrRef       const TokenStr&

#define VecChar            std::vector<char>
#define CVecChar           const VecChar
#define CVecCharRef        const VecChar&

#define VecTokenStr        std::vector<TokenStr>
#define CVecTokenStr       const VecTokenStr
#define CVecTokenStrRef    const VecTokenStr&

#define MapTokens          std::unordered_map<TokenStr, TokenType>
#define CMapTokens         const MapTokens

struct TokenStruct {
	std::tuple<TokenStr, TokenType, size_t, size_t> token =
		std::make_tuple("", TokenType::UNDEFINED, 0, 0);
	std::vector<std::shared_ptr<TokenStruct>> child = {};
};

#define SharedTokenStruct     std::shared_ptr<TokenStruct>
#define MSharedTokenStruct    std::make_shared<TokenStruct>
#define CSharedTokenStruct    const SharedTokenStruct
#define CSharedTokenStructRef const SharedTokenStruct&

struct TokenCurrentLink {
	std::shared_ptr<TokenCurrentLink> next;
	std::shared_ptr<TokenCurrentLink> prev;
	SharedTokenStruct current;
	std::vector<SharedTokenStruct> currTokens;
};

#define SharedTokenLink       std::shared_ptr<TokenCurrentLink>
#define MSharedTokenLink      std::make_shared<TokenCurrentLink>
#define CSharedTokenLink      const SharedTokenStruct
#define CSharedTokenLinkRef   const SharedTokenStruct&

template <class _TypeClass>
	CBool isInVec(const _TypeClass &e, const std::vector<_TypeClass> eList) {
	for (SizeT i = 0; i < eList.size(); i++) if (e == eList[i])
		return true;
	return false;
}

namespace JDM {

	const std::vector<std::string> powerVec   = { "**"                             };
	const std::vector<std::string> multDivVec = { "*", "/", "//"                   };
	const std::vector<std::string> addSubVec  = { "+", "-"                         };
	const std::vector<std::string> notVec     = { "~"                              };
	const std::vector<std::string> shiftVec   = { "<<", ">>"                       };
	const std::vector<std::string> andVec     = { "&"                              };
	const std::vector<std::string> xorVec     = { "^"                              };
	const std::vector<std::string> orVec      = { "|"                              };
	const std::vector<std::string> logicalVec = { "<", ">", "<=", ">=", "==", "!=" };
	const std::vector<std::string> relNotVec  = { "!"                              };
	const std::vector<std::string> relAndVec  = { "&&"                             };
	const std::vector<std::string> relOrVec   = { "||"                             };

	const std::vector<std::string> operatorCombinedVector = {
		"**", "*", "/", "//", "+", "-", "~", "<<", ">>", "&",
		"^", "|", "<", ">", "<=", ">=", "==", "!=", "!", "&&", "||"
	};

	const std::unordered_map<std::string, ControlFlowEnum> controlFlowMap = {
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

    const std::unordered_map<std::string, DataTypeEnum> dataTypeMap = {
        {"jany",     DataTypeEnum::DATA_ANY},
        {"jmap",     DataTypeEnum::DATA_MAP},
        {"jlist",    DataTypeEnum::DATA_LIST},
        {"jstring",  DataTypeEnum::DATA_STRING},
        {"jdouble",  DataTypeEnum::DATA_DOUBLE},
        {"jint",     DataTypeEnum::DATA_INTEGER},
        {"jboolean", DataTypeEnum::DATA_BOOLEAN},
        {"jlambda",  DataTypeEnum::DATA_LAMBDA}
    };

    const std::unordered_map<std::string, CustomKeywordEnum> customKeywordMap = {
        {"jreverse",  CustomKeywordEnum::KEYWORD_REVERSE},
        {"jbreak",    CustomKeywordEnum::KEYWORD_BREAK},
        {"jcontinue", CustomKeywordEnum::KEYWORD_CONTINUE},
        {"jdefault",  CustomKeywordEnum::KEYWORD_DEFAULT},
        {"jtrue",     CustomKeywordEnum::KEYWORD_TRUE},
        {"jfalse",    CustomKeywordEnum::KEYWORD_FALSE}
    };

    const std::unordered_map<std::string, CustomFunctionEnum> customFunctionMap = {
        {"$log",     CUSFUNC_LOG},
        {"$cast",    CUSFUNC_CAST},
        {"$gettype", CUSFUNC_GETTYPE},
        {"$sort",    CUSFUNC_SORT}
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

