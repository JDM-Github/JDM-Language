/**

Creator: JDM
MIT License

Copyright (c) 2023 JDM

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#pragma once
#include "utils/Setup.hpp"
#include "utils/ErrorHandler.hpp"

/**
 * Creator: JDM
 *
 * Tokens Class:
 * - This class is used to represent a collection of tokens and their properties.
 * - It provides access to various token-related data, including ignored keywords,
 *   escape combinations, operator symbols, and token mappings.
 */
class JDM_DLL Tokens
{
protected:
	CVecChar    __ignored_keywords  ;
	CVecChar    __escape_combination;
	CVecChar    __operator_symbol   ;
	CMapTokens  __tokenMap          ;
	VecTokenStr __dataTypeVector    ;
	VecTokenStr __controlFlowVector ;
	VecTokenStr __keywordVector     ;
	VecTokenStr __functionsVector   ;
	Tokens()                        ;
};

/**
 * Creator: JDM
 * 
 * - The Tokenizer class is responsible for breaking input text into tokens.
 * - This class extends the functionality provided by the Tokens class and specializes
 *   in tokenization tasks. It uses the token mappings and properties defined in Tokens
 *   to identify and categorize tokens in the input text.
 */
class JDM_DLL Tokenizer : public Tokens
{
public:
	/**
     * @brief Constructor for the Tokenizer class.
     *
     * Initializes a Tokenizer instance with the token mappings and properties
     * from the base Tokens class.
     *
     * @param filename Reference to the FileStream for input file.
     * @param input_buffer Reference to the InputStream for input text.
     */
	JDM_DLL inline Tokenizer() {}
	JDM_DLL Tokenizer(FileStream &filename, InputStream &input_buffer);
	inline ~Tokenizer() {}
	JDM_DLL CVoid tokenize();

	/**
     * @brief Analyzes all tokens in the input text and optionally prints the results.
     *
     * @param willPrint A boolean indicating whether to print the token analysis results.
     */
	JDM_DLL CVoid              analyzeAllTokens (CBool willPrint = false);

	/**
     * @brief Saves the token analysis results to a file.
     *
     * @param filename Reference to the FileStream where token analysis results will be saved.
     */
	JDM_DLL CVoid              saveTokens       (FileStream &filename, const bool autoOverwrite=false);
	JDM_DLL CVoid              loadTokens       (FileStream &filename);

	/**
     * @brief Gets the token analysis results.
     *
     * @return A shared pointer to a data structure containing token analysis results.
     */
	JDM_DLL CSharedTokenStructRef getTokens        (                       );

private:
	JDM_DLL CVoid       _traverseTokenStruct             (CSharedTokenStructRef curr, SpaceString space            );
	JDM_DLL CBool       _checkKeyword                    (CTokenStrRef input, CVecTokenStrRef tokens               );
	JDM_DLL CBool       _isNextTokenInIgnored            (SizeT _index                                             );
	JDM_DLL CTokenType  _determineTokenType              (CTokenStrRef token                                       );

	JDM_DLL CBool       _checkNextToken                  (CSizeT i, CTokenStrRef compStr, CVecCharRef vecCharacters);
	JDM_DLL CBool       _checkIfNextTokenIsOperatorStart (CSizeT i, CBool willAdd = true                           );

	JDM_DLL CBool       _handle_escape                   (CSizeT i                                                 );
	JDM_DLL CBool       _handle_string                   (CSizeT i                                                 );
	JDM_DLL CBool       _handle_paren                    (CSizeT i, char first, char second                        );
	JDM_DLL CBool       _addToken                        (                                                         );

	JDM_DLL CBool       _isComment                       (CSizeT i                                                 );
	JDM_DLL CVoid       _insertTokenStruct               (                                                         );
	JDM_DLL CVoid       _lineBreak                       (                                                         );
	JDM_DLL CBool       _isInParen                       (                                                         );

	// Main Function to get Tokens
	JDM_DLL CVoid       _getTokens                       (                                                         );

private:
	InputStream        __input_buffer     ;
	FileStream         __filename         ;

	TokenStr           __last_token       ;
	TokenStr           __current_token    ;
	SizeT              __current_column   ;
	SizeT              __current_row      ;
	SizeT              __track_column     ;
	SizeT              __track_row        ;
	char               __is_start_string  ;
	bool               __is_line_breaker  ;
	bool               __just_added_token ;
	bool               __just_added_escape;
	bool               __is_comment_line  ;
	bool               __is_comment_block ;
	bool               __is_in_paren      ;
	bool               __candidate_block  ;

	std::ostringstream __stringStream     ;
	JDM::TokenType     __last_toke_type   ;
	SharedTokenStruct  __allTokens        ;
	SharedTokenStruct  __lastTokStruct    ;
	SharedTokenLink    __currentStruct    ;
	MapTokens          __cache_tokType    ;
	VecChar            __opening_patt     ;
};


