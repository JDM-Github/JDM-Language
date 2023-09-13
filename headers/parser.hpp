#pragma once
#include <tuple>
#include <vector>

#include "include_h.hpp"
#include "objects.hpp"
#include "instruction.hpp"
#include "error_handler.hpp"

class Parser {
private:
	std::shared_ptr<TokenStruct> __rootTokens;
	std::shared_ptr<Block> __mainBlock = std::make_shared<Block>();

	std::vector<std::shared_ptr<TokenStruct>> __blockTokens;
	
	struct ExpressionToken {
		std::shared_ptr<Expression> expression;
		std::shared_ptr<TokenStruct> token;
	};
	std::shared_ptr<Block> _mainBlock;

public:
	inline Parser(const std::shared_ptr<TokenStruct> &tokens)
		: __rootTokens(tokens) {
		this->__mainBlock = this->_getBlock(this->__rootTokens);
		this->analyzeAST(this->__mainBlock);
	}
	inline const void analyzeAST(const std::shared_ptr<Block> &block, const std::string &space = "") {
		std::cout << space << "BLOCK: \n";
		for (const auto &instruction : block->instruction) {
			if (instruction->getType() == IfStatementInstruction) {
				std::cout << space << "  " << this->_instructionToString(instruction->getType()) << ": \n";
				this->analyzeAST(
					std::dynamic_pointer_cast<Block>(
						std::dynamic_pointer_cast<IfStatement>(instruction)->blockWillRun
					), space+"    ");
			} else if (instruction->getType() == ForLoopStatementInstruction) {
				std::cout << space << "  " << this->_instructionToString(instruction->getType()) << ": \n";
				auto forLoop = std::dynamic_pointer_cast<ForLoopStatement>(instruction);
				if (forLoop->start) {
					std::cout << space << "    START: \n";
					this->_printExpression(forLoop->start, space+"      ");
				}
				if (forLoop->stop) {
					std::cout << space << "    STOP : \n";
					this->_printExpression(forLoop->stop,  space+"      ");
				}
				if (forLoop->step) {
					std::cout << space << "    STEP : \n";
					this->_printExpression(forLoop->step,  space+"      ");
				}
				this->analyzeAST(std::dynamic_pointer_cast<Block>(forLoop->blockWillRun), space+"    ");
			}
			else
				std::cout << space << "  " << this->_instructionToString(instruction->getType()) << ": { ... }\n";
		}
	}

private:
	inline const char *_instructionToString(InstructionType instruction) {
		switch (instruction) {
			case DeclarationInstruction      : return "DECLARATION";
			case AssignmentInstruction       : return "ASSIGNMENT";
			case IfStatementInstruction      : return "IF";
			case ForLoopStatementInstruction : return "FOR";
		}
		return "Invalid Instruction";
	}

	inline const std::shared_ptr<Block> _getBlock(
		   const std::shared_ptr<TokenStruct>& tokenS) {
		std::shared_ptr<Block> block = std::make_shared<Block>();
		for (const auto& tokenStruct : tokenS->child)
			this->predictInstruction(block, tokenStruct->child);
		return block;
	}

	inline const void predictInstruction(
		const std::shared_ptr<Block> &block,
		const std::vector<std::shared_ptr<TokenStruct>>& tokens) {

		switch (std::get<1>(tokens[0]->token)) {
			case TokenType::DATA_TYPE:
				if (tokens.size() == 1) return;
				if (std::get<1>(tokens[1]->token) != TokenType::VARIABLE) 
					throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE");
				this->_manageDataType(block, tokens[0], tokens[1], {tokens.begin()+2, tokens.end()});
				return;

			case TokenType::VARIABLE:
				this->_manageVariable(block, tokens[0], {tokens.begin()+1, tokens.end()});
				return;

			case TokenType::CONTROL_FLOW:
				this->_manageControlFlow(block, tokens[0], {tokens.begin()+1, tokens.end()});
				return;
		};
	}

	inline const void _manageControlFlow(
		const std::shared_ptr<Block      > &block,
		const std::shared_ptr<TokenStruct> &controlType,
		const std::vector<std::shared_ptr<TokenStruct>>& tokenS) {

		ControlFlowEnum control = JDM::controlFlowMap.at(std::get<0>(controlType->token));
		switch (control) {
			case ControlFlowEnum::CONTROL_IF:
				this->_manageIfStatement(block, tokenS);
				break;

			case ControlFlowEnum::CONTROL_FOR:
				if (std::get<1>(tokenS[0]->token) != TokenType::VARIABLE) 
					throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE");
				if (std::get<1>(tokenS[1]->token) != TokenType::ARROW_OPERATOR) 
					throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");

				this->_manageForloop(block, tokenS[0], {tokenS.begin()+2, tokenS.end()});
				break;
		}
	}

	inline const bool _isBlockCurly(const std::shared_ptr<TokenStruct> &token) {
		return (std::get<1>(token->token) == TokenType::OPEN_CASES && std::get<0>(token->token) == "{");
	}

	inline const void _manageDataType(
		const std::shared_ptr<Block      > &block,
		const std::shared_ptr<TokenStruct> &dataType,
		const std::shared_ptr<TokenStruct> &varName,
		const std::vector<std::shared_ptr<TokenStruct>>& tokenS) {
		/**
		 * Make it available on Type Casting, if possible
		 * 
		 */

		std::shared_ptr<VariableObjects> newVar = std::make_shared<VariableObjects>(varName);

		if (tokenS.empty()) {	
			block->instruction.push_back(std::make_shared<Declaration>( dataType, newVar ));
			return;
		}
		if (std::get<1>(tokenS[0]->token) != TokenType::ASSIGNMENT_OPERATOR)
			throw std::runtime_error("SYNTAX ERROR: Expecting '='");

		std::shared_ptr<Expression> newExpression = this->_createExpression(
			this->_transformTokenStruct({tokenS.begin() + 1, tokenS.end()}))[0]->expression;

		block->instruction.push_back(std::make_shared<Declaration>( dataType, newVar, newExpression ));
	}

	inline const void _manageVariable(
		const std::shared_ptr<Block      > &block,
		const std::shared_ptr<TokenStruct> &varName,
		const std::vector<std::shared_ptr<TokenStruct>>& tokenS) {
		/**
		 * TODO:
		 * 
		 * -> Make it available on '.' or function call
		 * 
		 */

		if (tokenS.empty()) return;
		std::shared_ptr<VariableObjects> newVar = std::make_shared<VariableObjects>(varName);

		if (std::get<1>(tokenS[0]->token) != TokenType::ASSIGNMENT_OPERATOR)
			throw std::runtime_error("SYNTAX ERROR: Expecting '='");

		std::shared_ptr<Expression> newExpression = this->_createExpression(
			this->_transformTokenStruct({tokenS.begin() + 1, tokenS.end()}))[0]->expression;

		block->instruction.push_back(std::make_shared<Assignment>( newVar, newExpression ));
	}

	inline const void _manageIfStatement(
		const std::shared_ptr<Block      > &block,
		const std::vector<std::shared_ptr<TokenStruct>>& tokens) {

		auto it = std::find_if(tokens.begin(), tokens.end(), [](const std::shared_ptr<TokenStruct> &tok){
			return std::get<1>(tok->token) == TokenType::ARROW_OPERATOR;
		});

		if (it != tokens.end()) {
			std::shared_ptr<Block> blockWillRun;
			if (it+1 != tokens.end()) {
				auto newTok = (*(it+1));
				if ( this->_isBlockCurly(newTok)) blockWillRun = this->_getBlock(newTok);
				else {
					blockWillRun = std::make_shared<Block>();
					this->predictInstruction(blockWillRun, {it+1, tokens.end()});
				}
			} else throw std::runtime_error("SYNTAX ERROR: Expecting a BLOCK");

			std::shared_ptr<Expression> condition = this->_createExpression(
				this->_transformTokenStruct({tokens.begin(), it}))[0]->expression;

			block->instruction.push_back(std::make_shared<IfStatement>(blockWillRun, condition));
		}
		else throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");
	}

	inline const void _manageForloop(
		const std::shared_ptr<Block      > &block,
		const std::shared_ptr<TokenStruct> &varName,
		const std::vector<std::shared_ptr<TokenStruct>>& tokens) {

		std::shared_ptr<VariableObjects> newVar = std::make_shared<VariableObjects>(varName);
		auto it = std::find_if(tokens.begin(), tokens.end(), [](const std::shared_ptr<TokenStruct> &tok){
			return std::get<1>(tok->token) == TokenType::ARROW_OPERATOR;
		});

		if (it != tokens.end()) {
			std::vector<std::shared_ptr<Expression>> arguments;
			std::shared_ptr<Block> blockWillRun;

			if (it+1 != tokens.end()) {
				auto newTok = (*(it+1));
				if ( this->_isBlockCurly(newTok)) blockWillRun = this->_getBlock(newTok);
				else {
					blockWillRun = std::make_shared<Block>();
					this->predictInstruction(blockWillRun, {it+1, tokens.end()});
				}
			} else throw std::runtime_error("SYNTAX ERROR: Expecting a BLOCK");

			std::vector<std::shared_ptr<TokenStruct>> remainingToken = {tokens.begin(), it};
			if (remainingToken.size() == 1 && std::get<1>(remainingToken[0]->token) == TokenType::OPEN_CASES)
				this->_setArguments(arguments, remainingToken[0]->child[0]->child);
			else this->_setArguments(arguments, remainingToken);

			if (arguments.size() >= 3) {
				block->instruction.push_back(std::make_shared<ForLoopStatement>(
					blockWillRun, newVar,
					arguments[0], // START
					arguments[1], // STOP
					arguments[2]  // STEP
				));
			} else if (arguments.size() >= 2) {
				block->instruction.push_back(std::make_shared<ForLoopStatement>(
					blockWillRun, newVar,
					arguments[0], // START
					arguments[1]  // STOP
				));
			} else if (arguments.size() >= 1) {
				block->instruction.push_back(std::make_shared<ForLoopStatement>(
					blockWillRun, newVar,
					nullptr,      // START
					arguments[0]  // STOP
				));
			} else throw std::runtime_error("SYNTAX ERROR: Expecting 'Arguments'");
		}
		else throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");
	}

	inline const std::vector<std::shared_ptr<ExpressionToken>> _createExpression(
		const std::vector<std::shared_ptr<ExpressionToken>>& vec) {
		/**
		 * TODO:
		 * 
		 * -> Make the NOT operand and ~ work, as intended.
		 * -> Make it available on function call
		 *
		 */

		if (vec.empty()) throw std::runtime_error("SYNTAX ERROR: Invalid Expression");

		else if (vec.size() == 1) {
			if (vec[0]->expression == nullptr) {
				if (std::get<1>(vec[0]->token->token) == TokenType::OPEN_CASES) {

					return this->_createExpression(
						this->_transformTokenStruct(vec[0]->token->child[0]->child)
					);
				} else {
					std::shared_ptr<Expression> newExpression = std::make_shared<Expression>();
					this->_setValueObjects(newExpression->firstValue, vec[0]->token);
					vec[0]->expression = newExpression;
				}
			}
			return vec;
		}
		std::vector<std::shared_ptr<ExpressionToken>> newVec;
		if (this->_findAndReplaceExpression({"**"                            }, vec, newVec) // ARITH (EXPONENT to POWER)
		 || this->_findAndReplaceExpression({"*", "/", "//"                  }, vec, newVec) // ARITH (*, /, //)
		 || this->_findAndReplaceExpression({"+", "-"                        }, vec, newVec) // ARITH (+, -)
		 || this->_findAndReplaceExpression({"~"                             }, vec, newVec) // NOT
		 || this->_findAndReplaceExpression({"<<", ">>"                      }, vec, newVec) // SHIFT
		 || this->_findAndReplaceExpression({"&"                             }, vec, newVec) // AND
		 || this->_findAndReplaceExpression({"^"                             }, vec, newVec) // XOR
		 || this->_findAndReplaceExpression({"|"                             }, vec, newVec) // OR
		 || this->_findAndReplaceExpression({"<", ">", "<=", ">=", "==", "!="}, vec, newVec) // LOGICAL
		 || this->_findAndReplaceExpression({"!"                             }, vec, newVec) // RELATIONAL NOT
		 || this->_findAndReplaceExpression({"&&"                            }, vec, newVec) // RELATIONAL AND
		 || this->_findAndReplaceExpression({"||"                            }, vec, newVec) // RELATIONAL OR
		) { }
		return this->_createExpression(newVec);
	}

	inline const bool _findAndReplaceExpression(
		const std::vector<std::string> &targetString,
		const std::vector<std::shared_ptr<ExpressionToken>>& vec,
		std::vector<std::shared_ptr<ExpressionToken>> &newVec) {
		/**
		 * TODO:
		 * 
		 * -> Make it available or make it so it can handle function call
		 * 
		 */
		for (int index = 0; index < vec.size(); index++) {
			std::shared_ptr<TokenStruct> tok = vec[index]->token;

			if (tok != nullptr) {
				std::string tokStr = std::get<0>(tok->token);
				if (this->_isIn(tokStr, targetString)) {
					int leftMinus = 1;
					int rightAdd  = 2;
					bool isLeftParen = false;
					bool isRightParen = false;

					if (vec[index-1]->token != nullptr)
						if (std::get<0>(vec[index-1]->token->token) == "(")
							isLeftParen = true;

					if (vec[index+1]->token != nullptr)
						if (std::get<0>(vec[index+1]->token->token) == "(")
							isRightParen = true;

					newVec  = { vec.begin(), vec.begin() + (index - leftMinus) };
					std::shared_ptr<ExpressionToken> newExpressionTok = std::make_shared<ExpressionToken>();
					std::shared_ptr<Expression     > newExpression = std::make_shared<Expression>();

					if (vec[index - 1]->expression != nullptr)
						newExpression->firstExpression = vec[index - 1]->expression;
					else if (isLeftParen)
						if (!vec[index - 1]->token->child.empty() && vec[index - 1]->token->child.size() <= 1)
							newExpression->firstExpression = this->_getExpression(vec[index - 1]->token->child);
						else throw std::runtime_error("SYNTAX ERROR: Invalid Expression");
					else this->_setValueObjects(newExpression->firstValue, vec[index - 1]->token);

					newExpression->opWillUse = vec[index]->token;

					if (vec[index + 1]->expression != nullptr)
						newExpression->secondExpression = vec[index + 1]->expression;
					else if (isRightParen)
						if (!vec[index + 1]->token->child.empty() && vec[index + 1]->token->child.size() <= 1)
							newExpression->secondExpression = this->_getExpression(vec[index + 1]->token->child);
						else throw std::runtime_error("SYNTAX ERROR: Invalid Expression");
					else this->_setValueObjects(newExpression->secondValue, vec[index + 1]->token);

					newExpressionTok->expression = newExpression;
					newVec.push_back(newExpressionTok);

					newVec.insert(newVec.end(), vec.begin()+(index+rightAdd), vec.end());
					return true;
				}
			}
		}
		return false;
	}

	inline const std::shared_ptr<Expression> _getExpression(
		const std::vector<std::shared_ptr<TokenStruct>>& tokenS) {
		return this->_createExpression(this->_transformTokenStruct(tokenS[0]->child))[0]->expression;
	}

	inline const void _printExpression(const std::shared_ptr<Expression>& expr, const std::string &space = "", int depth=0) {
		/**
		 * Might need a little rework. But good for now.
		 * 
		 */
		if (!expr) return;
		if (depth == 0) std::cout << space << "EXPRESSION: {\n";

		if (expr->firstValue) {
			std::cout << space << "  Value: ";
			JDM::checkAndCallReturnStringValue(expr->firstValue);
			std::cout << '\n';
		} else {
			std::cout << space << "  Expression: {\n";
			this->_printExpression(expr->firstExpression, space+"  ", depth+1);
			std::cout << space << "  }\n";
		}

		// if operator exist then thier is 100% a second value or expression
		if (expr->opWillUse) {
			std::cout << space << "  Operator: " << std::get<0>(expr->opWillUse->token) << '\n';

			if (expr->secondValue) {
				std::cout << space << "  Value: ";
				JDM::checkAndCallReturnStringValue(expr->secondValue);
				std::cout << '\n';
			} else if (expr->secondExpression) {
				std::cout << space << "  Expression: {\n";
				this->_printExpression(expr->secondExpression, space+"  ", depth+1);
				std::cout << space << "  }\n";
			}
		}
		if (depth == 0) std::cout << space << "}\n";
	}

	inline const std::vector<std::shared_ptr<ExpressionToken>> _transformTokenStruct(
		const std::vector<std::shared_ptr<TokenStruct>>& tokenS) {
		std::vector<std::shared_ptr<ExpressionToken>> expressionTok;
		for (const auto &tok : tokenS) {
			std::shared_ptr<ExpressionToken> exp = std::make_shared<ExpressionToken>();
			exp->token = tok;
			expressionTok.push_back(exp);
		}
		return expressionTok;
	}

	inline const void _setValueObjects(std::shared_ptr<VarObjects> &value, const std::shared_ptr<TokenStruct> &tok) {
		switch (std::get<1>(tok->token)) {
			case TokenType::VARIABLE:
				value = std::make_shared<VariableObjects>(tok);
				return;
			case TokenType::DECIMAL:
				value = std::make_shared<DoubleObjects>(tok);
				return;
			case TokenType::INTEGER:
				value = std::make_shared<IntegerObjects>(tok);
				return;
			case TokenType::STRING:
				value = std::make_shared<StringObjects>(tok);
				return;
			default: {
				throw std::runtime_error("SYNTAX ERROR: Invalid Value");
			}
		}
	}

	inline const void _setArguments(
		std::vector<std::shared_ptr<Expression>> &vec,
		const std::vector<std::shared_ptr<TokenStruct>> &trav) {

		auto it = std::find_if(trav.begin(), trav.end(), [](const std::shared_ptr<TokenStruct>& tok) {
			return std::get<1>(tok->token) == TokenType::COMMA_OPERATOR;
		});
		if (it != trav.end()) {
			std::vector<std::shared_ptr<TokenStruct>> turnToExpression = { trav.begin(), it };
			vec.push_back(this->_createExpression(
				this->_transformTokenStruct(turnToExpression))[0]->expression);

			if (it+1 != trav.end()) _setArguments(vec, { it+1, trav.end() });

		} else
		vec.push_back(this->_createExpression(this->_transformTokenStruct(trav))[0]->expression);
	}

	template <class _TypeClass>
	inline const bool _isIn(const _TypeClass &e, const std::vector<_TypeClass> eList)  {
		for (size_t i = 0; i < eList.size(); i++) 
			if (e == eList[i]) return true;
		return false;
	}

	// inline const _createExpression(
	// 	const std::vector<std::string> &stringRepresentationToken
	// ) {
	// 	auto it = std::find(stringRepresentationToken.begin(), stringRepresentationToken.end(), 'x');
	// 	if (it != stringRepresentationToken.end()) {
	//     	index = std::distance(stringRepresentationToken.begin(), it);

	//     }
	// }

	// inline const void _traverseTokenStruct(const std::shared_ptr<TokenStruct> &curr, const std::string &space) {
	// 	this->__stringStream
	//     	<< space << "{\n"
	//     	<< space << "  token: "     << std::get<0>(curr->token) << ",\n"
	//     	<< space << "  tokenType: " << this->_tokenTypeToString(std::get<1>(curr->token)) << ",\n"
	//     	<< space << "  row: "       << std::get<2>(curr->token) << ",\n"
	//     	<< space << "  col: "       << std::get<3>(curr->token);

	// 	if (!curr->child.empty()) {
	// 		this->__stringStream << ",\n" << space << "  child: {\n";
	// 		for (const auto& tokenStruct : curr->child) {
	// 			this->__stringStream << space << "    [\n";
	// 			for (const auto& token : tokenStruct->child)
	// 				this->_traverseTokenStruct(token, space + "      ");
	// 			this->__stringStream << space << "    ],\n";
	// 		}
	// 		this->__stringStream << space << "  }";
	// 	}
	// 	this->__stringStream << '\n' << space << "},\n";
	// }
	inline const void _makeAssignment() {
	}
};
