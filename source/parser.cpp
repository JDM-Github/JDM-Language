#include "parser.hpp"

JDM_DLL
Parser::Parser(
	const std::shared_ptr<TokenStruct> &tokens)
	:
	__rootTokens(tokens)
{
	this->__mainBlock = this->_getBlock(this->__rootTokens);
	// this->analyzeAST(this->__mainBlock);
}

JDM_DLL
const void Parser::analyzeAST(
	const std::shared_ptr<Block> &block,
	const std::string &space)
{
	for (const auto &instruction : block->instruction)
	{
		if (instruction->getType() == IfStatementInstruction)
		{
			Log << space << "  " << this->_instructionToString(instruction->getType()) << ": \n";
			auto ifState = std::dynamic_pointer_cast<IfStatement>(instruction);
			this->analyzeAST(std::dynamic_pointer_cast<Block>(ifState->blockWillRun), space+"    ");
			this->_analyzeAllIfElse(ifState->elseIf, space);

		}
		else if (instruction->getType() == DeclarationInstruction)
		{
			auto declare = std::dynamic_pointer_cast<Declaration>(instruction);
			Log << space << "  " << this->_instructionToString(instruction->getType()) << ": {\n";
			this->_printExpression(declare->expression, space+"      ");
			Log << space << "  }\n";

		}
		else if (instruction->getType() == ForLoopStatementInstruction)
		{
			Log << space << "  " << this->_instructionToString(instruction->getType()) << ": \n";
			auto forLoop = std::dynamic_pointer_cast<ForLoopStatement>(instruction);
			if (forLoop->start)
			{
				Log << space << "    START: { ... }\n";
				// this->_printExpression(forLoop->start, space+"      ");
			}
			if (forLoop->stop)
			{
				Log << space << "    STOP { ... }: \n";
				// this->_printExpression(forLoop->stop,  space+"      ");
			}
			if (forLoop->step)
			{
				Log << space << "    STEP { ... }: \n";
				// this->_printExpression(forLoop->step,  space+"      ");
			}
			this->analyzeAST(std::dynamic_pointer_cast<Block>(forLoop->blockWillRun), space+"    ");

		// CREATE FUNCTION
		}
		else if (instruction->getType() == CreateFunctionInstruction)
		{
			Log << space << "  " << this->_instructionToString(instruction->getType()) << ": (\n";
			auto newFunc = std::dynamic_pointer_cast<CreateFunction>(instruction);
			for (int i = 0; i < newFunc->parameters.size(); i++)
				Log << space << "    " << newFunc->parameters[i]->varName->returnStringValue() << '\n';
			Log << space << "  " << ")\n";
			this->analyzeAST(std::dynamic_pointer_cast<Block>(newFunc->blockWillRun), space+"    ");

		// FOREACH LIST
		}
		else if (instruction->getType() == ForEachListStatementInstruction)
		{
			Log << space << "  " << this->_instructionToString(instruction->getType()) << ": \n";
			auto forEach = std::dynamic_pointer_cast<ForEachListStatement>(instruction);
			this->analyzeAST(std::dynamic_pointer_cast<Block>(forEach->blockWillRun), space+"    ");

		// FOREACH MAP
		}
		else if (instruction->getType() == ForEachMapStatementInstruction)
		{
			Log << space << "  " << this->_instructionToString(instruction->getType()) << ": \n";
			auto forEach = std::dynamic_pointer_cast<ForEachMapStatement>(instruction);
			this->analyzeAST(std::dynamic_pointer_cast<Block>(forEach->blockWillRun), space+"    ");
		}
		else
			Log << space << "  " << this->_instructionToString(instruction->getType()) << ": { ... }\n";
	}
}

/**
 * 
 * DEBUGGING PURPOSE
 * 
 */
JDM_DLL
const void Parser::_analyzeAllIfElse(
	const std::shared_ptr<IfStatement> &instruction,
	const std::string &space)
{
	if (instruction == nullptr) return;
	std::string typeStr = (instruction->condition == nullptr) ? "ELSE" : "ELSE IF";
	Log << space << "  " << typeStr << ": \n";
	this->analyzeAST(std::dynamic_pointer_cast<Block>(instruction->blockWillRun), space+"    ");
	this->_analyzeAllIfElse(instruction->elseIf, space);
}

JDM_DLL
const char *Parser::_instructionToString(InstructionType instruction)
{
	switch (instruction) {
		case DeclarationInstruction          : return "DECLARATION";
		case CallInstruction                 : return "ASSIGN-OR-CALL";
		case AssignmentInstruction           : return "ASSIGNMENT";
		case IfStatementInstruction          : return "IF";
		case ForLoopStatementInstruction     : return "FOR";
		case WhileStatementInstruction       : return "WHILE";
		case ForEachListStatementInstruction : return "FOREACH-LIST";
		case ForEachMapStatementInstruction  : return "FOREACH-MAP";
		case CreateFunctionInstruction       : return "CREATE FUNCTION";
	}
	return "Invalid Instruction";
}

JDM_DLL
const void Parser::_printExpression(
	const std::shared_ptr<Expression>& expr,
	const std::string &space,
	int depth)
{
	if (!expr) return;
	if (depth == 0) Log << space << "EXPRESSION: {\n";

	if (expr->firstValue) {
		Log << space << "  Value 1: " << expr->firstValue->returnStringValue() << '\n';
	} else {
		Log << space << "  Value in Paren 1: {\n";
		this->_printExpression(expr->firstExpression, space+"  ", depth+1);
		Log << space << "  }\n";
	}

	if (expr->opWillUse) {
		Log << space << "  Operator: " << std::get<0>(expr->opWillUse->token) << '\n';
		if (expr->secondValue) {
			Log << space << "  Value 2: " << expr->secondValue->returnStringValue() << '\n';
		} else if (expr->secondExpression) {
			Log << space << "  Value in Paren 2: {\n";
			this->_printExpression(expr->secondExpression, space+"  ", depth+1);
			Log << space << "  }\n";
		}
	}
	if (depth == 0) Log << space << "}\n";
}

JDM_DLL
const std::shared_ptr<Block> Parser::_getBlock(
	const std::shared_ptr<TokenStruct>& tokenS)
{
	if (this->__currIfLink == nullptr)
		this->__currIfLink = std::make_shared<IfStatementLink>();
	else {
		this->__currIfLink->next = std::make_shared<IfStatementLink>();
		this->__currIfLink->next->prev = this->__currIfLink;
		this->__currIfLink = this->__currIfLink->next;
	}

	std::shared_ptr<Block> block = std::make_shared<Block>();
	for (const auto& tokenStruct : tokenS->child)
		this->_predictInstruction(block, tokenStruct->child);

	if (this->__currIfLink->prev != nullptr) {
		this->__currIfLink = this->__currIfLink->prev;
		this->__currIfLink->next = nullptr;
	}
	return block;
}

JDM_DLL
const void Parser::_predictInstruction(
	const std::shared_ptr<Block> &block,
	const std::vector<std::shared_ptr<TokenStruct>>& tokens)
{
	JDM::TokenType tkType = std::get<1>(tokens[0]->token);
	if (tkType == JDM::TokenType::DATA_TYPE)
	{
		DataTypeEnum dataType = JDM::dataTypeMap.at(std::get<0>(tokens[0]->token));

		if (dataType == DataTypeEnum::DATA_CONST)
		{
			if (tokens.size() < 1 || std::get<1>(tokens[1]->token) != JDM::TokenType::DATA_TYPE)
				throw std::runtime_error("SYNTAX ERROR: Expecting Data Type after 'jconst'.");
			if (tokens.size() < 2 || std::get<1>(tokens[2]->token) != JDM::TokenType::VARIABLE)
				throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE");

			DataTypeEnum nextDataType = JDM::dataTypeMap.at(std::get<0>(tokens[1]->token));
			if (isInVec(nextDataType,
			{
				DataTypeEnum::DATA_CONST,
				DataTypeEnum::DATA_FORCE,
				DataTypeEnum::DATA_CFORCE,
			})) throw std::runtime_error("SYNTAX ERROR: Invalid Data Type after 'jconst'.");

			this->_manageDataType(block, tokens[1], tokens[2], {tokens.begin()+3, tokens.end()}, true);

		}
		else if (dataType == DataTypeEnum::DATA_FORCE)
		{
			if (tokens.size() < 1 || std::get<1>(tokens[1]->token) != JDM::TokenType::DATA_TYPE)
				throw std::runtime_error("SYNTAX ERROR: Expecting Data Type after 'jforce'.");
			if (tokens.size() < 2 || std::get<1>(tokens[2]->token) != JDM::TokenType::VARIABLE)
				throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE");

			DataTypeEnum nextDataType = JDM::dataTypeMap.at(std::get<0>(tokens[1]->token));
			if (isInVec(nextDataType, {
				DataTypeEnum::DATA_CONST,
				DataTypeEnum::DATA_FORCE,
				DataTypeEnum::DATA_CFORCE,
			})) throw std::runtime_error("SYNTAX ERROR: Invalid Data Type after 'jforce'.");

			this->_manageDataType(block, tokens[1], tokens[2], {tokens.begin()+3, tokens.end()}, false, true);

		}
		else if (dataType == DataTypeEnum::DATA_CFORCE)
		{
			if (tokens.size() < 1 || std::get<1>(tokens[1]->token) != JDM::TokenType::DATA_TYPE)
				throw std::runtime_error("SYNTAX ERROR: Expecting Data Type after 'jcforce'.");
			if (tokens.size() < 2 || std::get<1>(tokens[2]->token) != JDM::TokenType::VARIABLE)
				throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE");

			DataTypeEnum nextDataType = JDM::dataTypeMap.at(std::get<0>(tokens[1]->token));
			if (isInVec(nextDataType, {
				DataTypeEnum::DATA_CONST,
				DataTypeEnum::DATA_FORCE,
				DataTypeEnum::DATA_CFORCE,
			})) throw std::runtime_error("SYNTAX ERROR: Invalid Data Type after 'jcforce'.");

			this->_manageDataType(block, tokens[1], tokens[2], {tokens.begin()+3, tokens.end()}, true, true);

		}
		else
		{
			if (tokens.size() == 1 || std::get<1>(tokens[1]->token) != JDM::TokenType::VARIABLE)
				throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE");
			this->_manageDataType(block, tokens[0], tokens[1], {tokens.begin()+2, tokens.end()});
		}
	} else if (tkType == JDM::TokenType::VARIABLE) {
		this->_manageVariable(block, tokens);
	}
	else if (tkType == JDM::TokenType::CONTROL_FLOW)
	{
		this->_manageControlFlow(block, tokens[0], {tokens.begin()+1, tokens.end()});
	}
	else if (tkType == JDM::TokenType::CUSTOM_KEYWORD)
	{
		this->_manageCustomKeyword(block, tokens[0], {tokens.begin()+1, tokens.end()});
	}
	else if (tkType == JDM::TokenType::CUSTOM_FUNCTIONS)
	{
		auto customFunc = JDM::customFunctionMap.at( std::get<0>(tokens[0]->token) );
		if (customFunc == CUSFUNC_CLEAR)
		{
			if (tokens.size() == 1)
				block->instruction.push_back(std::make_shared<CFunction>(customFunc));
			else
				throw std::runtime_error("SYNTAX ERROR: Invalid clear syntax.");
			return;
		}
		if (tokens.size() < 2 || std::get<1>(tokens[1]->token) != JDM::TokenType::ARROW_OPERATOR)
			throw std::runtime_error("SYNTAX ERROR: Expecting '=>' after using Custom Function.");

		if (customFunc == CUSFUNC_LOG || customFunc == CUSFUNC_LOGN)
		{
			std::vector<std::shared_ptr<Expression>> expressions;
			this->_manageLogger(block, { tokens.begin()+2, tokens.end() }, expressions);
			block->instruction.push_back(std::make_shared<Logger>(expressions, (customFunc == CUSFUNC_LOGN)));
		}
		else if (customFunc == CUSFUNC_SLEEP)
		{
			auto expr = this->_createExpression(this->_transformTokenStruct({ tokens.begin()+2, tokens.end() }))[0]->expression;
			block->instruction.push_back(std::make_shared<CFunction>(customFunc, expr));
		}
		// else if (customFunc == CUSFUNC_GETTYPE)
		// {
		// 	auto expr = this->_createExpression(this->_transformTokenStruct({ tokens.begin()+2, tokens.end() }))[0]->expression;
		// 	block->instruction.push_back(std::make_shared<CFunction>(customFunc, expr));
		// }
		else if (customFunc == CUSFUNC_INCLUDE)
		{
			if (tokens.size() != 3)
				throw std::runtime_error("SYNTAX ERROR: Invalid include syntax.");

			if (std::get<1>(tokens[2]->token) != JDM::TokenType::VARIABLE)
				throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE");

			auto expr = this->_createExpression(this->_transformTokenStruct({ tokens.begin()+2, tokens.end() }))[0]->expression;
			block->instruction.push_back(std::make_shared<CFunction>(customFunc, expr));
		}
		else
		{
			throw std::runtime_error("SYNTAX ERROR: Invalid Custom Function.");
		}
	}
}

/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////

Parser: MANAGE THE TOKENS

//////////////////////////////////////////////////////////////////////////////////////////////////////////
*/
JDM_DLL
const void Parser::_manageLogger(
	const std::shared_ptr<Block> &block,
	const std::vector<std::shared_ptr<TokenStruct>>& tokens,
	std::vector<std::shared_ptr<Expression>> &expressions)
{
	auto it = std::find_if(tokens.begin(), tokens.end(), [](const std::shared_ptr<TokenStruct> &tok){
		return std::get<1>(tok->token) == JDM::TokenType::ARROW_OPERATOR;
	});
	if (it != tokens.end()) {
		expressions.push_back(this->_createExpression(
			this->_transformTokenStruct( { tokens.begin(), it } ))[0]->expression);
		if (it+1 != tokens.end()) this->_manageLogger( block, { it+1, tokens.end() }, expressions);

	} else {
		expressions.push_back(this->_createExpression(
			this->_transformTokenStruct(tokens))[0]->expression);
	}
}

JDM_DLL
const void Parser::_manageControlFlow(
	const std::shared_ptr<Block      > &block,
	const std::shared_ptr<TokenStruct> &controlType,
	const std::vector<std::shared_ptr<TokenStruct>>& tokenS)
{
	// I can use at since I know I the elements will exist 100%
	ControlFlowEnum control = JDM::controlFlowMap.at(std::get<0>(controlType->token));
	switch (control)
	{
		case ControlFlowEnum::CONTROL_IF    : this->_manageIfElseWhileStatement(block, tokenS, control); break;
		case ControlFlowEnum::CONTROL_ELSEIF: this->_manageIfElseWhileStatement(block, tokenS, control); break;
		case ControlFlowEnum::CONTROL_ELSE  : this->_manageIfElseWhileStatement(block, tokenS, control); break;
		case ControlFlowEnum::CONTROL_WHILE : this->_manageIfElseWhileStatement(block, tokenS, control); break;

		case ControlFlowEnum::CONTROL_FOR:
			if (std::get<1>(tokenS[0]->token) != JDM::TokenType::VARIABLE) throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE");
			if (std::get<1>(tokenS[1]->token) != JDM::TokenType::ARROW_OPERATOR) throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");
			this->_manageForloop(block, tokenS[0], {tokenS.begin()+2, tokenS.end()});
			break;

		case ControlFlowEnum::CONTROL_FOREACH: this->_manageForEachLoop(block, tokenS); break;
		case ControlFlowEnum::CONTROL_FUNC   :
			if (std::get<1>(tokenS[0]->token) != JDM::TokenType::VARIABLE) throw std::runtime_error("SYNTAX ERROR: Expecting FUNCTION NAME");
			this->_manageCreateFunction(block, tokenS[0], {tokenS.begin()+1, tokenS.end()});
			break;

		case ControlFlowEnum::CONTROL_RETURN :
			if (tokenS.empty()) block->instruction.push_back(std::make_shared<Return>(nullptr));
			else block->instruction.push_back(std::make_shared<Return>(this->_createExpression(this->_transformTokenStruct(tokenS))[0]->expression));
			break;
	}
	if (!isInVec(control, {ControlFlowEnum::CONTROL_IF, ControlFlowEnum::CONTROL_ELSEIF}))
		this->__currIfLink->current = nullptr;
}

JDM_DLL
const void Parser::_manageCustomKeyword(
	const std::shared_ptr<Block      > &block,
	const std::shared_ptr<TokenStruct> &keywordType,
	const std::vector<std::shared_ptr<TokenStruct>>& tokenS)
{
	CustomKeywordEnum keyword = JDM::customKeywordMap.at(std::get<0>(keywordType->token));
	switch (keyword)
	{
		case CustomKeywordEnum::KEYWORD_BREAK:
			if (!tokenS.empty()) throw std::runtime_error("SYNTAX ERROR: Break must not have expression.");
			block->instruction.push_back(std::make_shared<Break>());
			return;
		case CustomKeywordEnum::KEYWORD_CONTINUE :
			if (!tokenS.empty()) throw std::runtime_error("SYNTAX ERROR: Continue must not have expression.");
			block->instruction.push_back(std::make_shared<Continue>());
			return;
	}
	throw std::runtime_error("SYNTAX ERROR: The keyword is not appropriate here.");
}

JDM_DLL
const void Parser::_manageCreateFunction(
	const std::shared_ptr<Block      > &block,
	const std::shared_ptr<TokenStruct> &funcName,
	const std::vector<std::shared_ptr<TokenStruct>>& tokens)
{
	if (std::get<0>(tokens[0]->token) != "(") throw std::runtime_error("SYNTAX ERROR: Expecting '()'");
	if (tokens.size() < 3)                    throw std::runtime_error("SYNTAX ERROR: Expecting Block");
	if (std::get<1>(tokens[1]->token) != JDM::TokenType::ARROW_OPERATOR)
		throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");

	std::vector<std::shared_ptr<Variable>> parameters;
	if (!tokens[0]->child.empty()) this->_setTypeVarArgs(parameters, tokens[0]->child[0]->child);

	block->instruction.push_back(std::make_shared<CreateFunction>(
		this->_createBlockOrLine(tokens[2], {tokens.begin()+2, tokens.end()}),
		std::make_shared<VariableObjects>(funcName), parameters
	));
}

JDM_DLL
const void Parser::_manageForEachLoop(
	const std::shared_ptr<Block      > &block,
	const std::vector<std::shared_ptr<TokenStruct>>& tokens)
{
	auto it = std::find_if(tokens.begin(), tokens.end(), [](const std::shared_ptr<TokenStruct> &tok){
		return std::get<1>(tok->token) == JDM::TokenType::ARROW_OPERATOR;
	});
	if (it   == tokens.end()) throw std::runtime_error(           "SYNTAX ERROR: Expecting '=>'"        );
	if (it+1 == tokens.end()) throw std::runtime_error("SYNTAX ERROR: Expecting a List | Map | jreverse");

	std::vector<std::shared_ptr<VariableObjects>> varArgs       ;
	std::vector<std::shared_ptr<Expression     >> listArgs      ;
	std::vector<std::shared_ptr<MapStruct      >> mapArgs       ;
	std::vector<std::shared_ptr<TokenStruct    >> remainingToken;
	            std::shared_ptr<VariableObjects > varToRun      ;
	            std::shared_ptr<Block           > blockWillRun  ;

	bool isReverse = false;
	bool isMap     = false;
	remainingToken = { it+1, tokens.end() };

	// If : jreverse => ... => { ... }
	auto rev = JDM::customKeywordMap.find(std::get<0>(remainingToken[0]->token));
	if (rev != JDM::customKeywordMap.end() && rev->second == CustomKeywordEnum::KEYWORD_REVERSE) {
		if (std::get<1>(remainingToken[1]->token) != JDM::TokenType::ARROW_OPERATOR) 
			throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");

		// Remove two token on remainingToken as they already used
		// |> ... => { ... }
		isReverse = true;
		remainingToken = {remainingToken.begin() + 2, remainingToken.end()};
	}

	// Find another => on |> ... => { ... }
	auto it2 = std::find_if(remainingToken.begin(), remainingToken.end(), [](const std::shared_ptr<TokenStruct> &tok){
		return std::get<1>(tok->token) == JDM::TokenType::ARROW_OPERATOR;
	});
	if (it2   == remainingToken.end()) throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");
	if (it2+1 == remainingToken.end()) throw std::runtime_error("SYNTAX ERROR: Expecting a BLOCK");

	// Create the BLOCK
	blockWillRun = this->_createBlockOrLine(*(it2+1), {it2+1, remainingToken.end()});

	// Get token on left side |> ...
	remainingToken = { remainingToken.begin(), it2 };

	// Check if token is just a variable
	if (remainingToken.size() == 1 && std::get<1>(remainingToken[0]->token) == JDM::TokenType::VARIABLE) {
		varToRun = std::make_shared<VariableObjects>(remainingToken[0]);
	} else {
		if (remainingToken.size() != 1) throw std::runtime_error("SYNTAX ERROR: Expecting ('jlist' | 'jmap') not Expression");
		if (std::get<1>(remainingToken[0]->token) != JDM::TokenType::OPEN_CASES)
			throw std::runtime_error("SYNTAX ERROR: Expecting 'jlist' | 'jmap'");

		if (std::get<0>(remainingToken[0]->token) == "[")
			this->_setArguments(listArgs, remainingToken[0]->child[0]->child);
		else if (std::get<0>(remainingToken[0]->token) == "{") {
			isMap = true;
			this->_setMapArguments(mapArgs, remainingToken[0]->child[0]->child);
		} else std::runtime_error("SYNTAX ERROR: Expecting 'jlist' | 'jmap'");
	}

	// Return a list of VarObjects
	remainingToken = { tokens.begin(), it };
	if (remainingToken.size() == 1 && std::get<1>(remainingToken[0]->token) == JDM::TokenType::OPEN_CASES)
		this->_setVarArgs(varArgs, remainingToken[0]->child[0]->child);
	else
		this->_setVarArgs(varArgs, remainingToken);

	if (varToRun != nullptr) {
		if (varArgs.size() == 2) {
			block->instruction.push_back(std::make_shared<ForEachMapStatement>(
				blockWillRun, varArgs[0], varArgs[1], varToRun, isReverse));
		} else if (varArgs.size() == 1) {
			block->instruction.push_back(std::make_shared<ForEachListStatement>(
				blockWillRun, varArgs[0], varToRun, isReverse));
		} else throw std::runtime_error("SYNTAX ERROR: Cannot determine if 'jlist' | 'jmap'");

	} else if (isMap) {
		if (varArgs.size() < 2)  throw std::runtime_error("SYNTAX ERROR: Missing 'key' | 'value'");
		if (varArgs.size() >= 3) throw std::runtime_error("SYNTAX ERROR: Too many varArgs.");
		block->instruction.push_back(std::make_shared<ForEachMapStatement>(
			blockWillRun, varArgs[0], varArgs[1], std::make_shared<MapObject> (mapArgs), isReverse));
	} else {
		if (varArgs.size() < 1)  throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE.");
		if (varArgs.size() >= 2) throw std::runtime_error("SYNTAX ERROR: Too many varArgs.");
		block->instruction.push_back(std::make_shared<ForEachListStatement>(
			blockWillRun, varArgs[0], std::make_shared<ListObject>(listArgs), isReverse));
	}
}

JDM_DLL
const void Parser::_manageDataType(
	const std::shared_ptr<Block      > &block,
	const std::shared_ptr<TokenStruct> &dataType,
	const std::shared_ptr<TokenStruct> &varName,
	const std::vector<std::shared_ptr<TokenStruct>>& tokenS,
	const bool isConst, const bool isForce)
{
	DataTypeEnum currDataType = JDM::dataTypeMap.at(std::get<0>(dataType->token));
	auto newVar = std::make_shared<VariableObjects>(varName);

	if (tokenS.empty()) {	
		block->instruction.push_back(std::make_shared<Declaration>( dataType, newVar, nullptr, isConst, isForce ));
		return;
	}
	if (std::get<1>(tokenS[0]->token) != JDM::TokenType::ASSIGNMENT_OPERATOR
	 || std::get<0>(tokenS[0]->token) != "=") throw std::runtime_error("SYNTAX ERROR: Expecting '='");

	std::vector<std::shared_ptr<TokenStruct>> remainingToken = {tokenS.begin() + 1, tokenS.end()};
	if (remainingToken.empty()) throw std::runtime_error("SYNTAX ERROR: Invalid Expression");
	auto transformToken = this->_transformTokenStruct(remainingToken);

	block->instruction.push_back(std::make_shared<Declaration>(dataType, newVar, this->_createExpression(transformToken)[0]->expression, isConst, isForce ));
}

JDM_DLL
const void Parser::_manageVariable(
	const std::shared_ptr<Block      > &block,
	const std::vector<std::shared_ptr<TokenStruct>>& tokens)
{
	if (tokens.empty()) return;
	auto it = std::find_if(tokens.begin(), tokens.end(), [](const std::shared_ptr<TokenStruct> &tok){
		return std::get<1>(tok->token) == JDM::TokenType::ASSIGNMENT_OPERATOR;
	});

	if (it != tokens.end()) {
		std::vector<std::shared_ptr<TokenStruct>> leftToks  = {tokens.begin(), it};
		std::vector<std::shared_ptr<TokenStruct>> rightToks = {it+1, tokens.end()};
		std::shared_ptr<Expression> newExpression = this->_createExpression(
			this->_transformTokenStruct(rightToks))[0]->expression;

		if (leftToks.size() > 1)
			block->instruction.push_back(std::make_shared<Call>(true,
				this->_createCallObject(this->_transformTokenStruct(leftToks), nullptr, true),
				newExpression, std::get<0>((*it)->token)));

		else block->instruction.push_back(std::make_shared<Assignment>(
			std::make_shared<VariableObjects>(tokens[0]), *it, newExpression ));

	} else block->instruction.push_back(std::make_shared<Call>(false,
			this->_createCallObject(this->_transformTokenStruct(tokens), nullptr, false)));
}

JDM_DLL
const void Parser::_manageIfElseWhileStatement(
	const std::shared_ptr<Block      > &block,
	const std::vector<std::shared_ptr<TokenStruct>>& tokens,
	ControlFlowEnum control)
{
	auto it = std::find_if(tokens.begin(), tokens.end(), [](const std::shared_ptr<TokenStruct> &tok)
	{
		return std::get<1>(tok->token) == JDM::TokenType::ARROW_OPERATOR;
	});
	if (control == ControlFlowEnum::CONTROL_ELSE)
	{
		if (std::get<1>(tokens[0]->token) != JDM::TokenType::ARROW_OPERATOR)
			throw std::runtime_error("SYNTAX ERROR: Expecting '=>' on 'jelse' before the block.");
	}

	if (it   == tokens.end()) throw std::runtime_error("SYNTAX ERROR: Expecting '=>' on control flow.");
	if (it+1 == tokens.end()) throw std::runtime_error("SYNTAX ERROR: Expecting a BLOCK");

	std::shared_ptr<Expression> condition;
	std::shared_ptr<Block> blockWillRun = this->_createBlockOrLine(*(it+1), {it+1, tokens.end()});

	if (control != ControlFlowEnum::CONTROL_ELSE)
		condition = this->_createExpression(this->_transformTokenStruct({tokens.begin(), it}))[0]->expression;

	switch (control) {
		case ControlFlowEnum::CONTROL_IF:
			this->__currIfLink->current = std::make_shared<IfStatement>(blockWillRun, condition);
			block->instruction.push_back(this->__currIfLink->current);
			break;
		case ControlFlowEnum::CONTROL_ELSEIF:
			if (this->__currIfLink->current == nullptr) throw std::runtime_error("SYNTAX ERROR: jelseif with no jif");
			this->__currIfLink->current->elseIf = std::make_shared<IfStatement>(blockWillRun, condition);
			this->__currIfLink->current = this->__currIfLink->current->elseIf;
			break;
		case ControlFlowEnum::CONTROL_ELSE:
			if (this->__currIfLink->current == nullptr) throw std::runtime_error("SYNTAX ERROR: jelse with no jif");
			this->__currIfLink->current->elseIf = std::make_shared<IfStatement>(blockWillRun);
			break;
		case ControlFlowEnum::CONTROL_WHILE:
			block->instruction.push_back(std::make_shared<WhileStatement>(blockWillRun, condition));
			break;
	}
}

JDM_DLL
const void Parser::_manageForloop(
	const std::shared_ptr<Block      > &block,
	const std::shared_ptr<TokenStruct> &varName,
	const std::vector<std::shared_ptr<TokenStruct>>& tokens)
{
	auto it = std::find_if(tokens.begin(), tokens.end(), [](const std::shared_ptr<TokenStruct> &tok){
		return std::get<1>(tok->token) == JDM::TokenType::ARROW_OPERATOR;
	});
	if (it   == tokens.end()) throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");
	if (it+1 == tokens.end()) throw std::runtime_error("SYNTAX ERROR: Expecting a BLOCK");

	std::shared_ptr<VariableObjects> newVar       = std::make_shared<VariableObjects>(varName);
	std::shared_ptr<Block          > blockWillRun = this->_createBlockOrLine(*(it+1), { it+1, tokens.end() });
	std::vector<std::shared_ptr<TokenStruct>> remainingToken = { tokens.begin(), it };
	std::vector<std::shared_ptr<Expression>> arguments;

	if (remainingToken.size() == 1 && std::get<1>(remainingToken[0]->token) == JDM::TokenType::OPEN_CASES) {
		if (remainingToken[0]->child.empty()) throw std::runtime_error("SYNTAX ERROR: Expecting 'Arguments'");
		this->_setArguments(arguments, remainingToken[0]->child[0]->child);
	} else this->_setArguments(arguments, remainingToken);

	if (arguments.size() < 1) throw std::runtime_error("SYNTAX ERROR: Expecting 'Arguments'");

	auto start = (arguments.size() >= 2) ? arguments[0] : nullptr;      // If there is more than 1, then he is arg[1]
	auto stop  = (arguments.size() >= 2) ? arguments[1] : arguments[0]; // If there is more than 1, then he must be arg[1]
	auto step  = (arguments.size() >= 3) ? arguments[2] : nullptr;      // If there is more than 2, then he is arg[2]

	block->instruction.push_back(std::make_shared<ForLoopStatement>( blockWillRun, newVar, start, stop, step ));			
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
			JDM::TokenType::DOT_OPERATOR
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
		if (std::get<1>(tok->token) == JDM::TokenType::COMMA_OPERATOR) continue;
		if (std::get<1>(tok->token) == JDM::TokenType::VARIABLE) vec.push_back(std::make_shared<VariableObjects>(tok));
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
		return std::get<1>(tok->token) == JDM::TokenType::COMMA_OPERATOR; });

	if (it != trav.end()) {
		vec.push_back(this->_createExpression(this->_transformTokenStruct({ trav.begin(), it }))[0]->expression);
		if (it+1 != trav.end()) this->_setArguments(vec, { it+1, trav.end() });
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
		return std::get<1>(tok->token) == JDM::TokenType::COMMA_OPERATOR; });

	if (it != trav.end()) {
		vec.push_back(this->_createMap({ trav.begin(), it }));
		if (it+1 != trav.end()) this->_setMapArguments(vec, { it+1, trav.end() });
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
		return std::get<1>(tok->token) == JDM::TokenType::COMMA_OPERATOR; });

	if (it != trav.end()) {
		vec.push_back(this->_createVariable( { trav.begin(), it } ));
		if (it+1 != trav.end()) this->_setTypeVarArgs(vec, { it+1, trav.end() });
	} else vec.push_back(this->_createVariable( trav ));
}

/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////

Parser: CREATE PURPOSE

//////////////////////////////////////////////////////////////////////////////////////////////////////////
*/
JDM_DLL
const std::vector<std::shared_ptr<ExpressionToken>> Parser::_createExpression(
	const std::vector<std::shared_ptr<ExpressionToken>>& vec,
	bool checkInvalid, bool lambdaBool,
	bool powerBool   , bool mulDivBool,
	bool addSubBool  , bool notBool   ,
	bool shiftBool   , bool addBool   ,
	bool xorBool     , bool orBool    ,
	bool logicalBool , bool relNotBool,
	bool relAndBool  , bool relOrBool )
{
	if (vec.empty()) throw std::runtime_error("SYNTAX ERROR: Invalid Expression");
	else if (vec.size() == 1) {

		if (vec[0]->expression == nullptr) {
			if (std::get<1>(vec[0]->token->token) == JDM::TokenType::OPEN_CASES) {
				if (vec[0]->token->child.size() > 1) throw std::runtime_error("SYNTAX ERROR: Unexpected ';'");

				if (std::get<0>(vec[0]->token->token) == "(") {
					if (vec[0]->token->child.empty())     throw std::runtime_error("SYNTAX ERROR: Empty Open Case");					
					return this->_createExpression(this->_transformTokenStruct(vec[0]->token->child[0]->child));
				}
				else if (std::get<0>(vec[0]->token->token) == "[") {
					if (vec[0]->token->child.empty()) {
						auto newExpression = std::make_shared<Expression>();
						newExpression->firstValue = std::make_shared<ListObject>();
						return { std::make_shared<ExpressionToken>( newExpression ) };
					} else return { std::make_shared<ExpressionToken>(this->_createListExpression(vec[0]->token->child[0]->child) ) };

				} else if (std::get<0>(vec[0]->token->token) == "{") {
					if (vec[0]->token->child.empty()) {
						auto newExpression = std::make_shared<Expression>();
						newExpression->firstValue = std::make_shared<MapObject>();
						return { std::make_shared<ExpressionToken>( newExpression ) };
					} else return { std::make_shared<ExpressionToken>(this->_createMapExpression(vec[0]->token->child[0]->child) ) };
				}

			} else if (std::get<1>(vec[0]->token->token) == JDM::TokenType::CUSTOM_KEYWORD) {
				const std::string &value = std::get<0>(vec[0]->token->token);
				if (value != "jtrue" && value != "jfalse")
					throw std::runtime_error("SYNTAX ERROR: Unexpected Custom Keyword");

				auto newExpression = std::make_shared<Expression>();
				newExpression->firstValue = std::make_shared<IntegerObjects>(vec[0]->token);
				vec[0]->expression = newExpression;

			} else {
				auto newExpression = std::make_shared<Expression>();
				this->_setValueObjects(newExpression->firstValue, vec[0]->token);
				vec[0]->expression = newExpression;
			}
		}
		return vec;
	}

	if (checkInvalid) {
		for (const auto &v : vec) {
			if (std::get<1>(v->token->token) == JDM::TokenType::CONTROL_FLOW)
				throw std::runtime_error("SYNTAX ERROR: Invalid Expression. Control Flow in Expression is Forbidden");
			if (std::get<1>(v->token->token) == JDM::TokenType::COMMA_OPERATOR)
				throw std::runtime_error("SYNTAX ERROR: Invalid Expression. Comma is Forbidden");
			if (std::get<1>(v->token->token) == JDM::TokenType::ASSIGNMENT_OPERATOR)
				throw std::runtime_error("SYNTAX ERROR: Invalid Expression. Assignment is Forbidden");
		}
		checkInvalid = false;
	}
	std::vector<std::shared_ptr<ExpressionToken>> newVec;
	auto checkFunction = [&](const std::vector<std::string> &targetString, bool &check) {
		if (!check) return false;
		check = this->_findAndReplaceExpression(targetString, vec, newVec);
		return check;
	};
	if (lambdaBool) lambdaBool = this->_findAndReplaceLambdaCall(vec, newVec);
	if (lambdaBool
	 || checkFunction(JDM::powerVec  , powerBool  ) // ARITH (EXPONENT to POWER)
	 || checkFunction(JDM::multDivVec, mulDivBool ) // ARITH (*, /, //)
	 || checkFunction(JDM::addSubVec , addSubBool ) // ARITH (+, -)
	 || checkFunction(JDM::notVec    , notBool    ) // NOT
	 || checkFunction(JDM::shiftVec  , shiftBool  ) // SHIFT
	 || checkFunction(JDM::andVec    , addBool    ) // AND
	 || checkFunction(JDM::xorVec    , xorBool    ) // XOR
	 || checkFunction(JDM::orVec     , orBool     ) // OR
	 || checkFunction(JDM::logicalVec, logicalBool) // LOGICAL
	 || checkFunction(JDM::relNotVec , relNotBool ) // RELATIONAL NOT
	 || checkFunction(JDM::relAndVec , relAndBool ) // RELATIONAL AND
	 || checkFunction(JDM::relOrVec  , relOrBool  ) // RELATIONAL OR
	) { }
	else {
		if (vec[0]->expression == nullptr && std::get<1>(vec[0]->token->token) == JDM::TokenType::OPEN_CASES)
			throw std::runtime_error("SYNTAX ERROR: Case after case.");

		auto newExpression = std::make_shared<Expression>();
		// Check If it is a casting Object
		if (vec.size() == 2 && vec[0]->token != nullptr && vec[1]->token != nullptr
		 && std::get<1>(vec[0]->token->token) == JDM::TokenType::DATA_TYPE) {
			auto dataTypeEnum = JDM::dataTypeMap.at(std::get<0>(vec[0]->token->token));

			if (std::get<1>(vec[1]->token->token) != JDM::TokenType::OPEN_CASES) throw std::runtime_error("SYNTAX ERROR: Casting must be in Cases.");
			if (vec[1]->token->child.empty()) throw std::runtime_error("SYNTAX ERROR: Casting on Nothing.");					
			if (dataTypeEnum == DataTypeEnum::DATA_ANY) throw std::runtime_error("SYNTAX ERROR: Casting on Any.");

			if (dataTypeEnum == DataTypeEnum::DATA_LAMBDA) {
				std::vector<std::shared_ptr<ExpressionToken>> newVec;
				this->_findAndReplaceLambdaCall(this->_transformTokenStruct( vec[1]->token->child[0]->child ), newVec);
				if (newVec.size() != 1) throw std::runtime_error("SYNTAX ERROR: Invalid Lambda Declaration");
				else newExpression->firstValue = std::make_shared<CastObjects>( dataTypeEnum, newVec[0]->expression );
			} else
				newExpression->firstValue = std::make_shared<CastObjects>( dataTypeEnum,
					this->_createExpression(this->_transformTokenStruct( vec[1]->token->child[0]->child ))[0]->expression);
		} else newExpression->firstValue = this->_createCallObject(vec, nullptr, false);

		return { std::make_shared<ExpressionToken>(newExpression) };
	}
	return this->_createExpression(
		newVec, checkInvalid, lambdaBool, powerBool, mulDivBool,
		addSubBool, notBool, shiftBool, addBool, xorBool, orBool,
		logicalBool, relNotBool, relAndBool, relOrBool
	);
}

JDM_DLL
const std::shared_ptr<CallObjects> Parser::_createCallObject(
	const std::vector<std::shared_ptr<ExpressionToken>> &turnToCallObject,
	const std::shared_ptr<CallObjects> &previous,
	const bool isAssigning)
{
	if (turnToCallObject.empty()) throw std::runtime_error("SYNTAX ERROR: Empty Object.");
	if (std::get<1>(turnToCallObject[0]->token->token) != JDM::TokenType::VARIABLE
	 && std::get<1>(turnToCallObject[0]->token->token) != JDM::TokenType::STRING
	 && std::get<0>(turnToCallObject[0]->token->token) != "("
	 && std::get<0>(turnToCallObject[0]->token->token) != "[")
		throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE, STRING, FUNCTION, CLASS, MEMBER.");

	auto newFuncObj = std::make_shared<CallObjects>(nullptr, previous);
	int tokUsed = 0;
	if (std::get<1>(turnToCallObject[0]->token->token) == JDM::TokenType::OPEN_CASES) {
		if (std::get<0>(turnToCallObject[0]->token->token) == "(") {
			if (isAssigning) throw std::runtime_error("SYNTAX ERROR: Cannot Assign on Function Call.");

			std::vector<std::shared_ptr<Expression>> listVec;
			if (!turnToCallObject[0]->token->child.empty())
				this->_setArguments(listVec, turnToCallObject[0]->token->child[0]->child);

			newFuncObj->currObject = std::make_shared<FunctionObjects>(turnToCallObject[0]->token, listVec);
		}
		else {
			if (turnToCallObject[0]->token->child.empty())
				throw std::runtime_error("SYNTAX ERROR: Expecting a index integer");

			newFuncObj->currObject = std::make_shared<ExpressionObjects>(turnToCallObject[0]->token,
			 	this->_createExpression(this->_transformTokenStruct(
			 		turnToCallObject[0]->token->child[0]->child))[0]->expression
			);
		}

	} else if (turnToCallObject.size() > 1
		&& std::get<1>(turnToCallObject[1]->token->token) == JDM::TokenType::OPEN_CASES) {

		if (std::get<0>(turnToCallObject[1]->token->token) == "(") {
			if (isAssigning) throw std::runtime_error("SYNTAX ERROR: Cannot Assign on Function Call.");

			std::vector<std::shared_ptr<Expression>> listVec;
			if (!turnToCallObject[1]->token->child.empty())
				this->_setArguments(listVec, turnToCallObject[1]->token->child[0]->child);

			newFuncObj->currObject = std::make_shared<FunctionObjects>(turnToCallObject[0]->token, listVec);

		} else if (std::get<0>(turnToCallObject[1]->token->token) == "[") {
			if (turnToCallObject[1]->token->child.empty())
				throw std::runtime_error("SYNTAX ERROR: Expecting a index integer");

			newFuncObj->currObject = std::make_shared<ExpressionObjects>(turnToCallObject[0]->token,
			 	this->_createExpression(this->_transformTokenStruct(
			 		turnToCallObject[1]->token->child[0]->child))[0]->expression
			);
		} else throw std::runtime_error("SYNTAX ERROR: Expecting '(', '[' open case.");

		tokUsed++;
	} else {
		newFuncObj->currObject = std::make_shared<VariableObjects>(turnToCallObject[0]->token);
		if (std::get<1>(turnToCallObject[0]->token->token) == JDM::TokenType::STRING)
			newFuncObj->currObject->setToken(JDM::TokenType::STRING);
	}

	if (previous != nullptr) previous->nextObject = newFuncObj;
	if (turnToCallObject.size() > 1 + tokUsed) {
		if (std::get<1>(turnToCallObject[1 + tokUsed]->token->token) == JDM::TokenType::OPEN_CASES) {
			return this->_createCallObject( { turnToCallObject.begin() + tokUsed + 1, turnToCallObject.end() }, newFuncObj, isAssigning );

		} else if (std::get<1>(turnToCallObject[1 + tokUsed]->token->token) != JDM::TokenType::DOT_OPERATOR)
			throw std::runtime_error("SYNTAX ERROR: Expecting DOT OPERATOR");

		return this->_createCallObject( { turnToCallObject.begin() + tokUsed + 2, turnToCallObject.end() }, newFuncObj, isAssigning );
	}
	return newFuncObj;
}


/**
 * Create a MapStruct from a vector of TokenStructs.
 * This function converts a sequence of tokens into a map structure, where tokens are organized into key-value pairs.
 * It expects the presence of the '=>' token in the input vector.
 * 
 * Expected Tokens:
 * EXPRESSION ARROW EXPRESSION
 * 
 * Else: THROW
 * 
 * @param turnToMap A vector of TokenStructs representing the tokens to be transformed into a map.
 * @return A shared pointer to a MapStruct containing the resulting key-value mapping.
 * @throws std::runtime_error if '=>' token is not found in the input.
 */
JDM_DLL
const std::shared_ptr<MapStruct> Parser::_createMap(
	const std::vector<std::shared_ptr<TokenStruct>>& turnToMap)
{
	if (turnToMap.empty()) return nullptr;
	auto it = std::find_if(turnToMap.begin(), turnToMap.end(), [](const std::shared_ptr<TokenStruct>& tok) {
		return std::get<1>(tok->token) == JDM::TokenType::ARROW_OPERATOR; });

	if (it == turnToMap.end()) throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");

	auto newMap = std::make_shared<MapStruct>();
	newMap->key   = this->_createExpression(this->_transformTokenStruct( { turnToMap.begin(), it } ))[0]->expression;
	newMap->value = this->_createExpression(this->_transformTokenStruct( { it+1, turnToMap.end() } ))[0]->expression;
	return newMap;
}

/**
 * Create a Variable object from a vector of TokenStructs.
 * This function parses a sequence of tokens and constructs a Variable object, which represents a variable declaration
 * in the program. It expects tokens related to data type(optional) and variable name, and optionally, an assignment operator
 * followed by an expression for variable initialization.
 *
 * Expected Tokens:
 * VARIABLE
 * DATATYPE VARIABLE
 * DATATYPE VARIABLE ASSIGNMENT EXPRESSION
 * 
 * Else: THROW
 *
 * @param turnToVariable A vector of TokenStructs representing the tokens to be transformed into a Variable.
 * @return A shared pointer to a Variable object containing the parsed variable declaration.
 * @throws std::runtime_error if the input is not formatted correctly, or if essential tokens are missing.
 */
JDM_DLL
const std::shared_ptr<Variable> Parser::_createVariable(
	const std::vector<std::shared_ptr<TokenStruct>> &turnToVariable)
{
	if (turnToVariable.empty()) throw std::runtime_error("SYNTAX ERROR: Expecting DATATYPE | VARIABLE");
	auto newVariable = std::make_shared<Variable>();

	// Use to ensure the correct index, even if there is Data type present if not.
	int incrementIfDataType = 0;

	// Parse the data type token if it's present.
	if (std::get<1>(turnToVariable[0]->token) == JDM::TokenType::DATA_TYPE)
		newVariable->dataType = turnToVariable[incrementIfDataType++];

	if (turnToVariable.size() == incrementIfDataType || std::get<1>(turnToVariable[incrementIfDataType]->token) != JDM::TokenType::VARIABLE)
		throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE");

	// Check for optional variable initialization.
	newVariable->varName = std::make_shared<VariableObjects>(turnToVariable[incrementIfDataType]);
	if (turnToVariable.size() > 1+incrementIfDataType) {
		if (std::get<1>(turnToVariable[1+incrementIfDataType]->token) != JDM::TokenType::ASSIGNMENT_OPERATOR
		 || std::get<0>(turnToVariable[1+incrementIfDataType]->token) != "=")
			throw std::runtime_error("SYNTAX ERROR: Expecting '=' or None");

		newVariable->varValue = this->_createExpression(
			this->_transformTokenStruct( { turnToVariable.begin() + 2 + incrementIfDataType, turnToVariable.end() } ))[0]->expression;
	}
	return newVariable;
}


/**
 * 
 * USEFUL FUNCTIONS
 * 
 */
JDM_DLL
const bool Parser::_isBlockCurly(const std::shared_ptr<TokenStruct> &token)
{
	return std::get<0>(token->token) == "{";
}

JDM_DLL
const bool Parser::_findAndReplaceLambdaCall(
	const std::vector<std::shared_ptr<ExpressionToken>>& vec,
	std::vector<std::shared_ptr<ExpressionToken>> &newVec)
{
	for (int index = 0; index < vec.size(); index++) {
		if (vec[index]->token == nullptr || std::get<1>(vec[index]->token->token) != JDM::TokenType::ARROW_OPERATOR) continue;

		auto leftTokens  = this->_getAllTokenFromSide(vec, index, true);
		auto rightTokens = this->_getAllTokenFromSide(vec, index, false);

		if ((rightTokens.size() != 1 && rightTokens.size() != 3) || leftTokens.size() != 1
		|| std::get<0>(leftTokens [0]->token->token) != "("
		|| std::get<0>(rightTokens[0]->token->token) != "{")
			throw std::runtime_error("SYNTAX ERROR: Invalid Expression. Is it a Lambda");

		bool willCall = false;
		std::vector<std::shared_ptr<Variable  >> parameters;
		std::vector<std::shared_ptr<Expression>> arguments;

		auto blockWillRun = this->_getBlock(rightTokens[0]->token);

		if (!leftTokens[0]->token->child.empty())
			this->_setTypeVarArgs(parameters, leftTokens[0]->token->child[0]->child);

		if (rightTokens.size() == 3) {
			willCall = true;
			if (std::get<1>(rightTokens[1]->token->token) != JDM::TokenType::ARROW_OPERATOR
			 || std::get<0>(rightTokens[2]->token->token) != "(")
				throw std::runtime_error("SYNTAX ERROR: Invalid Lambda Expression.");
			if (!rightTokens[2]->token->child.empty()) this->_setArguments(arguments, rightTokens[2]->token->child[0]->child);

			if (arguments.size() < parameters.size()) throw std::runtime_error("SYNTAX ERROR: Too few Arguments.");
			if (arguments.size() > parameters.size()) throw std::runtime_error("SYNTAX ERROR: Too Many Arguments.");
		}
		auto newExpression = std::make_shared<Expression>();
		newExpression->firstValue = std::make_shared<LambdaObjects>( willCall, arguments, parameters, blockWillRun );

		newVec  = { vec.begin(), vec.begin() + (index - leftTokens.size()) };
		newVec.push_back( std::make_shared<ExpressionToken>(newExpression) );
		newVec.insert(newVec.end(), vec.begin() + (index + rightTokens.size() + 1), vec.end());
		return true;
	}
	return false;
}

JDM_DLL
const std::vector<std::shared_ptr<ExpressionToken>> Parser::_getAllTokenFromSide(
	const std::vector<std::shared_ptr<ExpressionToken>>& vec,
	int index, bool isLeft)
{
	std::vector<std::shared_ptr<ExpressionToken>> sideVector;
	index += (isLeft) ? -1 : 1; // Don't start at the index

	while (index >= 0 && index < vec.size()) {
		if (vec[index]->token != nullptr && (
				std::get<1>(vec[index]->token->token) != JDM::TokenType::STRING &&
				isInVec(std::get<0>(vec[index]->token->token), JDM::operatorCombinedVector))
			) {
			if (sideVector.empty()) throw std::runtime_error("SYNTAX ERROR: Invalid Expression");
			break;
		}
		sideVector.push_back(vec[index]);
		index += (isLeft) ? -1 : 1;
	}
	if (isLeft) std::reverse(sideVector.begin(), sideVector.end());
	return sideVector;
}

JDM_DLL
const bool Parser::_findAndReplaceExpression(
	const std::vector<std::string> &targetString,
	const std::vector<std::shared_ptr<ExpressionToken>>& vec,
	std::vector<std::shared_ptr<ExpressionToken>> &newVec)
{
	for (int index = 0; index < vec.size(); index++) {
		if (vec[index]->token == nullptr || !(isInVec(std::get<0>(vec[index]->token->token), targetString))) continue;
		if (std::get<1>(vec[index]->token->token) == JDM::TokenType::STRING) continue;

		if (targetString.size() == 1 && targetString[0] == "!") {
			auto rightTokens = this->_getAllTokenFromSide(vec, index, false);
			std::shared_ptr<Expression> newExpression = std::make_shared<Expression>();
			this->_getExpressionFromTokens(rightTokens, newExpression->firstExpression, newExpression->firstValue);

			newVec = newVec  = { vec.begin(), vec.begin() + index };
			newExpression->opWillUse = vec[index]->token;
			newVec.push_back( std::make_shared<ExpressionToken>(newExpression) );
			newVec.insert(newVec.end(), vec.begin()+(index+rightTokens.size()+1), vec.end());

		} else {
			auto leftTokens  = this->_getAllTokenFromSide(vec, index, true);
			auto rightTokens = this->_getAllTokenFromSide(vec, index, false);

			std::shared_ptr<Expression> newExpression = std::make_shared<Expression>();
			this->_getExpressionFromTokens(leftTokens, newExpression->firstExpression, newExpression->firstValue);
			this->_getExpressionFromTokens(rightTokens, newExpression->secondExpression, newExpression->secondValue);

			newVec  = { vec.begin(), vec.begin() + (index - leftTokens.size()) };
			newExpression->opWillUse = vec[index]->token;
			newVec.push_back( std::make_shared<ExpressionToken>(newExpression) );
			newVec.insert(newVec.end(), vec.begin()+(index+rightTokens.size()+1), vec.end());
		}
		return true;
	}
	return false;
}

JDM_DLL
const void Parser::_getExpressionFromTokens(
	const std::vector<std::shared_ptr<ExpressionToken>> &exprToken,
	std::shared_ptr<Expression> &expression,
	std::shared_ptr<VarObjects> &value)
{
	if (exprToken.size() == 1) {
		if (exprToken[0]->expression != nullptr) expression = exprToken[0]->expression;
		else if (exprToken[0]->token != nullptr && std::get<1>(exprToken[0]->token->token) == JDM::TokenType::OPEN_CASES)
			expression = this->_createExpression( { exprToken[0] } )[0]->expression;
		else this->_setValueObjects(value, exprToken[0]->token);
	} else 
		expression = this->_createExpression( exprToken )[0]->expression;
}

JDM_DLL
const std::vector<std::shared_ptr<ExpressionToken>> Parser::_transformTokenStruct(
	const std::vector<std::shared_ptr<TokenStruct>>& tokenS)
{
	std::vector<std::shared_ptr<ExpressionToken>> expressionTok;
	for (const auto &tok : tokenS) {
		expressionTok.push_back( std::make_shared<ExpressionToken>(nullptr, tok) );
	}
	return expressionTok;
}

JDM_DLL
const void Parser::_setValueObjects(
	std::shared_ptr<VarObjects> &value,
	const std::shared_ptr<TokenStruct> &tok)
{
	switch (std::get<1>(tok->token)) {
		case JDM::TokenType::VARIABLE: value = std::make_shared<VariableObjects>(tok); return;
		case JDM::TokenType::DOUBLE : value = std::make_shared<DoubleObjects  >(tok); return;
		case JDM::TokenType::INTEGER : value = std::make_shared<IntegerObjects >(tok); return;
		case JDM::TokenType::STRING  : value = std::make_shared<StringObjects  >(tok); return;
		case JDM::TokenType::CUSTOM_KEYWORD :
			if (std::get<0>(tok->token) != "jtrue" && std::get<0>(tok->token) != "jfalse")
				throw std::runtime_error("SYNTAX ERROR: Unexpected Custom Keyword");
			value = std::make_shared<IntegerObjects>(tok); return;

		default: throw std::runtime_error("SYNTAX ERROR: Invalid Value");
	}
}
