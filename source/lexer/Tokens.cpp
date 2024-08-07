#include "Lexer.hpp"

JDM_DLL Tokens::Tokens()
	: __ignored_keywords   ({ ' ', '\t', '\n' }),
	  __escape_combination ({ '\\', '\'', '"', '?', 'a', 'b', 'f', 'n', 'r', 't', 'v' }),
	  __operator_symbol    ({ '=', '!', '+', '-', '*', '/', '%', '&', '|', '>', '<', '^', '~', '.', ','}),
	  __tokenMap           ({
			{"^(-?[0-9]*)$"              , JDM::TokenType::INTEGER},
			{"^(-?[0-9]*\\.[0-9]*)$"     , JDM::TokenType::DOUBLE},
			{"^[a-zA-Z_][a-zA-Z0-9_]*$"  , JDM::TokenType::VARIABLE} })
{
	// Data Type
	this->__dataTypeVector.push_back   ("jany"     );
	this->__dataTypeVector.push_back   ("jlist"    );
	this->__dataTypeVector.push_back   ("jmap"     );
	this->__dataTypeVector.push_back   ("jstring"  );
	this->__dataTypeVector.push_back   ("jdouble"  );
	this->__dataTypeVector.push_back   ("jint"     );
	this->__dataTypeVector.push_back   ("jboolean" );
	this->__dataTypeVector.push_back   ("jlambda"  );
	this->__dataTypeVector.push_back   ("jobject"  );

	this->__dataTypeVector.push_back   ("jconst"   );
	this->__dataTypeVector.push_back   ("jforce"   );
	this->__dataTypeVector.push_back   ("jcforce"  );

	// Control Flow
	this->__controlFlowVector.push_back("jif"      );
	this->__controlFlowVector.push_back("jelseif"  );
	this->__controlFlowVector.push_back("jelse"    );

	this->__controlFlowVector.push_back("jfor"     );
	this->__controlFlowVector.push_back("jforeach" );
	this->__controlFlowVector.push_back("jwhile"   );
	this->__controlFlowVector.push_back("jswitch"  );
	this->__controlFlowVector.push_back("jfunc"    );
	this->__controlFlowVector.push_back("jthen"    );

	this->__controlFlowVector.push_back("jcase"    );
	this->__controlFlowVector.push_back("jreturn"  );

	this->__keywordVector.push_back    ("jbreak"   );
	this->__keywordVector.push_back    ("jreverse" );
	this->__keywordVector.push_back    ("jcontinue");
	this->__keywordVector.push_back    ("jdefault" );
	this->__keywordVector.push_back    ("jtrue"    );
	this->__keywordVector.push_back    ("jfalse"   );

	this->__functionsVector.push_back  ("$log"      );
	this->__functionsVector.push_back  ("$logn"     );
	this->__functionsVector.push_back  ("$clear"    );
	this->__functionsVector.push_back  ("$sleep"    );
	this->__functionsVector.push_back  ("$include"  );
}
