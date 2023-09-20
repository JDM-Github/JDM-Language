#include "parser.hpp"

JDM_DLL
Parser::Parser(
	const std::shared_ptr<TokenStruct> &tokens)
	:
	__rootTokens(tokens)
{
	this->__mainBlock = this->_getBlock(this->__rootTokens);
	this->analyzeAST(this->__mainBlock);
}


JDM_DLL
const void Parser::analyzeAST(
	const std::shared_ptr<Block> &block,
	const std::string &space)
{
	for (const auto &instruction : block->instruction) {
		if (instruction->getType() == IfStatementInstruction) {
			Log << space << "  " << this->_instructionToString(instruction->getType()) << ": \n";
			auto ifState = std::dynamic_pointer_cast<IfStatement>(instruction);
			this->analyzeAST(std::dynamic_pointer_cast<Block>(ifState->blockWillRun), space+"    ");
			this->_analyzeAllIfElse(ifState->elseIf, space);

		} else if (instruction->getType() == DeclarationInstruction) {
			auto declare = std::dynamic_pointer_cast<Declaration>(instruction);
			Log << space << "  " << this->_instructionToString(instruction->getType()) << ": {\n";
			this->_printExpression(declare->expression, space+"      ");
			Log << space << "  }\n";

		} else if (instruction->getType() == ForLoopStatementInstruction) {
			Log << space << "  " << this->_instructionToString(instruction->getType()) << ": \n";
			auto forLoop = std::dynamic_pointer_cast<ForLoopStatement>(instruction);
			if (forLoop->start) {
				Log << space << "    START: { ... }\n";
				// this->_printExpression(forLoop->start, space+"      ");
			}
			if (forLoop->stop) {
				Log << space << "    STOP { ... }: \n";
				// this->_printExpression(forLoop->stop,  space+"      ");
			}
			if (forLoop->step) {
				Log << space << "    STEP { ... }: \n";
				// this->_printExpression(forLoop->step,  space+"      ");
			}
			this->analyzeAST(std::dynamic_pointer_cast<Block>(forLoop->blockWillRun), space+"    ");

		// CREATE FUNCTION
		} else if (instruction->getType() == CreateFunctionInstruction) {
			Log << space << "  " << this->_instructionToString(instruction->getType()) << ": (\n";
			auto newFunc = std::dynamic_pointer_cast<CreateFunction>(instruction);
			for (int i = 0; i < newFunc->parameters.size(); i++)
				Log << space << "    " << newFunc->parameters[i]->varName->returnStringValue() << '\n';
			Log << space << "  " << ")\n";
			this->analyzeAST(std::dynamic_pointer_cast<Block>(newFunc->blockWillRun), space+"    ");

		// FOREACH LIST
		} else if (instruction->getType() == ForEachListStatementInstruction) {
			Log << space << "  " << this->_instructionToString(instruction->getType()) << ": \n";
			auto forEach = std::dynamic_pointer_cast<ForEachListStatement>(instruction);
			this->analyzeAST(std::dynamic_pointer_cast<Block>(forEach->blockWillRun), space+"    ");

		// FOREACH MAP
		} else if (instruction->getType() == ForEachMapStatementInstruction) {
			Log << space << "  " << this->_instructionToString(instruction->getType()) << ": \n";
			auto forEach = std::dynamic_pointer_cast<ForEachMapStatement>(instruction);
			this->analyzeAST(std::dynamic_pointer_cast<Block>(forEach->blockWillRun), space+"    ");
		}
		else
			Log << space << "   " << this->_instructionToString(instruction->getType()) << ": { ... }\n";
	}
}


/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////

Parser: CREATING LIST OF TOKENS

//////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

JDM_DLL
const std::shared_ptr<Block> Parser::_createBlockOrLine(
	const std::shared_ptr<TokenStruct> &currToken,
	const std::vector<std::shared_ptr<TokenStruct>>& tokens)
{
	std::shared_ptr<Block> blockWillRun = std::make_shared<Block>();
	if (this->_isBlockCurly(currToken)) {
		if (tokens.size() != 1 && !isInVec(std::get<1>(tokens[1]->token), {
			TokenType::DOT_OPERATOR
		}))
			throw std::runtime_error("SYNTAX ERROR: Unexpected Operator at the end of Block");
		blockWillRun = this->_getBlock(currToken);
	}
	else {
		blockWillRun = std::make_shared<Block>();
		this->_predictInstruction(blockWillRun, tokens);
	}
	return blockWillRun;
}

JDM_DLL
const std::shared_ptr<Expression> Parser::_createListExpression(
	const std::vector<std::shared_ptr<TokenStruct>>& tokens)
{
	auto newExpression = std::make_shared<Expression>();

	std::vector<std::shared_ptr<Expression>> listVec;
	this->_setArguments(listVec, tokens);

	newExpression->firstValue = std::make_shared<ListObject>(listVec);
	return newExpression;
}

JDM_DLL
const std::shared_ptr<Expression> Parser::_createMapExpression(
	const std::vector<std::shared_ptr<TokenStruct>>& tokens)
{
	auto newExpression = std::make_shared<Expression>();

	std::vector<std::shared_ptr<MapStruct>> mapVec;
	this->_setMapArguments(mapVec, tokens);

	newExpression->firstValue = std::make_shared<MapObject>(mapVec);
	return newExpression;
}

/**
 * Set Variable Arguments - Useful for extracting VARIABLE tokens without expressions.
 *
 * This function is designed to extract and set VARIABLE tokens from a vector of TokenStructs.
 * It iterates through the tokens and adds VARIABLE tokens to the provided vector while ignoring comma operators.
 * This function is handy in scenarios like foreach loops over a Map, where only variable names are needed.
 *
 * @param vec A reference to a vector of shared pointers to VariableObjects where the extracted VARIABLE tokens will be stored.
 * @param trav A vector of TokenStructs representing the tokens to be processed.
 * @throws std::runtime_error if a token other than VARIABLE is encountered.
 * @return None (void).
 */
JDM_DLL
const void Parser::_setVarArgs(
	std::vector<std::shared_ptr<VariableObjects>> &vec,
	const std::vector<std::shared_ptr<TokenStruct>> &trav)
{
	for (const auto &tok : trav) {
		if (std::get<1>(tok->token) == TokenType::COMMA_OPERATOR) continue;
		if (std::get<1>(tok->token) == TokenType::VARIABLE) vec.push_back(std::make_shared<VariableObjects>(tok));
		else throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE");
	}
}

/**
 * Set Arguments - Used for extracting expressions representing function call arguments.
 *
 * This function is designed to extract and set expressions representing function call arguments.
 * It processes a vector of TokenStructs representing the tokens within the argument list.
 * It recursively separates the arguments based on comma operators and constructs Expression objects.
 *
 * @param vec A reference to a vector of shared pointers to Expression objects where the extracted argument expressions will be stored.
 * @param trav A vector of TokenStructs representing the tokens within the argument list.
 * @return None (void).
 */
JDM_DLL
const void Parser::_setArguments(
	std::vector<std::shared_ptr<Expression>> &vec,
	const std::vector<std::shared_ptr<TokenStruct>> &trav)
{
	if (trav.empty()) return;
	auto it = std::find_if(trav.begin(), trav.end(), [](const std::shared_ptr<TokenStruct>& tok) {
		return std::get<1>(tok->token) == TokenType::COMMA_OPERATOR; });

	if (it != trav.end()) {
		vec.push_back(this->_createExpression(this->_transformTokenStruct({ trav.begin(), it }))[0]->expression);
		if (it+1 != trav.end()) _setArguments(vec, { it+1, trav.end() });
	} else vec.push_back(this->_createExpression(this->_transformTokenStruct(trav))[0]->expression);
}


/**
 * Create MAP Values for Function Arguments.
 *
 * This function is used to parse and create MAP values for function arguments.
 * It processes a vector of TokenStructs representing tokens related to function arguments.
 * It recursively separates the arguments based on comma operators and constructs MapStruct objects.
 *
 * @param vec A reference to a vector of shared pointers to MapStruct objects where the parsed MAP values will be stored.
 * @param trav A vector of TokenStructs representing the tokens related to function arguments.
 * @return None (void).
 */
JDM_DLL
const void Parser::_setMapArguments(
	std::vector<std::shared_ptr<MapStruct>> &vec,
	const std::vector<std::shared_ptr<TokenStruct>> &trav)
{
	if (trav.empty()) return;
	auto it = std::find_if(trav.begin(), trav.end(), [](const std::shared_ptr<TokenStruct>& tok) {
		return std::get<1>(tok->token) == TokenType::COMMA_OPERATOR; });

	if (it != trav.end()) {
		vec.push_back(this->_createMap({ trav.begin(), it }));
		if (it+1 != trav.end()) _setMapArguments(vec, { it+1, trav.end() });
	} else vec.push_back(this->_createMap( trav ));
}

/**
 * Set Data Type and Variable Arguments for Function Parameters.
 *
 * This function is used to parse and set data type and variable arguments for function parameters.
 * It processes a vector of TokenStructs representing tokens related to function parameter declarations.
 * It recursively separates the arguments based on comma operators and constructs Variable objects.
 *
 * @param vec A reference to a vector of shared pointers to Variable objects where the parsed arguments will be stored.
 * @param trav A vector of TokenStructs representing the tokens related to function parameter.
 * @return None (void).
 */
JDM_DLL
const void Parser::_setTypeVarArgs(
	std::vector<std::shared_ptr<Variable>> &vec,
	const std::vector<std::shared_ptr<TokenStruct>> &trav)
{
	if (trav.empty()) return;
	auto it = std::find_if(trav.begin(), trav.end(), [](const std::shared_ptr<TokenStruct>& tok) {
		return std::get<1>(tok->token) == TokenType::COMMA_OPERATOR; });

	if (it != trav.end()) {
		vec.push_back(this->_createVariable( { trav.begin(), it } ));
		if (it+1 != trav.end()) _setTypeVarArgs(vec, { it+1, trav.end() });
	} else vec.push_back(this->_createVariable( trav ));
}
