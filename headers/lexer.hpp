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

#include "error_handler.hpp"

#define J_LINE_BREAKER     ';'
#define J_INPUT_STREAMS    const std::string
#define J_CTOKENSR         const std::string&
#define J_TOKENS           std::string
#define J_LOG_STREAMS      std::cout

class Tokens {
protected:
	enum TokenType {
		UNDEFINED,
		DATA_TYPE,
		CONTROL_FLOW,
		CUSTOM_KEYWORD,
		FUNCTIONS,
		STRING,
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
		INTEGER,
		DECIMAL,
		VARIABLE
	};

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
		this->__dataTypeVector.push_back("jstring" );
		this->__dataTypeVector.push_back("jdouble" );
		this->__dataTypeVector.push_back("jint"    );
		this->__dataTypeVector.push_back("jboolean");

		// Control Flow
		this->__controlFlowVector.push_back("jif");

		this->__controlFlowVector.push_back("in");
		this->__controlFlowVector.push_back("of");
		this->__controlFlowVector.push_back("as");
		this->__controlFlowVector.push_back("then");

		this->__controlFlowVector.push_back("jfor");
		this->__controlFlowVector.push_back("jwhile");
		this->__controlFlowVector.push_back("jswitch");
		this->__controlFlowVector.push_back("jcase");
		this->__controlFlowVector.push_back("jfunc");
		this->__controlFlowVector.push_back("jreturn");

		this->__keywordVector.push_back("jbreak");
		this->__keywordVector.push_back("jcontinue");
		this->__keywordVector.push_back("jdefault");

		this->__functionsVector.push_back("$log");
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
	bool __is_line_breaker           = false;
	bool __just_added_token          = false;
	bool __just_added_escape         = false;
	bool __is_comment_line           = false;
	bool __is_comment_block          = false;

	Tokens::TokenType __last_toke_type = UNDEFINED;
	typedef std::vector<std::tuple<J_TOKENS, Tokens::TokenType, size_t, size_t>> TokenStruct;
	TokenStruct __currentTokens;
	std::vector<char> __opening_patt = {};
	std::vector<TokenStruct> __allTokens;

public:
	inline Tokenizer(const std::string &filename, J_INPUT_STREAMS &input_buffer)
		: __filename(filename), __input_buffer(input_buffer), Tokens()
	{
		this->_getTokens();
		if (!this->__opening_patt.empty())
			throw UnterminatedCases(this->__filename, this->__last_token, this->__current_token, this->__track_row, this->__track_column);
	}

	inline const void printAllTokens(bool printTokType=true, bool showCoords=false) {
		std::cout << "[\n";
		for (const auto& row : this->__allTokens) {
			std::cout << "  [";
			bool firstToken = true;
			for (const auto& token : row) {
				if (!firstToken) std::cout << ",\n   ";

				std::ostringstream oss;
				oss << std::right << std::setw(5) << std::get<2>(token) << ", ";
				oss << std::right << std::setw(5) << std::get<3>(token);
				std::cout << "[ " << std::left << std::setw(20) << std::get<J_TOKENS>(token) << ": "
						  << std::left << std::setw(30) << (printTokType ? this->_tokenTypeToString(std::get<Tokens::TokenType>(token)) : "")
						  << std::left << std::setw(15) << (showCoords ? (
								"(" + oss.str() + ")") : "")
						  << "]";
				firstToken = false;
			}
			std::cout << "\n  ]\n";
		}
		std::cout << "]\n";
	}

	inline const void saveTokens(const std::string &filename) {
		std::ofstream outputFile(filename);

		if (outputFile.is_open()) {
			outputFile << "[\n";
			for (const auto& row : this->__allTokens) {
				outputFile << "  [";
				bool firstToken = true;
				for (const auto& token : row) {
					if (!firstToken) outputFile << ",\n   ";

					outputFile << "[ " << std::left << std::setw(20) << std::get<J_TOKENS>(token) << ": "
							  << std::left << std::setw(30) << this->_tokenTypeToString(std::get<Tokens::TokenType>(token))
							  << "(" << std::right << std::setw(5) << std::get<2>(token) << ", "
									 << std::right << std::setw(5) << std::get<3>(token) << ")"
							  << "]";
					firstToken = false;
				}
				outputFile << "\n  ]\n";
			}
			outputFile << "]\n";
			outputFile.close();
		}
	}

private:
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

	inline const Tokens::TokenType _determineTokenType(J_CTOKENSR token) const {
		if (this->_checkKeyword(token, this->__dataTypeVector   )) return Tokens::TokenType::DATA_TYPE;
		if (this->_checkKeyword(token, this->__controlFlowVector)) return Tokens::TokenType::CONTROL_FLOW;
		if (this->_checkKeyword(token, this->__keywordVector    )) return Tokens::TokenType::CUSTOM_KEYWORD;
		if (this->_checkKeyword(token, this->__functionsVector  )) return Tokens::TokenType::FUNCTIONS;

		if ((token.size() >= 2 && (token.front() == '"'  && token.back() == '"' ))
		||  (token.size() >= 2 && (token.front() == '\'' && token.back() == '\'')))
			return Tokens::TokenType::STRING;

		// I did not use regex, as using this is faster
		if (token == "->")                   return Tokens::TokenType::ARROW_OPERATOR;
		if (token == "=")                    return Tokens::TokenType::ASSIGNMENT_OPERATOR;
		if (token == ".")                    return Tokens::TokenType::DOT_OPERATOR;
		if (token == ",")                    return Tokens::TokenType::COMMA_OPERATOR;
		if (token == "++" || token == "--")  return Tokens::TokenType::INCREMENT_DECREMENT_OPERATOR;

		if (token == "("  || token == "["  || token == "{")
			return Tokens::TokenType::OPEN_CASES;

		if (token == ")"  || token == "]"  || token == "}")
			return Tokens::TokenType::CLOSE_CASES;

		if (token == "+"  || token == "-"  || token == "*"
		 || token == "/"  || token == "%") 
			return Tokens::TokenType::ARITHMETIC_OPERATOR;
		if (token == "&&" || token == "||" || token == "==" || token == "!=" || token == ">"  || token == "!="
		 || token == "<"  || token == "=>" || token == "<=" || token == ">=" || token == "=<")
			return Tokens::TokenType::RELATIONAL_OPERATOR;
		if (token == "&"  || token == "|"  || token == "^"
		 || token == "~"  || token == "<<" || token == ">>")
			return Tokens::TokenType::BITWISE_OPERATOR;

		// Use regex from complex pattern
		for (const auto& entry : this->__tokenMap) {
        	std::regex pattern(entry.first);
        	if (std::regex_match(token, pattern))
            	return entry.second;
    	}
    	throw UnrecognizeToken(this->__filename, this->__last_token, token, this->__track_row, this->__track_column);
	}

	constexpr inline const char *_tokenTypeToString(const Tokens::TokenType type) const noexcept {
		switch (type) {
			case Tokens::TokenType::DATA_TYPE                    : return "DATA_TYPE";
			case Tokens::TokenType::CONTROL_FLOW                 : return "CONTROL_FLOW";
			case Tokens::TokenType::CUSTOM_KEYWORD               : return "CUSTOM_KEYWORD";
			case Tokens::TokenType::FUNCTIONS                    : return "FUNCTIONS";
			case Tokens::TokenType::STRING                       : return "STRING";
			case Tokens::TokenType::OPEN_CASES                   : return "OPEN_CASES";
			case Tokens::TokenType::CLOSE_CASES                  : return "CLOSE_CASES";
			case Tokens::TokenType::ARROW_OPERATOR               : return "ARROW_OPERATOR";
			case Tokens::TokenType::COMMA_OPERATOR               : return "COMMA_OPERATOR";
			case Tokens::TokenType::DOT_OPERATOR                 : return "DOT_OPERATOR";
			case Tokens::TokenType::INCREMENT_DECREMENT_OPERATOR : return "INCREMENT_DECREMENT_OPERATOR";
			case Tokens::TokenType::ASSIGNMENT_OPERATOR          : return "ASSIGNMENT_OPERATOR";
			case Tokens::TokenType::ARITHMETIC_OPERATOR          : return "ARITHMETIC_OPERATOR";
			case Tokens::TokenType::RELATIONAL_OPERATOR          : return "RELATIONAL_OPERATOR";
			case Tokens::TokenType::BITWISE_OPERATOR             : return "BITWISE_OPERATOR";
			case Tokens::TokenType::INTEGER                      : return "INTEGER";
			case Tokens::TokenType::DECIMAL                      : return "DECIMAL";
			case Tokens::TokenType::VARIABLE                     : return "VARIABLE";
			default                                              : return "UNDEFINED";
		}
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

	inline const bool _checkIfNextTokenIsOperatorStart(const size_t i) noexcept {
		if (this->__input_buffer.size() > i+1) {
			if (this->__input_buffer[i+1] == '.')
				if (std::regex_match(this->__current_token, std::regex("^(-?[0-9]*)$")))
					return true;

			if (!this->_isIn(this->__input_buffer[i],   this->__operator_symbol)
			 &&  this->_isIn(this->__input_buffer[i+1], this->__operator_symbol)) {
				this->_addToken();
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
			  	Tokens::TokenType::VARIABLE,
			  	Tokens::TokenType::CONTROL_FLOW,
			  	Tokens::TokenType::OPEN_CASES,
				Tokens::TokenType::ARROW_OPERATOR,
				Tokens::TokenType::COMMA_OPERATOR,
				Tokens::TokenType::BITWISE_OPERATOR,
				Tokens::TokenType::ARITHMETIC_OPERATOR,
				Tokens::TokenType::ASSIGNMENT_OPERATOR,
				Tokens::TokenType::RELATIONAL_OPERATOR,
			}))) {
			this->_addToken();
			this->__opening_patt.push_back(second);
			this->__current_token += this->__input_buffer[i];
			this->_addToken();
			return true;

		} else if (this->__input_buffer[i] == second) {
			this->__just_added_token = false;
			int last_index = this->__opening_patt.size() - 1;
			if (this->__opening_patt.empty() || this->__opening_patt[last_index] != this->__input_buffer[i])
				throw UnexpectedCharacter(this->__filename, this->__last_token, this->__current_token, this->__track_row, this->__track_column);
			this->_addToken();

			this->__opening_patt.pop_back();
			this->__current_token += this->__input_buffer[i];
			this->_addToken();
			this->__just_added_token = true;
			return true;
		}
		return false;
	}

	inline const bool _addToken() {
		if (!this->__current_token.empty()) {
			if (this->__is_start_string != 'N')
				throw UnterminatedString(this->__filename, this->__last_token, this->__current_token, this->__track_row, this->__track_column);
			TokenType tokenType = this->_determineTokenType(this->__current_token);

			this->__last_toke_type = tokenType;
			if (tokenType == Tokens::TokenType::STRING)
				this->__current_token = this->__current_token.substr(1, this->__current_token.size()-2);

			this->__currentTokens.push_back(
				std::make_tuple(this->__current_token, tokenType, this->__current_row, this->__current_column)
			);
			this->__last_token = this->__current_token;
			this->__current_token.clear();
			this->__just_added_token = false;
			this->__is_line_breaker = false;
			return true;
		}
		this->__just_added_token = false;
		return false;
	}

	inline const bool _isComment(const size_t i) {
		if (this->__current_token == "|") {
			if (this->__input_buffer[i] == '>') {
				this->__is_comment_line = true;
				return true;
			}
			if (this->__input_buffer[i] == '-') {
				// Potential comment block
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

	inline const void _getTokens() {
		this->__allTokens.clear();
		this->__currentTokens.clear();
		for (size_t i = 0; i < this->__input_buffer.size(); i++) {

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
				this->__just_added_token = false;
				this->_addToken();

				if (!this->__currentTokens.empty()) {
					 this->__allTokens.push_back(this->__currentTokens);
					 this->__currentTokens.clear();
				}
				this->__track_column++;
				this->__is_line_breaker = true;
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
						if (this->__current_token[0] != '.' &&
						  !(this->__current_token[0] == '-' &&
						   (this->__is_line_breaker ||
							this->_isIn(this->__last_toke_type, {
							  	Tokens::TokenType::VARIABLE,
							  	Tokens::TokenType::CONTROL_FLOW,
							  	Tokens::TokenType::OPEN_CASES,
								Tokens::TokenType::ARROW_OPERATOR,
								Tokens::TokenType::COMMA_OPERATOR,
								Tokens::TokenType::BITWISE_OPERATOR,
								Tokens::TokenType::ARITHMETIC_OPERATOR,
								Tokens::TokenType::ASSIGNMENT_OPERATOR,
								Tokens::TokenType::RELATIONAL_OPERATOR,
							})))) {
							this->_addToken();
						}
					}
					// If token is just added, example token added is string, "test" and you add without the space, throw error, example
					// "test"c c is UnexpectedCharacter
					if (this->__just_added_token && !this->_isIn(this->__input_buffer[i], {')', ']', '}'}))
						throw UnexpectedCharacter(this->__filename, this->__last_token, this->__current_token, this->__track_row, this->__track_column);

					// Handle the escape '\' character combination
					if (this->_handle_escape(i)) {
						this->__current_token += this->__input_buffer[i];
					}
					// Handle the string, if the token is string
					else if (!this->_handle_string(i)) {
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
				this->__just_added_token = false;
				if (!this->__is_comment_line) {
					this->_addToken();
				}
			}
			this->__track_column++;
		}
		// Check the last token and token Collections
		if (this->__current_token.size() || this->__currentTokens.size()) {
			this->_addToken();
			this->__allTokens.push_back(this->__currentTokens);
			this->__currentTokens.clear();
		}
	}
};


