#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <cstdlib>
#include <tuple>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <regex>
#include <memory>

#include "include_h.hpp"
#include "error_handler.hpp"

class Tokens {
protected:

	const std::vector<char> __ignored_keywords   = { ' ', '\t', '\n' };
	const std::vector<char> __escape_combination = { '\'', '"', '?', '\\', 'a', 'b', 'f', 'n', 'r', 't', 'v' };
	const std::vector<char> __operator_symbol    = { '=', '+', '-', '*', '/', '%', '&', '|', '>', '<', '^', '~', '.', ','};

	const std::unordered_map<std::string, TokenType> __tokenMap = {
    	{"^(-?[0-9]*)$"              , TokenType::INTEGER},
    	{"^(-?[0-9]*\\.[0-9]*)$"     , TokenType::DECIMAL},
    	{"^[a-zA-Z_][a-zA-Z0-9_]*$"  , TokenType::VARIABLE}
	};
	std::vector<J_TOKENS> __dataTypeVector;
	std::vector<J_TOKENS> __controlFlowVector;
	std::vector<J_TOKENS> __keywordVector;
	std::vector<J_TOKENS> __functionsVector;

	inline Tokens() {
		// Data Type
		this->__dataTypeVector.push_back   ("jany"    );
		this->__dataTypeVector.push_back   ("jlist"   );
		this->__dataTypeVector.push_back   ("jmap"    );
		this->__dataTypeVector.push_back   ("jstring" );
		this->__dataTypeVector.push_back   ("jdouble" );
		this->__dataTypeVector.push_back   ("jint"    );
		this->__dataTypeVector.push_back   ("jboolean");
		this->__dataTypeVector.push_back   ("jlambda" );

		// Control Flow
		this->__controlFlowVector.push_back("jif"    );
		this->__controlFlowVector.push_back("jelseif");
		this->__controlFlowVector.push_back("jelse"  );

		this->__controlFlowVector.push_back("jfor");
		this->__controlFlowVector.push_back("jforeach");
		this->__controlFlowVector.push_back("jwhile");
		this->__controlFlowVector.push_back("jswitch");
		this->__controlFlowVector.push_back("jfunc");
		this->__controlFlowVector.push_back("jthen");

		this->__controlFlowVector.push_back("jcase");
		this->__controlFlowVector.push_back("jreturn");

		this->__keywordVector.push_back    ("jbreak");
		this->__keywordVector.push_back    ("jreverse");
		this->__keywordVector.push_back    ("jcontinue");
		this->__keywordVector.push_back    ("jdefault");
		this->__keywordVector.push_back    ("jtrue");
		this->__keywordVector.push_back    ("jfalse");

		this->__functionsVector.push_back  ("$log");
		this->__functionsVector.push_back  ("$cast");
		this->__functionsVector.push_back  ("$getType");
		this->__functionsVector.push_back  ("$sort");

		// OOP
		// this->__keywordVector.push_back("jclass");
		// this->__keywordVector.push_back("jpublic");
		// this->__keywordVector.push_back("jprotected");
		// this->__keywordVector.push_back("jprivate");

		// this->__keywordVector.push_back("jimport");
	}
};

class Tokenizer : public Tokens {
private:
	J_INPUT_STREAMS __input_buffer;
	std::string     __filename;

	J_TOKENS        __last_token     = "";
	J_TOKENS        __current_token  = "";
	size_t          __current_column = 0;
	size_t          __current_row    = 0;

	size_t          __track_column   = 0;
	size_t          __track_row      = 1;

	char __is_start_string           = 'N';
	bool __is_line_breaker           = true;
	bool __just_added_token          = false;
	bool __just_added_escape         = false;
	bool __is_comment_line           = false;
	bool __is_comment_block          = false;
	bool __is_in_paren               = false;
	bool __candidate_block           = false;

	std::ostringstream __stringStream;
	std::vector<char> __opening_patt   = {};
	TokenType __last_toke_type = UNDEFINED;

	struct TokenCurrentLink {
		TokenCurrentLink *next    = nullptr;
		TokenCurrentLink *prev    = nullptr;

		std::shared_ptr<TokenStruct> current;
		std::vector<std::shared_ptr<TokenStruct>> currTokens = {};
	};

	std::shared_ptr<TokenStruct> __allTokens;
	std::shared_ptr<TokenStruct> __lastTokStruct;

	TokenCurrentLink *__currentStruct;
	std::unordered_map<std::string, TokenType> __cache_tokType;

public:
	inline Tokenizer(const std::string &filename, J_INPUT_STREAMS &input_buffer)
		: __filename(filename), __input_buffer(input_buffer), Tokens()
	{
		this->_getTokens();
		if (!this->__opening_patt.empty())
			throw UnterminatedCases(this->__filename, this->__last_token, this->__current_token, this->__track_row, this->__track_column);
	}

	inline ~Tokenizer() {
		delete this->__currentStruct;
	}

	inline const void analyzeAllTokens(const bool willPrint=false) {
		this->__stringStream.str("");
		this->__stringStream << "{\n";
		for (const auto& tokenStruct : this->__allTokens->child) {
			this->__stringStream << "  [\n";
			for (const auto& token : tokenStruct->child)
				this->_traverseTokenStruct(token, "    ");
			this->__stringStream << "  ],\n";
		}
		this->__stringStream << "}\n";
		if (willPrint) std::cout << this->__stringStream.str() << '\n';
	}

	inline const void saveTokens(const std::string &filename) {
        std::ofstream outputFile(filename);
        if (this->__stringStream.str().size())
            this->analyzeAllTokens();

        outputFile << this->__stringStream.str();
        outputFile.close();
    }

    inline const std::shared_ptr<TokenStruct> getTokens() {
    	return this->__allTokens;
    }

private:
	inline const void _traverseTokenStruct(const std::shared_ptr<TokenStruct> &curr, const std::string &space) {
		this->__stringStream
        	<< space << "{\n"
        	<< space << "  token: "     << std::get<0>(curr->token) << ",\n"
        	<< space << "  tokenType: " << JDM::tokenTypeToString(std::get<1>(curr->token)) << ",\n"
        	<< space << "  row: "       << std::get<2>(curr->token) << ",\n"
        	<< space << "  col: "       << std::get<3>(curr->token);

		if (!curr->child.empty()) {
			this->__stringStream << ",\n" << space << "  child: {\n";
			for (const auto& tokenStruct : curr->child) {
				this->__stringStream << space << "    [\n";
				for (const auto& token : tokenStruct->child)
					this->_traverseTokenStruct(token, space + "      ");
				this->__stringStream << space << "    ],\n";
			}
			this->__stringStream << space << "  }";
		}
		this->__stringStream << '\n' << space << "},\n";
	}

	inline const bool _checkKeyword(J_CTOKENSR input, const std::vector<J_TOKENS> &tokens) const noexcept {
		for (const auto& keyword : tokens)
			if (input.compare(keyword) == 0) return true;
		return false;
	}

	template <class _TypeClass>
	inline const bool _isIn(const _TypeClass &e, const std::vector<_TypeClass> eList) const noexcept {
		for (size_t i = 0; i < eList.size(); i++)
			if (e == eList[i]) return true;
		return false;
	}

	inline const bool _isNextTokenInIgnored(size_t _index) const noexcept {
		return this->_isIn(this->__input_buffer[_index], this->__ignored_keywords);
	}

	inline const TokenType _determineTokenType(J_CTOKENSR token) const {
		if (this->__cache_tokType.count(token) > 0)
			return this->__cache_tokType.at(token);

		if (this->_checkKeyword(token, this->__dataTypeVector   )) return TokenType::DATA_TYPE;
		if (this->_checkKeyword(token, this->__controlFlowVector)) return TokenType::CONTROL_FLOW;
		if (this->_checkKeyword(token, this->__keywordVector    )) return TokenType::CUSTOM_KEYWORD;
		if (this->_checkKeyword(token, this->__functionsVector  )) return TokenType::CUSTOM_FUNCTIONS;

		if ((token.size() >= 2 && (token.front() == '"'  && token.back() == '"' ))
		||  (token.size() >= 2 && (token.front() == '\'' && token.back() == '\'')))
			return TokenType::STRING;

		// I did not use regex, as using this is faster
		if (token == "->" || token == "=>")  return TokenType::ARROW_OPERATOR;
		if (token == "=")                    return TokenType::ASSIGNMENT_OPERATOR;
		if (token == ".")                    return TokenType::DOT_OPERATOR;
		if (token == ",")                    return TokenType::COMMA_OPERATOR;
		if (token == "++" || token == "--")  return TokenType::INCREMENT_DECREMENT_OPERATOR;

		if (token == "("  || token == "["  || token == "{")
			return TokenType::OPEN_CASES;

		if (token == ")"  || token == "]"  || token == "}")
			return TokenType::CLOSE_CASES;

		if (token == "+"  || token == "-"  || token == "*" || token == "**"
		 || token == "/"  || token == "%"  || token == "//")
			return TokenType::ARITHMETIC_OPERATOR;

		if (token == "&&" || token == "||" || token == "==" || token == "!=" 
		 || token == "<"  || token == "<=" || token == ">=" || token == ">" )
			return TokenType::RELATIONAL_OPERATOR;

		if (token == "&"  || token == "|"  || token == "^"
		 || token == "~"  || token == "<<" || token == ">>")
			return TokenType::BITWISE_OPERATOR;

		// Use regex from complex pattern
		for (const auto& entry : this->__tokenMap) {
        	std::regex pattern(entry.first);
        	if (std::regex_match(token, pattern))
            	return entry.second;
    	}
    	throw UnrecognizeToken(this->__filename, this->__last_token, token, this->__track_row, this->__track_column);
	}

	inline const bool _checkNextToken(const size_t i, J_CTOKENSR compStr, const std::vector<char> &vecCharacters) noexcept {
		if (this->__current_token.compare(compStr) == 0) {
			this->__just_added_token = false;
			if (this->_isIn(this->__input_buffer[i], vecCharacters)) {
				this->__current_token += this->__input_buffer[i];
				this->_addToken();
				return true;
			}
		}
		return false;
	}

	inline const bool _checkIfTokenIsOperator(J_CTOKENSR token) const noexcept {
		return (token == "==" || token == "++" || token == "--" || token == "**" || token == "//"
			 || token == "%"  || token == "&&" || token == "||" || token == "=>" || token == "<="
			 || token == ">=" || token == "=<" || token == "<<" || token == ">>" || token == "^"
			 || token == "~"  || token == ".");
	}

	inline const bool _checkIfNextTokenIsOperatorStart(const size_t i, const bool willAdd=true) noexcept {
		if (this->__input_buffer.size() > i+1) {
			if (this->_isIn(this->__input_buffer[i+1], {'(', '[', '{'})) {
				if (willAdd) this->_addToken();
				this->__just_added_token = false;
				return true;
			}

			if (this->__input_buffer[i+1] == '.')
				if (std::regex_match(this->__current_token, std::regex("^(-?[0-9]*)$")))
					return true;

			if (!this->_isIn(this->__input_buffer[i],   this->__operator_symbol)
			 &&  this->_isIn(this->__input_buffer[i+1], this->__operator_symbol)) {
				if (willAdd) this->_addToken();
				this->__just_added_token = false;
				return true;
			}
		}
		return false;
	}

	inline const bool _handle_escape(const size_t i) {
		if (this->__just_added_escape) {
			if (!this->_isIn(this->__input_buffer[i], this->__escape_combination)) 
				throw IllegalEscape(this->__filename, this->__last_token, this->__current_token, this->__track_row, this->__track_column);
			this->__just_added_escape = false;
			return true;
		}
		else if (this->__input_buffer[i] == '\\') {
			if (this->__is_start_string == 'N') 
				throw IllegalEscape(this->__filename, this->__last_token, this->__current_token, this->__track_row, this->__track_column);
			this->__just_added_escape = true;
		}
		return false;
	}

	inline const bool _handle_string(const size_t i) {
		if (this->__is_start_string != 'N') {
			if (this->__input_buffer[i] == this->__is_start_string) {
				this->__is_start_string  = 'N';
				this->__current_token += this->__input_buffer[i];
				if (!this->_checkIfNextTokenIsOperatorStart(i)) {
					this->_addToken();
					this->__just_added_token = true;
				}
				return true;
			}
			this->__current_token += this->__input_buffer[i];
			return true;
		}
		else if (this->__input_buffer[i] == '\'' || this->__input_buffer[i] == '"') {
			if (!this->__current_token.empty())
				throw UnexpectedCharacter(this->__filename, this->__last_token, this->__current_token, this->__track_row, this->__track_column);
			this->__is_start_string = this->__input_buffer[i];
		}
		return false;
	}

	inline const bool _handle_paren(const size_t i, char first, char second) {
		if (this->__input_buffer[i] == first &&
		   (this->__is_line_breaker ||
			this->_isIn(this->__last_toke_type, {
			  	TokenType::VARIABLE,
			  	TokenType::CONTROL_FLOW,
			  	TokenType::OPEN_CASES,
				TokenType::ARROW_OPERATOR,
				TokenType::COMMA_OPERATOR,
				TokenType::BITWISE_OPERATOR,
				TokenType::ARITHMETIC_OPERATOR,
				TokenType::ASSIGNMENT_OPERATOR,
				TokenType::RELATIONAL_OPERATOR,
			}))) {
			this->_addToken();
			this->__opening_patt.push_back(second);
			this->__current_token += this->__input_buffer[i];
			this->_addToken();

			this->__currentStruct->next          = new TokenCurrentLink();
			this->__currentStruct->next->current = this->__lastTokStruct;
			this->__currentStruct->next->prev    = this->__currentStruct;
			this->__currentStruct                = this->__currentStruct->next;
			return true;

		} else if (this->__input_buffer[i] == second) {
			this->__just_added_token = false;
			int last_index = this->__opening_patt.size() - 1;
			if (this->__opening_patt.empty() || this->__opening_patt[last_index] != this->__input_buffer[i])
				throw UnexpectedCharacter(this->__filename, this->__last_token, this->__current_token, this->__track_row, this->__track_column);

			this->_addToken();
			if (this->__currentStruct->currTokens.size())
				this->_insertTokenStruct();

			this->__currentStruct       = this->__currentStruct->prev;
			this->__currentStruct->next = nullptr;

			this->__opening_patt.pop_back();
			this->__current_token.clear();

			if (!this->_checkIfNextTokenIsOperatorStart(i, false))
				this->__just_added_token = true;

			this->__is_line_breaker = false;
			this->__last_toke_type  = TokenType::CLOSE_CASES;
			this->__last_token      = second;

			if (!this->__is_in_paren) {
				this->__just_added_token = false;
				this->__candidate_block  = true;
			}
			return true;
		}
		return false;
	}

	inline const bool _addToken() {
		if (!this->__current_token.empty()) {

			if (this->__is_start_string != 'N')
				throw UnterminatedString(this->__filename, this->__last_token, this->__current_token, this->__track_row, this->__track_column);

			TokenType tokenType = this->_determineTokenType(this->__current_token);

			if (this->__candidate_block) {
				if (!this->_isIn(tokenType, {
					TokenType::ARROW_OPERATOR,
					TokenType::COMMA_OPERATOR,
					TokenType::DOT_OPERATOR,
					TokenType::BITWISE_OPERATOR,
					TokenType::ARITHMETIC_OPERATOR,
					TokenType::RELATIONAL_OPERATOR
				})) this->_lineBreak();
			}

			this->__candidate_block = false;
			this->__last_toke_type = tokenType;
			if (tokenType == TokenType::STRING)
				this->__current_token = this->__current_token.substr(1, this->__current_token.size()-2);
			else this->__cache_tokType[this->__current_token] = tokenType;

			// ADD TOKEN
			this->__lastTokStruct = std::make_shared<TokenStruct>();
			this->__lastTokStruct->token = std::make_tuple(this->__current_token, tokenType, this->__current_row, this->__current_column);
			this->__currentStruct->currTokens.push_back(this->__lastTokStruct);

			this->__last_token = this->__current_token;
			this->__current_token.clear();
			this->__just_added_token = (this->_isIn(tokenType, {
				TokenType::VARIABLE,
				TokenType::STRING,
				TokenType::INTEGER,
				TokenType::DECIMAL,
			}));
			this->__is_line_breaker = false;
			return true;
		}
		return false;
	}

	inline const bool _isComment(const size_t i) {
		if (this->__current_token == "|") {
			if (this->__input_buffer[i] == '>') {
				this->__is_comment_line = true;
				return true;
			}
			if (this->__input_buffer[i] == '-') {
				if (this->__input_buffer.size() > i+1) {
					if (this->__input_buffer[i+1] == '>') {
						this->__is_comment_block = true;
						return true;
					}
				}
				throw UnexpectedCharacter(this->__filename, this->__last_token, this->__current_token, this->__track_row, this->__track_column);
			}
		}
		return false;
	}

	inline const void _insertTokenStruct() {
		std::shared_ptr<TokenStruct> newToken = std::make_shared<TokenStruct>();
		newToken->child = this->__currentStruct->currTokens;

		this->__currentStruct->current->child.push_back(newToken);
		this->__currentStruct->currTokens.clear();
	}

	inline const void _lineBreak() {
		this->__just_added_token = false;
		if (!this->__currentStruct->currTokens.empty()) {
			 this->_insertTokenStruct();
		}
		this->__is_line_breaker = true;
	}

	inline const bool _isInParen() {
		for (const auto &v : this->__opening_patt)
			if (v == ')' || v == ']') return true;
		return false;
	}

	inline const void _getTokens() {
		this->__allTokens = std::make_shared<TokenStruct>();

		this->__currentStruct = new TokenCurrentLink();
		this->__currentStruct->current = this->__allTokens;
		this->__currentStruct->currTokens.clear();

		for (size_t i = 0; i < this->__input_buffer.size(); i++) {
			this->__is_in_paren = this->_isInParen();

			if (this->__input_buffer[i] == '\n') {
				this->__track_row++;
			}
			// If current Token is empty, Just keep the current Tracker updated
			if (this->__current_token.empty()) {
				this->__current_column = this->__track_column;
				this->__current_row    = this->__track_row;
			}

			// If the input is line break and it is not a comment, Add the token if there is, and make new currentTokens list
			if (this->__input_buffer[i] == J_LINE_BREAKER && !this->__is_comment_line && !this->__is_comment_block) {
				// Don't allow lin break when parenthesis, use comma or something
				if (this->__is_in_paren)
					throw UnexpectedCharacter(
						this->__filename, this->__last_token,
						this->__current_token, this->__track_row, this->__track_column);

				this->__candidate_block = false;
				this->_addToken();
				if (this->_isIn(this->__last_toke_type, {
					TokenType::CONTROL_FLOW,
					TokenType::OPEN_CASES,
					TokenType::ARROW_OPERATOR,
					TokenType::COMMA_OPERATOR,
					TokenType::DOT_OPERATOR,
					TokenType::BITWISE_OPERATOR,
					TokenType::ARITHMETIC_OPERATOR,
					TokenType::ASSIGNMENT_OPERATOR,
					TokenType::RELATIONAL_OPERATOR,
					TokenType::FUNCTIONS,
					TokenType::DATA_TYPE,
				}))
					throw UnexpectedCharacter(
						this->__filename, this->__last_token,
						this->__current_token, this->__track_row, this->__track_column);

				this->__track_column++;
				this->_lineBreak();
				continue;
			}
			//  Check if it is comment block
			if (this->__is_comment_block) {

				// Add all of in current_token for testing
				if (this->_isIn(this->__input_buffer[i], {'<', '-', '|'})) {
					this->__current_token += this->__input_buffer[i];
				} else this->__current_token.clear();

				if (this->__current_token.size() >= 3) {
					// Means the comment block is ended
					if (this->__current_token == "<-|") {
						this->__is_comment_block = false;
						this->__current_token.clear();
					}
					// Minus the string if it is not, but have pattern
					else if (this->__current_token[1] == '<' && this->__current_token[2] == '-')
						this->__current_token = this->__current_token.substr(1);
					else if (this->__current_token[2] == '<')
						this->__current_token = this->__current_token.substr(2);
					else this->__current_token.clear();
				}
			}

			// Check if it is in ignored character, and if it is not, and also not a comment line, the proceed
			else if (!this->_isNextTokenInIgnored(i) && !this->__is_comment_line) {

				// Check if it will be a comment
				if (this->_isComment(i)) this->__current_token.clear();
				// Check inidividual token that might have a double pattern, example ++ -- eg.
				else if (!this->_checkNextToken(i, "=", {'=', '>', '<'})
					  && !this->_checkNextToken(i, "+", {'+'          })
					  && !this->_checkNextToken(i, "-", {'-', '>'     })
					  && !this->_checkNextToken(i, "*", {'*'          })
					  && !this->_checkNextToken(i, "/", {'/'          })
					  && !this->_checkNextToken(i, "&", {'&'          })
					  && !this->_checkNextToken(i, "|", {'|'          })
					  && !this->_checkNextToken(i, ">", {'=', '>'     })
					  && !this->_checkNextToken(i, "<", {'=', '<'     })
				) {
					// Check if the token is operator, example + - . , eg.
					if (this->__current_token.size() == 1 && this->_isIn(this->__current_token[0], this->__operator_symbol)) {
						if (!(
						   (this->__current_token[0] == '-' ||
						   	this->__current_token[0] == '.') &&
						   (this->__is_line_breaker ||
							this->_isIn(this->__last_toke_type, {
							  	TokenType::CONTROL_FLOW,
							  	TokenType::OPEN_CASES,
								TokenType::ARROW_OPERATOR,
								TokenType::COMMA_OPERATOR,
								TokenType::BITWISE_OPERATOR,
								TokenType::ARITHMETIC_OPERATOR,
								TokenType::ASSIGNMENT_OPERATOR,
								TokenType::RELATIONAL_OPERATOR,
								TokenType::INCREMENT_DECREMENT_OPERATOR,
							})))) {
							this->_addToken();
						}
					}
					// If token is just added, example token added is string, "test" and you add without the space, throw error, example
					// "test"c c is UnexpectedCharacter
					if (this->__just_added_token &&
					   !this->_isIn(this->__input_buffer[i], {')', ']', '}'}) &&
					   !this->_isIn(this->__input_buffer[i], this->__operator_symbol)
					)
						throw UnexpectedCharacter(this->__filename, this->__last_token, this->__current_token, this->__track_row, this->__track_column);

					// Handle the escape '\' character combination
					if (this->_handle_escape(i)) {
						this->__current_token += this->__input_buffer[i];
					}
					// Handle the string, if the token is string
					else if (!this->_handle_string(i)) {
						// Handle the the parenthesis or any cases
						if (!this->_handle_paren(i, '(', ')')
						 && !this->_handle_paren(i, '[', ']')
						 && !this->_handle_paren(i, '{', '}')
						 ) {
							// Proceed as the token is not a string, might be a variable
							this->__current_token += this->__input_buffer[i];
							this->_checkIfNextTokenIsOperatorStart(i);
						}
					}
				}
			} else {
				// If not, then it must be whitespace or any ignored character
				if (this->__input_buffer[i] == '\n') {
					this->__is_comment_line = false;
					this->__track_column = 0;
				}
				// If it is not comment line then add token
				// this->__just_added_token = false;
				if (!this->__is_comment_line) {
					this->_addToken();
				}
			}
			this->__track_column++;
		}
		// Check the last token and token Collections
		if (this->__current_token.size() || this->__currentStruct->currTokens.size()) {
			this->_addToken();
			this->_insertTokenStruct();
		}
	}
};


