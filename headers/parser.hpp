#pragma once
#include <tuple>
#include <vector>

#include <algorithm>
#include "defines.hpp"
#include "objects.hpp"
#include "instruction.hpp"
#include "error_handler.hpp"

class JDM_DLL Parser {
private:
	std::shared_ptr<TokenStruct> __rootTokens;
	std::shared_ptr<Block> __mainBlock = std::make_shared<Block>();

	std::vector<std::shared_ptr<TokenStruct>> __blockTokens;

	struct ExpressionToken {
		std::shared_ptr<Expression> expression;
		std::shared_ptr<TokenStruct> token;
		ExpressionToken(
			const std::shared_ptr<Expression> &exp = nullptr,
			const std::shared_ptr<TokenStruct> &tok = nullptr)
		: expression(exp), token(tok) {}
	};
	std::shared_ptr<Block> _mainBlock;

	struct IfStatementLink {
		std::shared_ptr<IfStatement> current;
		std::shared_ptr<IfStatementLink> next;
		std::shared_ptr<IfStatementLink> prev;
	};

	std::shared_ptr<IfStatementLink> __currIfLink;

public:
	JDM_DLL Parser(const std::shared_ptr<TokenStruct> &tokens);
	JDM_DLL const void analyzeAST(const std::shared_ptr<Block> &block, const std::string &space = "");

private:

	JDM_DLL const std::shared_ptr<Block> _getBlock(
		   const std::shared_ptr<TokenStruct>& tokenS) {

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

	JDM_DLL const void _predictInstruction(
		const std::shared_ptr<Block> &block,
		const std::vector<std::shared_ptr<TokenStruct>>& tokens) {

		switch (std::get<1>(tokens[0]->token)) {
			case TokenType::DATA_TYPE:
				if (tokens.size() == 1 || std::get<1>(tokens[1]->token) != TokenType::VARIABLE)
					throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE");
				this->_manageDataType(block, tokens[0], tokens[1], {tokens.begin()+2, tokens.end()});
				return;

			case TokenType::VARIABLE:
				this->_manageVariable(block, tokens);
				return;

			case TokenType::CONTROL_FLOW:
				this->_manageControlFlow(block, tokens[0], {tokens.begin()+1, tokens.end()});
				return;
		};
	}

	JDM_DLL const void _manageControlFlow(
		const std::shared_ptr<Block      > &block,
		const std::shared_ptr<TokenStruct> &controlType,
		const std::vector<std::shared_ptr<TokenStruct>>& tokenS) {

		// I can use at since I know I the elements will exist 100%
		ControlFlowEnum control = JDM::controlFlowMap.at(std::get<0>(controlType->token));
		switch (control) {
			case ControlFlowEnum::CONTROL_IF    : this->_manageIfElseWhileStatement(block, tokenS, control); break;
			case ControlFlowEnum::CONTROL_ELSEIF: this->_manageIfElseWhileStatement(block, tokenS, control); break;
			case ControlFlowEnum::CONTROL_ELSE  : this->_manageIfElseWhileStatement(block, tokenS, control); break;
			case ControlFlowEnum::CONTROL_WHILE : this->_manageIfElseWhileStatement(block, tokenS, control); break;

			case ControlFlowEnum::CONTROL_FOR:
				if (std::get<1>(tokenS[0]->token) != TokenType::VARIABLE) 
					throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE");
				if (std::get<1>(tokenS[1]->token) != TokenType::ARROW_OPERATOR) 
					throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");
				this->_manageForloop(block, tokenS[0], {tokenS.begin()+2, tokenS.end()});
				break;

			case ControlFlowEnum::CONTROL_FOREACH: this->_manageForEachLoop(block, tokenS); break;
			case ControlFlowEnum::CONTROL_FUNC   :
				if (std::get<1>(tokenS[0]->token) != TokenType::VARIABLE) 
					throw std::runtime_error("SYNTAX ERROR: Expecting FUNCTION NAME");
				this->_manageCreateFunction(block, tokenS[0], {tokenS.begin()+1, tokenS.end()});
				break;
		}

		if (!isInVec(control, {ControlFlowEnum::CONTROL_IF, ControlFlowEnum::CONTROL_ELSEIF}))
			this->__currIfLink->current = nullptr;
	}

/**
 * 
 * MANAGE TOKENS
 * 
 */
private:

	JDM_DLL const void _manageCreateFunction(
		const std::shared_ptr<Block      > &block,
		const std::shared_ptr<TokenStruct> &funcName,
		const std::vector<std::shared_ptr<TokenStruct>>& tokens) {

		if (std::get<0>(tokens[0]->token) != "(") throw std::runtime_error("SYNTAX ERROR: Expecting '()'");
		if (tokens.size() < 3)                    throw std::runtime_error("SYNTAX ERROR: Expecting Block");

		std::vector<std::shared_ptr<Variable>> parameters;

		if (!tokens[0]->child.empty())
			this->_setTypeVarArgs(parameters, tokens[0]->child[0]->child);

		if (std::get<1>(tokens[1]->token) != TokenType::ARROW_OPERATOR) 
			throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");

		block->instruction.push_back(std::make_shared<CreateFunction>(
			this->_createBlockOrLine(tokens[2], {tokens.begin()+2, tokens.end()}),
			std::make_shared<VariableObjects>(funcName), parameters
		));
	}

	JDM_DLL const void _manageForEachLoop(
		const std::shared_ptr<Block      > &block,
		const std::vector<std::shared_ptr<TokenStruct>>& tokens) {

		auto it = std::find_if(tokens.begin(), tokens.end(), [](const std::shared_ptr<TokenStruct> &tok){
			return std::get<1>(tok->token) == TokenType::ARROW_OPERATOR;
		});
		if (it   == tokens.end()) throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");
		if (it+1 == tokens.end()) throw std::runtime_error("SYNTAX ERROR: Expecting a List | Map | jreverse");

		std::shared_ptr<Block>                        blockWillRun;
		std::vector<std::shared_ptr<VariableObjects>> varArgs;
		std::vector<std::shared_ptr<Expression>>      listArgs;
		std::vector<std::shared_ptr<MapStruct>>       mapArgs;
		std::shared_ptr<VariableObjects>              varToRun;
		std::vector<std::shared_ptr<TokenStruct>>     remainingToken;

		bool isReverse = false;
		bool isMap     = false;

		// Retrieve remaining token on right side
		// Might be
		// |> jreverse => ... => { ... }
		// |> ... => { ... }
		remainingToken = { it+1, tokens.end() };

		// If : jreverse => ... => { ... }
		auto rev = JDM::customKeywordMap.find(std::get<0>(remainingToken[0]->token));
		if (rev != JDM::customKeywordMap.end() && rev->second == CustomKeywordEnum::KEYWORD_REVERSE) {
			if (std::get<1>(remainingToken[1]->token) != TokenType::ARROW_OPERATOR) 
				throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");

			// Remove two token on remainingToken as they already used
			// |> ... => { ... }
			isReverse = true;
			remainingToken = {remainingToken.begin() + 2, remainingToken.end()};
		}

		// Find another => on |> ... => { ... }
		auto it2 = std::find_if(remainingToken.begin(), remainingToken.end(), [](const std::shared_ptr<TokenStruct> &tok){
			return std::get<1>(tok->token) == TokenType::ARROW_OPERATOR;
		});
		if (it2   == remainingToken.end()) throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");
		if (it2+1 == remainingToken.end()) throw std::runtime_error("SYNTAX ERROR: Expecting a BLOCK");

		// Create the BLOCK
		blockWillRun = this->_createBlockOrLine(*(it2+1), {it2+1, remainingToken.end()});

		// Get token on left side |> ...
		remainingToken = { remainingToken.begin(), it2 };

		// Check if token is just a variable
		if (remainingToken.size() == 1 && std::get<1>(remainingToken[0]->token) == TokenType::VARIABLE) {
			varToRun = std::make_shared<VariableObjects>(remainingToken[0]);
		} else {
			if (remainingToken.size() != 1) throw std::runtime_error("SYNTAX ERROR: Expecting ('jlist' | 'jmap') not Expression");
			if (std::get<1>(remainingToken[0]->token) != TokenType::OPEN_CASES)
				throw std::runtime_error("SYNTAX ERROR: Expecting 'jlist' | 'jmap'");

			if (std::get<0>(remainingToken[0]->token) == "[")
				this->_setArguments(listArgs, remainingToken[0]->child[0]->child);
			else if (std::get<0>(remainingToken[0]->token) == "{") {
				isMap = true;
				this->_setMapArguments(mapArgs, remainingToken[0]->child[0]->child);
			} else std::runtime_error("SYNTAX ERROR: Expecting 'jlist' | 'jmap'");
		}

		// Return a list of VarObjects
		remainingToken = {tokens.begin(), it};
		if (remainingToken.size() == 1 && std::get<1>(remainingToken[0]->token) == TokenType::OPEN_CASES)
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

	JDM_DLL const void _manageDataType(
		const std::shared_ptr<Block      > &block,
		const std::shared_ptr<TokenStruct> &dataType,
		const std::shared_ptr<TokenStruct> &varName,
		const std::vector<std::shared_ptr<TokenStruct>>& tokenS) {
		/**
		 * Make it available on Type Casting, if possible
		 */
		DataTypeEnum currDataType = JDM::dataTypeMap.at(std::get<0>(dataType->token));
		auto newVar = std::make_shared<VariableObjects>(varName);

		if (tokenS.empty()) {	
			block->instruction.push_back(std::make_shared<Declaration>( dataType, newVar, nullptr ));
			return;
		}
		if (std::get<1>(tokenS[0]->token) != TokenType::ASSIGNMENT_OPERATOR)
			throw std::runtime_error("SYNTAX ERROR: Expecting '='");

		std::vector<std::shared_ptr<TokenStruct>> remainingToken = {tokenS.begin() + 1, tokenS.end()};
		if (remainingToken.empty()) throw std::runtime_error("SYNTAX ERROR: Invalid Expression");
		auto transformToken = this->_transformTokenStruct(remainingToken);

		if (currDataType == DataTypeEnum::DATA_LAMBDA) {
			std::vector<std::shared_ptr<ExpressionToken>> newVec;
			this->_findAndReplaceLambdaCall(transformToken, newVec, false);
			if (newVec.size() != 1) throw std::runtime_error("SYNTAX ERROR: Invalid Lambda Declaration");
			else block->instruction.push_back(std::make_shared<Declaration>( dataType, newVar, newVec[0]->expression ));
		} else {
			block->instruction.push_back(std::make_shared<Declaration>(
				dataType, newVar, this->_createExpression(transformToken)[0]->expression ));
		}
	}

	JDM_DLL const void _manageVariable(
		const std::shared_ptr<Block      > &block,
		const std::vector<std::shared_ptr<TokenStruct>>& tokens) {
		if (tokens.empty()) return;
		auto it = std::find_if(tokens.begin(), tokens.end(), [](const std::shared_ptr<TokenStruct> &tok){
			return std::get<1>(tok->token) == TokenType::ASSIGNMENT_OPERATOR;
		});

		if (it != tokens.end()) {
			std::vector<std::shared_ptr<TokenStruct>> leftToks  = {tokens.begin(), it};
			std::vector<std::shared_ptr<TokenStruct>> rightToks = {it+1, tokens.end()};
			std::shared_ptr<Expression> newExpression = this->_createExpression(
				this->_transformTokenStruct(rightToks))[0]->expression;

			if (leftToks.size() > 1)
				block->instruction.push_back(std::make_shared<Call>(true,
					this->_createCallObject(this->_transformTokenStruct(leftToks), nullptr, true),
					newExpression));

			else block->instruction.push_back(std::make_shared<Assignment>(
				std::make_shared<VariableObjects>(tokens[0]), newExpression ));

		} else block->instruction.push_back(std::make_shared<Call>(false,
				this->_createCallObject(this->_transformTokenStruct(tokens), nullptr, false)));
	}

	// WORKING, WILL OPTIMIZE SOON
	JDM_DLL const void _manageIfElseWhileStatement(
		const std::shared_ptr<Block      > &block,
		const std::vector<std::shared_ptr<TokenStruct>>& tokens,
		ControlFlowEnum control) {

		auto it = std::find_if(tokens.begin(), tokens.end(), [](const std::shared_ptr<TokenStruct> &tok){
			return std::get<1>(tok->token) == TokenType::ARROW_OPERATOR;
		});
		if (it   == tokens.end()) throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");
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
				if (this->__currIfLink->current == nullptr)
					throw std::runtime_error("SYNTAX ERROR: jelseif with no jif");
				this->__currIfLink->current->elseIf = std::make_shared<IfStatement>(blockWillRun, condition);
				this->__currIfLink->current = this->__currIfLink->current->elseIf;
				break;
			case ControlFlowEnum::CONTROL_ELSE:
				if (this->__currIfLink->current == nullptr)
					throw std::runtime_error("SYNTAX ERROR: jelse with no jif");
				this->__currIfLink->current->elseIf = std::make_shared<IfStatement>(blockWillRun);
				break;
			case ControlFlowEnum::CONTROL_WHILE:
				block->instruction.push_back(std::make_shared<WhileStatement>(blockWillRun, condition));
				break;
		}
	}

	// WORKING, WILL OPTIMIZE
	JDM_DLL const void _manageForloop(
		const std::shared_ptr<Block      > &block,
		const std::shared_ptr<TokenStruct> &varName,
		const std::vector<std::shared_ptr<TokenStruct>>& tokens) {

		auto it = std::find_if(tokens.begin(), tokens.end(), [](const std::shared_ptr<TokenStruct> &tok){
			return std::get<1>(tok->token) == TokenType::ARROW_OPERATOR;
		});
		if (it   == tokens.end()) throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");
		if (it+1 == tokens.end()) throw std::runtime_error("SYNTAX ERROR: Expecting a BLOCK");

		std::shared_ptr<VariableObjects> newVar = std::make_shared<VariableObjects>(varName);
		std::vector<std::shared_ptr<Expression>> arguments;

		std::shared_ptr<Block> blockWillRun = this->_createBlockOrLine(*(it+1), {it+1, tokens.end()});

		std::vector<std::shared_ptr<TokenStruct>> remainingToken = {tokens.begin(), it};
		if (remainingToken.size() == 1 && std::get<1>(remainingToken[0]->token) == TokenType::OPEN_CASES) {
			if (remainingToken[0]->child.empty()) throw std::runtime_error("SYNTAX ERROR: Expecting 'Arguments'");
			this->_setArguments(arguments, remainingToken[0]->child[0]->child);
		} else this->_setArguments(arguments, remainingToken);

		if (arguments.size() < 1) throw std::runtime_error("SYNTAX ERROR: Expecting 'Arguments'");

		auto start = (arguments.size() >= 2) ? arguments[0] : nullptr;      // If there is more than 1, then he is arg[1]
		auto stop  = (arguments.size() >= 2) ? arguments[1] : arguments[0]; // If there is more than 1, then he must be arg[1]
		auto step  = (arguments.size() >= 3) ? arguments[2] : nullptr;      // If there is more than 2, then he is arg[2]

		block->instruction.push_back(std::make_shared<ForLoopStatement>( blockWillRun, newVar, start, stop, step ));			
	}

/**
 * 
 * USEFUL FUNCTIONS
 * 
 */
private:

	JDM_DLL const bool _isBlockCurly(const std::shared_ptr<TokenStruct> &token) {
		return std::get<0>(token->token) == "{";
	}

	JDM_DLL const bool _findAndReplaceLambdaCall(
		const std::vector<std::shared_ptr<ExpressionToken>>& vec,
		std::vector<std::shared_ptr<ExpressionToken>> &newVec,
		bool willNeedToCall = true) {

		for (int index = 0; index < vec.size(); index++) {
			if (vec[index]->token == nullptr || std::get<1>(vec[index]->token->token) != TokenType::ARROW_OPERATOR) continue;

			auto leftTokens  = this->_getAllTokenFromSide(vec, index, true);
			auto rightTokens = this->_getAllTokenFromSide(vec, index, false);

			if (willNeedToCall && rightTokens.size() != 3)
				throw std::runtime_error("SYNTAX ERROR: Invalid Expression. Lambda Expected to be called on Expression.");
			if (!willNeedToCall && rightTokens.size() != 1)
				throw std::runtime_error("SYNTAX ERROR: Invalid Expression. Lambda Expected to NOT be called.");

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
				if (std::get<1>(rightTokens[1]->token->token) != TokenType::ARROW_OPERATOR
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

	JDM_DLL const std::vector<std::shared_ptr<ExpressionToken>> _getAllTokenFromSide(
		const std::vector<std::shared_ptr<ExpressionToken>>& vec,
		int index, bool isLeft = false) {

		std::vector<std::shared_ptr<ExpressionToken>> sideVector;
		index += (isLeft) ? -1 : 1; // Don't start at the index

		while (index >= 0 && index < vec.size()) {
			 if (vec[index]->token != nullptr && isInVec(std::get<0>(vec[index]->token->token), JDM::operatorCombinedVector)) {
			 	if (sideVector.empty()) throw std::runtime_error("SYNTAX ERROR: Invalid Expression");
			 	return sideVector;
			 }
			 sideVector.push_back(vec[index]);
			 index += (isLeft) ? -1 : 1;
		}
		if (isLeft) std::reverse(sideVector.begin(), sideVector.end());
		return sideVector;
	}

	JDM_DLL const bool _findAndReplaceExpression(
		const std::vector<std::string> &targetString,
		const std::vector<std::shared_ptr<ExpressionToken>>& vec,
		std::vector<std::shared_ptr<ExpressionToken>> &newVec) {

		for (int index = 0; index < vec.size(); index++) {
			if (vec[index]->token == nullptr || !isInVec(std::get<0>(vec[index]->token->token), targetString)) continue;

			auto leftTokens  = this->_getAllTokenFromSide(vec, index, true);
			auto rightTokens = this->_getAllTokenFromSide(vec, index, false);

			std::shared_ptr<Expression     > newExpression = std::make_shared<Expression>();
			this->_getExpressionFromTokens(leftTokens, newExpression->firstExpression, newExpression->firstValue);
			this->_getExpressionFromTokens(rightTokens, newExpression->secondExpression, newExpression->secondValue);

			newVec  = { vec.begin(), vec.begin() + (index - leftTokens.size()) };
			newExpression->opWillUse = vec[index]->token;
			newVec.push_back( std::make_shared<ExpressionToken>(newExpression) );
			newVec.insert(newVec.end(), vec.begin()+(index+rightTokens.size()+1), vec.end());
			return true;
		}
		return false;
	}

	JDM_DLL const void _getExpressionFromTokens(
		const std::vector<std::shared_ptr<ExpressionToken>> &exprToken,
		std::shared_ptr<Expression> &expression,
		std::shared_ptr<VarObjects> &value) {

		if (exprToken.size() == 1) {
			if (exprToken[0]->expression != nullptr) expression = exprToken[0]->expression;
			else if (exprToken[0]->token != nullptr && std::get<1>(exprToken[0]->token->token) == TokenType::OPEN_CASES)
				expression = this->_createExpression( { exprToken[0] } )[0]->expression;
			else this->_setValueObjects(value, exprToken[0]->token);
		} else 
			expression = this->_createExpression( exprToken )[0]->expression;
	}

	JDM_DLL const std::vector<std::shared_ptr<ExpressionToken>> _transformTokenStruct(
		const std::vector<std::shared_ptr<TokenStruct>>& tokenS) {
		std::vector<std::shared_ptr<ExpressionToken>> expressionTok;
		for (const auto &tok : tokenS) {
			expressionTok.push_back( std::make_shared<ExpressionToken>(nullptr, tok) );
		}
		return expressionTok;
	}

	JDM_DLL const void _setValueObjects(std::shared_ptr<VarObjects> &value, const std::shared_ptr<TokenStruct> &tok) {
		switch (std::get<1>(tok->token)) {
			case TokenType::VARIABLE: value = std::make_shared<VariableObjects>(tok); return;
			case TokenType::DECIMAL : value = std::make_shared<DoubleObjects  >(tok); return;
			case TokenType::INTEGER : value = std::make_shared<IntegerObjects >(tok); return;
			case TokenType::STRING  : value = std::make_shared<StringObjects  >(tok); return;
			default: throw std::runtime_error("SYNTAX ERROR: Invalid Value");
		}
	}

/*
 * 
 * CREATING LIST OF TOKENS
 * 
 */
private:
	JDM_DLL const std::shared_ptr<Block> _createBlockOrLine(const std::shared_ptr<TokenStruct> &currToken, const std::vector<std::shared_ptr<TokenStruct>>& tokens);
	JDM_DLL const std::shared_ptr<Expression> _createListExpression(const std::vector<std::shared_ptr<TokenStruct>>& tokens);
	JDM_DLL const std::shared_ptr<Expression> _createMapExpression(const std::vector<std::shared_ptr<TokenStruct>>& tokens);
	JDM_DLL const void _setVarArgs(std::vector<std::shared_ptr<VariableObjects>> &vec, const std::vector<std::shared_ptr<TokenStruct>> &trav);
	JDM_DLL const void _setArguments(std::vector<std::shared_ptr<Expression>> &vec, const std::vector<std::shared_ptr<TokenStruct>> &trav);
	JDM_DLL const void _setMapArguments(std::vector<std::shared_ptr<MapStruct>> &vec, const std::vector<std::shared_ptr<TokenStruct>> &trav);
	JDM_DLL const void _setTypeVarArgs( std::vector<std::shared_ptr<Variable>> &vec, const std::vector<std::shared_ptr<TokenStruct>> &trav);

/**
 * 
 * CREATE PURPOSE
 * 
 */
private:

	JDM_DLL const std::vector<std::shared_ptr<ExpressionToken>> _createExpression(
		const std::vector<std::shared_ptr<ExpressionToken>>& vec,
		bool checkInvalid = true, bool lambdaBool = true,
		bool powerBool    = true, bool mulDivBool = true,
		bool addSubBool   = true, bool notBool    = true,
		bool shiftBool    = true, bool addBool    = true,
		bool xorBool      = true, bool orBool     = true,
		bool logicalBool  = true, bool relNotBool = true,
		bool relAndBool   = true, bool relOrBool  = true
		) {
		if (vec.empty()) throw std::runtime_error("SYNTAX ERROR: Invalid Expression");

		else if (vec.size() == 1) {

			if (vec[0]->expression == nullptr) {
				if (std::get<1>(vec[0]->token->token) == TokenType::OPEN_CASES) {
					if (vec[0]->token->child.empty())     throw std::runtime_error("SYNTAX ERROR: Empty Open Case");
					if (vec[0]->token->child.size() != 1) throw std::runtime_error("SYNTAX ERROR: Unexpected ';'");

					if (std::get<0>(vec[0]->token->token) == "(")
						return this->_createExpression(this->_transformTokenStruct(vec[0]->token->child[0]->child));
					else if (std::get<0>(vec[0]->token->token) == "[") {
						return { std::make_shared<ExpressionToken>(this->_createListExpression (vec[0]->token->child[0]->child) ) };
					} else if (std::get<0>(vec[0]->token->token) == "{") {
						return { std::make_shared<ExpressionToken>(this->_createMapExpression  (vec[0]->token->child[0]->child) ) };
					}
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
				if (std::get<1>(v->token->token) == TokenType::CONTROL_FLOW)
					throw std::runtime_error("SYNTAX ERROR: Invalid Expression. Control Flow in Expression is Forbidden");
				if (std::get<1>(v->token->token) == TokenType::COMMA_OPERATOR)
					throw std::runtime_error("SYNTAX ERROR: Invalid Expression. Comma is Forbidden");
				if (std::get<1>(v->token->token) == TokenType::ASSIGNMENT_OPERATOR)
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
			auto newExpression = std::make_shared<Expression>();

			// Check If it is a casting Object
			if (vec.size() == 2 && vec[0]->token != nullptr && vec[1]->token != nullptr) {
				if (std::get<1>(vec[0]->token->token) == TokenType::DATA_TYPE) {
					auto dataTypeEnum = JDM::dataTypeMap.at(std::get<0>(vec[0]->token->token));

					if (std::get<1>(vec[1]->token->token) != TokenType::OPEN_CASES) throw std::runtime_error("SYNTAX ERROR: Casting must be in Cases.");
					if (vec[1]->token->child.empty()) throw std::runtime_error("SYNTAX ERROR: Casting on Nothing.");					
					if (dataTypeEnum == DataTypeEnum::DATA_ANY) throw std::runtime_error("SYNTAX ERROR: Casting on Any.");

					if (dataTypeEnum == DataTypeEnum::DATA_LAMBDA) {
						std::vector<std::shared_ptr<ExpressionToken>> newVec;
						this->_findAndReplaceLambdaCall(this->_transformTokenStruct( vec[1]->token->child[0]->child ), newVec, false);
						if (newVec.size() != 1) throw std::runtime_error("SYNTAX ERROR: Invalid Lambda Declaration");
						else
							newExpression->firstValue = std::make_shared<CastObjects>( dataTypeEnum, newVec[0]->expression );
					} else
						newExpression->firstValue = std::make_shared<CastObjects>( dataTypeEnum,
							this->_createExpression(this->_transformTokenStruct( vec[1]->token->child[0]->child ))[0]->expression);
				}
			} else newExpression->firstValue = this->_createCallObject(vec);

			return { std::make_shared<ExpressionToken>(newExpression) };
		}
		return this->_createExpression(
			newVec, checkInvalid, lambdaBool, powerBool, mulDivBool,
			addSubBool, notBool, shiftBool, addBool, xorBool, orBool,
			logicalBool, relNotBool, relAndBool, relOrBool
		);
	}

	JDM_DLL const std::shared_ptr<CallObjects> _createCallObject(
		const std::vector<std::shared_ptr<ExpressionToken>> &turnToCallObject,
		const std::shared_ptr<CallObjects> &previous = nullptr,
		const bool isAssigning = false) {

		if (turnToCallObject.empty()) throw std::runtime_error("SYNTAX ERROR: Empty Object.");
		if (std::get<1>(turnToCallObject[0]->token->token) != TokenType::VARIABLE)
			throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE, FUNCTION, CLASS, MEMBER.");

		auto newFuncObj = std::make_shared<CallObjects>(nullptr, previous);

		if (turnToCallObject.size() >= 2) {
			int tokUsed = 0;
			if (std::get<1>(turnToCallObject[1]->token->token) == TokenType::OPEN_CASES) {
				if (isAssigning) throw std::runtime_error("SYNTAX ERROR: Cannot Assign on Function Call.");

				std::vector<std::shared_ptr<Expression>> listVec;
				if (!turnToCallObject[1]->token->child.empty())
					this->_setArguments(listVec, turnToCallObject[1]->token->child[0]->child);

				newFuncObj->currObject = std::make_shared<FunctionObjects>(turnToCallObject[0]->token, listVec);
				tokUsed++;

			} else 
				newFuncObj->currObject = std::make_shared<VariableObjects>(turnToCallObject[0]->token);

			if (turnToCallObject.size() >= 2 + tokUsed) {
				if (std::get<1>(turnToCallObject[1 + tokUsed]->token->token) != TokenType::DOT_OPERATOR)
					throw std::runtime_error("SYNTAX ERROR: Expecting DOT OPERATOR");

				return this->_createCallObject( { turnToCallObject.begin() + tokUsed + 2, turnToCallObject.end() }, newFuncObj, isAssigning );
			}
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
	JDM_DLL const std::shared_ptr<MapStruct> _createMap(
		const std::vector<std::shared_ptr<TokenStruct>>& turnToMap) {

		if (turnToMap.empty()) return nullptr;
		auto it = std::find_if(turnToMap.begin(), turnToMap.end(), [](const std::shared_ptr<TokenStruct>& tok) {
			return std::get<1>(tok->token) == TokenType::ARROW_OPERATOR; });

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
	JDM_DLL const std::shared_ptr<Variable> _createVariable(
		const std::vector<std::shared_ptr<TokenStruct>> &turnToVariable) {

		if (turnToVariable.empty()) throw std::runtime_error("SYNTAX ERROR: Expecting DATATYPE | VARIABLE");
		auto newVariable = std::make_shared<Variable>();

		// Use to ensure the correct index, even if there is Data type present if not.
		int incrementIfDataType = 0;

		// Parse the data type token if it's present.
		if (std::get<1>(turnToVariable[0]->token) == TokenType::DATA_TYPE)
			newVariable->dataType = turnToVariable[incrementIfDataType++];

		if (turnToVariable.size() == incrementIfDataType || std::get<1>(turnToVariable[incrementIfDataType]->token) != TokenType::VARIABLE)
			throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE");

		// Check for optional variable initialization.
		newVariable->varName = std::make_shared<VariableObjects>(turnToVariable[incrementIfDataType]);
		if (turnToVariable.size() > 1+incrementIfDataType) {
			if (std::get<1>(turnToVariable[1+incrementIfDataType]->token) != TokenType::ASSIGNMENT_OPERATOR)
				throw std::runtime_error("SYNTAX ERROR: Expecting '=' or None");

			newVariable->varValue = this->_createExpression(
				this->_transformTokenStruct( { turnToVariable.begin() + 2 + incrementIfDataType, turnToVariable.end() } ))[0]->expression;
		}
		return newVariable;
	}



/**
 * 
 * DEBUGGING PURPOSE
 * 
 */
private:

	JDM_DLL const void _analyzeAllIfElse(const std::shared_ptr<IfStatement> &instruction, const std::string &space) {
		if (instruction == nullptr) return;
		std::string typeStr = (instruction->condition == nullptr) ? "ELSE" : "ELSE IF";
		Log << space << "  " << typeStr << ": \n";
		this->analyzeAST(std::dynamic_pointer_cast<Block>(instruction->blockWillRun), space+"    ");
		this->_analyzeAllIfElse(instruction->elseIf, space);
	}

	JDM_DLL const char *_instructionToString(InstructionType instruction) {
		switch (instruction) {
			case DeclarationInstruction          : return "DECLARATION";
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

	JDM_DLL const void _printExpression(const std::shared_ptr<Expression>& expr, const std::string &space = "", int depth=0) {
		/**
		 * Might need a little rework. But good for now.
		 * 
		 */
		if (!expr) return;
		if (depth == 0) Log << space << "EXPRESSION: {\n";

		if (expr->firstValue) {
			Log << space << "  Value 1: " << expr->firstValue->returnStringValue() << '\n';
		} else {
			Log << space << "  Value 1: {\n";
			this->_printExpression(expr->firstExpression, space+"  ", depth+1);
			Log << space << "  }\n";
		}

		// if operator exist then thier is 100% a second value or expression
		if (expr->opWillUse) {
			Log << space << "  Operator: " << std::get<0>(expr->opWillUse->token) << '\n';
			if (expr->secondValue) {
				Log << space << "  Value 2: " << expr->secondValue->returnStringValue() << '\n';
			} else if (expr->secondExpression) {
				Log << space << "  Value 2: {\n";
				this->_printExpression(expr->secondExpression, space+"  ", depth+1);
				Log << space << "  }\n";
			}
		}
		if (depth == 0) Log << space << "}\n";
	}
};
