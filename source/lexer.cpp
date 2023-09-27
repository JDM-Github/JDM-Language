#include "lexer.hpp"

JDM_DLL
Tokenizer::Tokenizer(
	FileStream &filename,
	InputStream &input_buffer)
	:
	__filename(filename),
	__input_buffer(input_buffer),
	Tokens()
{
	this->__last_token        = "";
	this->__current_token     = "";
	this->__current_column    = 0;
	this->__current_row       = 0;
	this->__track_column      = 0;
	this->__track_row         = 1;
	this->__is_start_string   = 'N';
	this->__is_line_breaker   = true;
	this->__just_added_token  = false;
	this->__just_added_escape = false;
	this->__is_comment_line   = false;
	this->__is_comment_block  = false;
	this->__is_in_paren       = false;
	this->__candidate_block   = false;
	this->__last_toke_type    = TokenType::UNDEFINED;
	this->__currentStruct     = MSharedTokenLink();

	this->_getTokens();
	if (!this->__opening_patt.empty()) {
		throw UnterminatedCases(
			this->__filename,
			this->__last_token,
			this->__current_token,
			this->__track_row,
			this->__track_column
		);
	}
}

JDM_DLL
CVoid Tokenizer::analyzeAllTokens(
	CBool willPrint)
{
	this->__stringStream.str("");
	this->__stringStream << "{\n";
	for (const auto &tokenStruct : this->__allTokens->child) {
		this->__stringStream << "  [\n";
		for (const auto& token : tokenStruct->child)
			this->_traverseTokenStruct(token, "    ");
		this->__stringStream << "  ],\n";
	}
	this->__stringStream << "}\n";
	if (willPrint) Log << this->__stringStream.str() << '\n';
}

JDM_DLL
CVoid Tokenizer::saveTokens(
	FileStream &filename)
{
	std::ofstream outputFile(filename);
	if (this->__stringStream.str().size())
		this->analyzeAllTokens();

	outputFile << this->__stringStream.str();
	outputFile.close();
}


/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////

Tokenizer Private Class Method

//////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

JDM_DLL
CSharedTokenStruct Tokenizer::getTokens() { return this->__allTokens; }

/**
 * @brief Traverses the token structure recursively and performs a specific task.
 *
 * @param curr The current token structure being traversed.
 * @param space A string representing the current spacing or indentation level.
 */
JDM_DLL
CVoid Tokenizer::_traverseTokenStruct(
	CSharedTokenStructRef curr,
	SpaceString space)
{
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

/**
 * @brief Checks if a given input token matches any keyword tokens in a vector.
 *
 * @param input The input token to check.
 * @param tokens A vector of keyword tokens to compare against.
 * @return True if the input token is a keyword; otherwise, false.
 */
JDM_DLL
CBool Tokenizer::_checkKeyword(
	CTokenStrRef input,
	CVecTokenStrRef tokens)
{
	for (const auto& keyword : tokens)
		if (input.compare(keyword) == 0)
			return true;
	return false;
}

/**
 * @brief Checks if the next token at the specified index is in the list of ignored tokens.
 *
 * @param _index The index of the token to check.
 * @return True if the next token is in the list of ignored tokens; otherwise, false.
 */
JDM_DLL
CBool Tokenizer::_isNextTokenInIgnored(
	SizeT _index) { return isInVec(this->__input_buffer[_index], this->__ignored_keywords); }

/**
 * @brief Determines the token type of the given input token.
 *
 * @param token The input token to analyze.
 * @return The TokenType corresponding to the input token.
 */
JDM_DLL
CTokenType Tokenizer::_determineTokenType(
	CTokenStrRef token)
{
	if (this->__cache_tokType.count(token) > 0)                return this->__cache_tokType.at(token);
	if (this->_checkKeyword(token, this->__dataTypeVector   )) return TokenType::DATA_TYPE;
	if (this->_checkKeyword(token, this->__controlFlowVector)) return TokenType::CONTROL_FLOW;
	if (this->_checkKeyword(token, this->__keywordVector    )) return TokenType::CUSTOM_KEYWORD;
	if (this->_checkKeyword(token, this->__functionsVector  )) return TokenType::CUSTOM_FUNCTIONS;

	if ((token.size() >= 2 && (token.front() == '"'  && token.back() == '"' ))
	||  (token.size() >= 2 && (token.front() == '\'' && token.back() == '\'')))
		return TokenType::STRING;

	if (token == "->" || token == "=>")  return TokenType::ARROW_OPERATOR;
	if (token == "=")                    return TokenType::ASSIGNMENT_OPERATOR;
	if (token == ".")                    return TokenType::DOT_OPERATOR;
	if (token == ",")                    return TokenType::COMMA_OPERATOR;
	if (token == "++" || token == "--")  return TokenType::INCREMENT_DECREMENT_OPERATOR;

	if (token == "("  || token == "["  || token == "{") return TokenType::OPEN_CASES;
	if (token == ")"  || token == "]"  || token == "}") return TokenType::CLOSE_CASES;

	if (token == "+"  || token == "-"  || token == "*" || token == "**"
	 || token == "/"  || token == "%"  || token == "//")
		return TokenType::ARITHMETIC_OPERATOR;
	if (token == "&&" || token == "||" || token == "==" || token == "!=" || token == "!"
	 || token == "<"  || token == "<=" || token == ">=" || token == ">" )
		return TokenType::RELATIONAL_OPERATOR;
	if (token == "&"  || token == "|"  || token == "^"
	 || token == "~"  || token == "<<" || token == ">>")
		return TokenType::BITWISE_OPERATOR;

	for (const auto& entry : this->__tokenMap) {
		std::regex pattern(entry.first);
		if (std::regex_match(token, pattern))
			return entry.second;
	}
	throw UnrecognizeToken(this->__filename, this->__last_token, token, this->__track_row, this->__track_column);
}

/**
 * @brief Checks if the next token at the specified index matches a given string.
 *
 * @param i The index of the Character to check.
 * @param compStr The string to compare with the next token.
 * @param vecCharacters A vector of characters representing the input text.
 * @return True if the next token matches the specified string; otherwise, false.
 */
JDM_DLL
CBool Tokenizer::_checkNextToken(
	CSizeT i,
	CTokenStrRef compStr,
	CVecCharRef vecCharacters)
{
	// Mainly use for checking double instance of Token.
	// Example ++, --, etc
	if (this->__current_token.compare(compStr) == 0) {
		this->__just_added_token = false;

		if (isInVec(this->__input_buffer[i], vecCharacters)) {
			this->__current_token += this->__input_buffer[i];
			this->_addToken();
			return true;
		}
	}
	return false;
}

/**
 * @brief Checks if the given token is an operator token.
 *
 * @param token The input token to check.
 * @return True if the token is an operator; otherwise, false.
 */
JDM_DLL
CBool Tokenizer::_checkIfTokenIsOperator(
	CTokenStrRef token)
{
	return (token == "==" || token == "++" || token == "--" || token == "**" || token == "//"
		||  token == "%"  || token == "&&" || token == "||" || token == "=>" || token == "<="
		||  token == ">=" || token == "=<" || token == "<<" || token == ">>" || token == "^"
		||  token == "~"  || token == ".");
}

/**
 * @brief Checks if the next token at the specified index is the start of an operator.
 *
 * @param i The index of the Character to check.
 * @param willAdd Indicates whether the operator token should be added.
 * @return True if the next token is the start of an operator; otherwise, false.
 */
JDM_DLL
CBool Tokenizer::_checkIfNextTokenIsOperatorStart(
	CSizeT i,
	CBool willAdd)
{
	if (this->__input_buffer.size() > i+1) {
		if (isInVec(this->__input_buffer[i+1], {'(', '[', '{'})) {
			if (willAdd) this->_addToken();
			this->__just_added_token = false;
			return true;
		}

		if (this->__input_buffer[i+1] == '.')
			if (std::regex_match(this->__current_token, std::regex("^(-?[0-9]*)$")))
				return true;

		if (!isInVec(this->__input_buffer[i],   this->__operator_symbol)
			&&  isInVec(this->__input_buffer[i+1], this->__operator_symbol)) {
			if (willAdd) this->_addToken();
			this->__just_added_token = false;
			return true;
		}
	}
	return false;
}

/**
 * @brief Handles the escape sequence at the specified index.
 *
 * @param i The index of the character representing the escape sequence.
 * @return True if the escape sequence is successfully handled; otherwise, false.
 */
JDM_DLL
CBool Tokenizer::_handle_escape(
	CSizeT i)
{
	if (this->__just_added_escape) {
		// If just added escape then not in escape combination. throw error
		// Example: /q is not a valid escape sequence
		if (!isInVec(this->__input_buffer[i], this->__escape_combination)) 
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


/**
 * @brief Handles a string token starting from the specified index.
 *
 * @param i The index of the character representing the start of the string token.
 * @return True if the string token is successfully handled; otherwise, false.
 */
JDM_DLL
CBool Tokenizer::_handle_string(
	CSizeT i)
{
	if (this->__is_start_string != 'N') {
		if (this->__input_buffer[i] == this->__is_start_string) {
			this->__is_start_string = 'N';
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
		if (!this->__current_token.empty()) {
			throw UnexpectedCharacter(this->__filename, this->__last_token, this->__current_token, this->__track_row, this->__track_column);
		}
		this->__is_start_string = this->__input_buffer[i];
		this->__current_token  += this->__input_buffer[i];
		return true;
	}
	return false;
}

/**
 * @brief Handles parentheses tokens starting from the specified index.
 *
 * @param i The index of the character representing the start of the parentheses.
 * @param first The first character of the parentheses ('(').
 * @param second The second character of the parentheses (')').
 * @return True if the parentheses tokens are successfully handled; otherwise, false.
 */
JDM_DLL
CBool Tokenizer::_handle_paren(
	CSizeT i,
	char first,
	char second)
{
	if (this->__input_buffer[i] == first &&
	   (this->__is_line_breaker ||
		isInVec(this->__last_toke_type, {
			TokenType::DATA_TYPE,
			TokenType::VARIABLE,
			TokenType::CONTROL_FLOW,
			TokenType::OPEN_CASES,
			TokenType::CLOSE_CASES,
			TokenType::ARROW_OPERATOR,
			TokenType::COMMA_OPERATOR,
			TokenType::BITWISE_OPERATOR,
			TokenType::ARITHMETIC_OPERATOR,
			TokenType::ASSIGNMENT_OPERATOR,
			TokenType::RELATIONAL_OPERATOR,
		}))) {
		this->_addToken(); // Add the last Token

		this->__opening_patt.push_back(second);
		this->__current_token += this->__input_buffer[i];
		this->_addToken(); // Add the Open Cases

		this->__currentStruct->next          = MSharedTokenLink();
		this->__currentStruct->next->current = this->__lastTokStruct;
		this->__currentStruct->next->prev    = this->__currentStruct;
		this->__currentStruct                = this->__currentStruct->next;
		return true;

	} else if (this->__input_buffer[i] == second) {

		this->__just_added_token = false;
		int last_index           = this->__opening_patt.size() - 1;

		if (this->__opening_patt.empty() || this->__opening_patt[last_index] != this->__input_buffer[i])
			throw UnexpectedCharacter(this->__filename, this->__last_token, this->__current_token, this->__track_row, this->__track_column);
 
		this->_addToken(); // Add the last token
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

/**
 * @brief Adds a token to the token list.
 *
 * @return True if the token is successfully added; otherwise, false.
 */
JDM_DLL
CBool Tokenizer::_addToken()
{
	if (!this->__current_token.empty()) {
		if (this->__is_start_string != 'N')
			throw UnterminatedString(this->__filename, this->__last_token, this->__current_token, this->__track_row, this->__track_column);

		TokenType tokenType = this->_determineTokenType(this->__current_token);

		if (this->__candidate_block) {
			if (!isInVec(tokenType, {
				TokenType::ASSIGNMENT_OPERATOR,
				TokenType::ARROW_OPERATOR,
				TokenType::COMMA_OPERATOR,
				TokenType::DOT_OPERATOR,
				TokenType::BITWISE_OPERATOR,
				TokenType::ARITHMETIC_OPERATOR,
				TokenType::RELATIONAL_OPERATOR
			})) this->_lineBreak();
		}
		this->__candidate_block = false;
		this->__last_toke_type  = tokenType;

		if (tokenType == TokenType::STRING)
			this->__current_token = this->__current_token.substr(1, this->__current_token.size()-2);
		else
			this->__cache_tokType[this->__current_token] = tokenType;

		// ADD TOKEN
		this->__lastTokStruct        = MSharedTokenStruct();
		this->__lastTokStruct->token = std::make_tuple(this->__current_token, tokenType, this->__current_row, this->__current_column);
		this->__currentStruct->currTokens.push_back(this->__lastTokStruct);
		this->__last_token = this->__current_token;

		this->__current_token.clear();
		this->__just_added_token = (
			isInVec(tokenType, {
				TokenType::VARIABLE,
				TokenType::STRING,
				TokenType::INTEGER,
				TokenType::DECIMAL,
			})
		);
		this->__is_line_breaker = false;
		return true;
	}
	return false;
}

/**
 * @brief Checks if the character at the specified index is the start of a comment.
 *
 * @param i The index of the character to check.
 * @return True if the character represents the start of a comment; otherwise, false.
 */
JDM_DLL
CBool Tokenizer::_isComment(
	CSizeT i)
{
	if (this->__current_token == "|") {

		if (this->__input_buffer[i] == '>') {
			this->__is_comment_line = true;
			return true;
		}
		if (this->__input_buffer[i] == '-') {
			if (this->__input_buffer.size() > i+1)
				if (this->__input_buffer[i+1] == '>') {
					this->__is_comment_block = true;
					return true;
				}
			throw UnexpectedCharacter(this->__filename, this->__last_token, this->__current_token, this->__track_row, this->__track_column);
		}
	}
	return false;
}

/**
 * @brief Inserts a token structure into the list of tokens.
 */
JDM_DLL
CVoid Tokenizer::_insertTokenStruct()
{
	SharedTokenStruct newToken = MSharedTokenStruct();
	newToken->child            = this->__currentStruct->currTokens;

	this->__currentStruct->current->child.push_back(newToken);
	this->__currentStruct->currTokens.clear();
}

/**
 * @brief Handles a line break character.
 */
JDM_DLL
CVoid Tokenizer::_lineBreak()
{
	this->__just_added_token = false;
	if (!this->__currentStruct->currTokens.empty())
		this->_insertTokenStruct();
	this->__is_line_breaker = true;
}

/**
 * @brief Checks if the tokenizer is currently inside a set of parentheses.
 *
 * @return True if inside parentheses; otherwise, false.
 */
JDM_DLL
CBool Tokenizer::_isInParen()
{
	for (const auto &v : this->__opening_patt)
		if (v == ')' || v == ']')
			return true;
	return false;
}

/**
 * @brief Main function to tokenize the input text.
 */
JDM_DLL
CVoid Tokenizer::_getTokens()
{
	this->__allTokens             = MSharedTokenStruct();

	this->__currentStruct          = MSharedTokenLink();
	this->__currentStruct->current = this->__allTokens;
	this->__currentStruct->currTokens.clear();

	for (SizeT i = 0; i < this->__input_buffer.size(); i++) {
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
		if (this->__input_buffer[i] == EndLine && !this->__is_comment_line && !this->__is_comment_block) {
			// Don't allow lin break when parenthesis, use comma or something
			if (this->__is_in_paren)
				throw UnexpectedCharacter(
					this->__filename, this->__last_token,
					this->__current_token, this->__track_row, this->__track_column);

			this->__candidate_block = false;
			this->_addToken();
			if (isInVec(this->__last_toke_type, {
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
			})) {
				throw UnexpectedCharacter(
					this->__filename, this->__last_token,
					this->__current_token, this->__track_row, this->__track_column);
			}

			this->__track_column++;
			this->_lineBreak();
			continue;
		}
		//  Check if it is comment block
		if (this->__is_comment_block) {

			// Add all of in current_token for testing
			if (isInVec(this->__input_buffer[i], {'<', '-', '|'}))
				this->__current_token += this->__input_buffer[i];
			else
				this->__current_token.clear();

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
				else
					this->__current_token.clear();
			}
		}
		else if (this->__is_start_string != 'N') {
			// Handle the escape '\' character combination
			if (this->_handle_escape(i))
				this->__current_token += this->__input_buffer[i];
			else this->_handle_string(i);
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
				  && !this->_checkNextToken(i, "!", {'=',         })
			) {
				// Check if the token is operator, example + - . , eg.
				if (this->__current_token.size() == 1 && isInVec(this->__current_token[0], this->__operator_symbol)) {
					if (!(
					   (this->__current_token[0] == '-' ||
						this->__current_token[0] == '.') &&
					   (this->__is_line_breaker ||
						isInVec(this->__last_toke_type, {
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
				   !isInVec(this->__input_buffer[i], {'(', '[', '{', ')', ']', '}'}) &&
				   !isInVec(this->__input_buffer[i], this->__operator_symbol)
				)
					throw UnexpectedCharacter(this->__filename, this->__last_token, this->__current_token, this->__track_row, this->__track_column);

				// Handle the escape '\' character combination
				if (this->_handle_escape(i))
					this->__current_token += this->__input_buffer[i];

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
			if (!this->__is_comment_line)
				this->_addToken();
		}
		this->__track_column++;
	}
	// Check the last token and token Collections
	if (this->__current_token.size() || this->__currentStruct->currTokens.size()) {
		this->_addToken();
		this->_insertTokenStruct();
	}
}
