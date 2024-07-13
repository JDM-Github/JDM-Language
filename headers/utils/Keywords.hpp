#pragma once

namespace JDM
{
	CVector<StringStd> powerVec   = { "**"                             };
	CVector<StringStd> multDivVec = { "*", "/", "//", "%"              };
	CVector<StringStd> addSubVec  = { "+", "-"                         };
	CVector<StringStd> notVec     = { "~"                              };
	CVector<StringStd> shiftVec   = { "<<", ">>"                       };
	CVector<StringStd> andVec     = { "&"                              };
	CVector<StringStd> xorVec     = { "^"                              };
	CVector<StringStd> orVec      = { "|"                              };
	CVector<StringStd> logicalVec = { "<", ">", "<=", ">=", "==", "!=" };
	CVector<StringStd> relNotVec  = { "!"                              };
	CVector<StringStd> relAndVec  = { "&&"                             };
	CVector<StringStd> relOrVec   = { "||"                             };

	const std::vector<StringStd> operatorCombinedVector = {
		"**", "*", "/", "//", "%", "+", "-", "~", "<<", ">>", "&",
		"^", "|", "<", ">", "<=", ">=", "==", "!=", "!", "&&", "||"
	};

	const std::unordered_map<StringStd, ControlFlowEnum> controlFlowMap = {
        {"jif"     , ControlFlowEnum::CONTROL_IF     },
        {"jelseif" , ControlFlowEnum::CONTROL_ELSEIF },
        {"jelse"   , ControlFlowEnum::CONTROL_ELSE   },
        {"jfor"    , ControlFlowEnum::CONTROL_FOR    },
        {"jforeach", ControlFlowEnum::CONTROL_FOREACH},
        {"jwhile"  , ControlFlowEnum::CONTROL_WHILE  },
        {"jswitch" , ControlFlowEnum::CONTROL_SWITCH },
        {"jcase"   , ControlFlowEnum::CONTROL_CASE   },
        {"jfunc"   , ControlFlowEnum::CONTROL_FUNC   },
        {"jreturn" , ControlFlowEnum::CONTROL_RETURN },
        {"jthen"   , ControlFlowEnum::CONTROL_THEN   }
    };

    const std::unordered_map<StringStd, DataTypeEnum> dataTypeMap = {
        {"jany"    , DataTypeEnum::DATA_ANY    },
        {"jmap"    , DataTypeEnum::DATA_MAP    },
        {"jlist"   , DataTypeEnum::DATA_LIST   },
        {"jstring" , DataTypeEnum::DATA_STRING },
        {"jdouble" , DataTypeEnum::DATA_DOUBLE },
        {"jint"    , DataTypeEnum::DATA_INTEGER},
        {"jboolean", DataTypeEnum::DATA_BOOLEAN},
        {"jlambda" , DataTypeEnum::DATA_LAMBDA },
        {"jobject" , DataTypeEnum::DATA_OBJECT },
        {"jconst"  , DataTypeEnum::DATA_CONST  },
        {"jforce"  , DataTypeEnum::DATA_FORCE  },
        {"jcforce" , DataTypeEnum::DATA_CFORCE },
    };

    const std::unordered_map<StringStd, CustomKeywordEnum> customKeywordMap = {
        {"jreverse" , CustomKeywordEnum::KEYWORD_REVERSE },
        {"jbreak"   , CustomKeywordEnum::KEYWORD_BREAK   },
        {"jcontinue", CustomKeywordEnum::KEYWORD_CONTINUE},
        {"jdefault" , CustomKeywordEnum::KEYWORD_DEFAULT },
        {"jtrue"    , CustomKeywordEnum::KEYWORD_TRUE    },
        {"jfalse"   , CustomKeywordEnum::KEYWORD_FALSE   }
    };

    const std::unordered_map<StringStd, CustomFunctionEnum> customFunctionMap = {
        {"$log"    , CUSFUNC_LOG    },
        {"$logn"   , CUSFUNC_LOGN   },
        {"$clear"  , CUSFUNC_CLEAR  },
        {"$sleep"  , CUSFUNC_SLEEP  },
        {"$include", CUSFUNC_INCLUDE}
    };

	constexpr const char *tokenTypeToString(const JDM::TokenType type)
	{
		switch (type) {
			case JDM::TokenType::DATA_TYPE                    : return "DATA_TYPE";
			case JDM::TokenType::CONTROL_FLOW                 : return "CONTROL_FLOW";
			case JDM::TokenType::CUSTOM_KEYWORD               : return "CUSTOM_KEYWORD";
			case JDM::TokenType::CUSTOM_FUNCTIONS             : return "CUSTOM_FUNCTIONS";
			case JDM::TokenType::FUNCTIONS                    : return "FUNCTIONS";
			case JDM::TokenType::STRING                       : return "STRING";
			case JDM::TokenType::OPEN_CASES                   : return "OPEN_CASES";
			case JDM::TokenType::CLOSE_CASES                  : return "CLOSE_CASES";
			case JDM::TokenType::ARROW_OPERATOR               : return "ARROW_OPERATOR";
			case JDM::TokenType::COMMA_OPERATOR               : return "COMMA_OPERATOR";
			case JDM::TokenType::DOT_OPERATOR                 : return "DOT_OPERATOR";
			case JDM::TokenType::INCREMENT_DECREMENT_OPERATOR : return "INCREMENT_DECREMENT_OPERATOR";
			case JDM::TokenType::ASSIGNMENT_OPERATOR          : return "ASSIGNMENT_OPERATOR";
			case JDM::TokenType::ARITHMETIC_OPERATOR          : return "ARITHMETIC_OPERATOR";
			case JDM::TokenType::RELATIONAL_OPERATOR          : return "RELATIONAL_OPERATOR";
			case JDM::TokenType::BITWISE_OPERATOR             : return "BITWISE_OPERATOR";
			case JDM::TokenType::INTEGER                      : return "INTEGER";
			case JDM::TokenType::DOUBLE                       : return "DECIMAL";
			case JDM::TokenType::LIST                         : return "LIST";
			case JDM::TokenType::MAP                          : return "MAP";
			case JDM::TokenType::VARIABLE                     : return "VARIABLE";
			default                                           : return "UNDEFINED";
		}
	}
};
