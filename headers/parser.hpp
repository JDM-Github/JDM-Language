#pragma once
#include <tuple>
#include <vector>

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
	};
	std::shared_ptr<Block> _mainBlock;

	struct IfStatementLink {
		std::shared_ptr<IfStatement> current;
		std::shared_ptr<IfStatementLink> next;
		std::shared_ptr<IfStatementLink> prev;
	};

	std::shared_ptr<IfStatementLink> __currIfLink;

public:
	inline Parser(const std::shared_ptr<TokenStruct> &tokens)
		: __rootTokens(tokens) {
		this->__mainBlock = this->_getBlock(this->__rootTokens);
		this->analyzeAST(this->__mainBlock);
	}
	inline const void analyzeAST(const std::shared_ptr<Block> &block, const std::string &space = "") {
		Log << space << "BLOCK: \n";
		for (const auto &instruction : block->instruction) {
			if (instruction->getType() == IfStatementInstruction) {
				Log << space << "  " << this->_instructionToString(instruction->getType()) << ": \n";
				auto ifState = std::dynamic_pointer_cast<IfStatement>(instruction);
				this->analyzeAST(std::dynamic_pointer_cast<Block>(ifState->blockWillRun), space+"    ");
				this->_analyzeAllIfElse(ifState->elseIf, space);

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
				Log << space << "  " << this->_instructionToString(instruction->getType()) << ": { ... }\n";
		}
	}

private:
	inline const void _analyzeAllIfElse(const std::shared_ptr<IfStatement> &instruction, const std::string &space) {
		if (instruction == nullptr) return;
		std::string typeStr = (instruction->condition == nullptr) ? "ELSE" : "ELSE IF";
		Log << space << "  " << typeStr << ": \n";
		this->analyzeAST(std::dynamic_pointer_cast<Block>(instruction->blockWillRun), space+"    ");
		this->_analyzeAllIfElse(instruction->elseIf, space);
	}

	inline const char *_instructionToString(InstructionType instruction) {
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

	inline const std::shared_ptr<Block> _getBlock(
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
			this->predictInstruction(block, tokenStruct->child);

		if (this->__currIfLink->prev != nullptr) {
			this->__currIfLink = this->__currIfLink->prev;
			this->__currIfLink->next = nullptr;
		}
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

	inline const bool _isBlockCurly(const std::shared_ptr<TokenStruct> &token) {
		return (std::get<1>(token->token) == TokenType::OPEN_CASES && std::get<0>(token->token) == "{");
	}

	inline const void _createBlockOrLine(
		std::shared_ptr<Block> &blockWillRun,
		const std::shared_ptr<TokenStruct> &currToken,
		const std::vector<std::shared_ptr<TokenStruct>>& tokens) {

		if (this->_isBlockCurly(currToken)) {
			if (tokens.size() != 1 && !isInVec(std::get<1>(tokens[1]->token), {
				TokenType::DOT_OPERATOR
			}))
				throw std::runtime_error("SYNTAX ERROR: Unexpected Operator at the end of Block");
			blockWillRun = this->_getBlock(currToken);
		}
		else {
			blockWillRun = std::make_shared<Block>();
			this->predictInstruction(blockWillRun, tokens);
		}
	}

	inline const void _manageCreateFunction(
		const std::shared_ptr<Block      > &block,
		const std::shared_ptr<TokenStruct> &funcName,
		const std::vector<std::shared_ptr<TokenStruct>>& tokens) {

		std::vector<std::shared_ptr<Variable>> parameters;
		std::shared_ptr<Block> blockWillRun;

		if (std::get<0>(tokens[0]->token) != "(") throw std::runtime_error("SYNTAX ERROR: Expecting '()'");

		if (!tokens[0]->child.empty()) this->_setTypeVarArgs(parameters, tokens[0]->child[0]->child);

		if (std::get<1>(tokens[1]->token) != TokenType::ARROW_OPERATOR) 
			throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");

		if (tokens.size() < 3) throw std::runtime_error("SYNTAX ERROR: Expecting Block");
		this->_createBlockOrLine(blockWillRun, tokens[2], {tokens.begin()+2, tokens.end()});

		block->instruction.push_back(std::make_shared<CreateFunction>(
			blockWillRun, std::make_shared<VariableObjects>(funcName), parameters
		));
	}

	inline const void _manageForEachLoop(
		const std::shared_ptr<Block      > &block,
		const std::vector<std::shared_ptr<TokenStruct>>& tokens) {

		auto it = std::find_if(tokens.begin(), tokens.end(), [](const std::shared_ptr<TokenStruct> &tok){
			return std::get<1>(tok->token) == TokenType::ARROW_OPERATOR;
		});

		std::vector<std::shared_ptr<TokenStruct>> remainingToken;
		if (it != tokens.end()) {
			std::shared_ptr<Block> blockWillRun;

			std::vector<std::shared_ptr<VariableObjects>> varArgs;
			std::vector<std::shared_ptr<Expression>>      listArgs;
			std::vector<std::shared_ptr<MapStruct>>       mapArgs;
			std::shared_ptr<VariableObjects>              varToRun;

			bool isReverse = false;
			bool isMap     = false;

			if (it+1 != tokens.end()) {

				// Retrieve remaining token on right side
				// Might be
				// |> jreverse => ... => { ... }
				// |> ... => { ... }
				remainingToken = {it+1, tokens.end()};

				// If : jreverse => ... => { ... }
				auto rev = JDM::customKeywordMap.find(std::get<0>(remainingToken[0]->token));
				if (rev != JDM::customKeywordMap.end() && rev->second == CustomKeywordEnum::KEYWORD_REVERSE) {
					isReverse = true;
					if (std::get<1>(remainingToken[1]->token) != TokenType::ARROW_OPERATOR) 
						throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");

					// Remove two token on remainingToken as they already used
					// |> ... => { ... }
					remainingToken = {remainingToken.begin() + 2, remainingToken.end()};
				}

				// Find another => on |> ... => { ... }
				auto it2 = std::find_if(remainingToken.begin(), remainingToken.end(), [](const std::shared_ptr<TokenStruct> &tok){
					return std::get<1>(tok->token) == TokenType::ARROW_OPERATOR;
				});

				if (it2   == remainingToken.end()) throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");
				if (it2+1 == remainingToken.end()) throw std::runtime_error("SYNTAX ERROR: Expecting a BLOCK");

				// Check if the next token is a BLOCK or a ONE line
				auto newTok = (*(it2+1));
				if ( this->_isBlockCurly(newTok))
					blockWillRun = this->_getBlock(newTok);
				else {
					blockWillRun = std::make_shared<Block>();
					this->predictInstruction(blockWillRun, {it2+1, remainingToken.end()});
				}

				// Get token on left side |> ...
				remainingToken = {remainingToken.begin(), it2};

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
			} else throw std::runtime_error("SYNTAX ERROR: Expecting a List | Map | jreverse");

			remainingToken = {tokens.begin(), it};
			// Return a list of VarObjects
			if (remainingToken.size() == 1 && std::get<1>(remainingToken[0]->token) == TokenType::OPEN_CASES)
				this->_setVarArgs(varArgs, remainingToken[0]->child[0]->child);
			else this->_setVarArgs(varArgs, remainingToken);

			if (varToRun != nullptr) {
				if (varArgs.size() == 2) {
					block->instruction.push_back(std::make_shared<ForEachMapStatement>(
						blockWillRun, varArgs[0],
						varArgs[1], varToRun, isReverse
					));
				} else if (varArgs.size() == 1) {
					block->instruction.push_back(std::make_shared<ForEachListStatement>(
						blockWillRun, varArgs[0], varToRun, isReverse
					));
				} else throw std::runtime_error("SYNTAX ERROR: Cannot determine if 'jlist' | 'jmap'");
			} else if (isMap) {
				if (varArgs.size() < 2)  throw std::runtime_error("SYNTAX ERROR: Missing 'key' | 'value'");
				if (varArgs.size() >= 3) throw std::runtime_error("SYNTAX ERROR: Too many varArgs.");
				block->instruction.push_back(std::make_shared<ForEachMapStatement>(
					blockWillRun, varArgs[0],
					varArgs[1], std::make_shared<MapObject> (mapArgs), isReverse
				));
			} else {
				if (varArgs.size() < 1)  throw std::runtime_error("SYNTAX ERROR: Expecting a VARIABLE.");
				if (varArgs.size() >= 2) throw std::runtime_error("SYNTAX ERROR: Too many varArgs.");
				block->instruction.push_back(std::make_shared<ForEachListStatement>(
					blockWillRun, varArgs[0], std::make_shared<ListObject>(listArgs), isReverse
				));
			}

		} else throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");
	}

	inline const void _manageDataType(
		const std::shared_ptr<Block      > &block,
		const std::shared_ptr<TokenStruct> &dataType,
		const std::shared_ptr<TokenStruct> &varName,
		const std::vector<std::shared_ptr<TokenStruct>>& tokenS) {
		/**
		 * Make it available on Type Casting, if possible
		 */
		std::shared_ptr<VariableObjects> newVar = std::make_shared<VariableObjects>(varName);

		if (tokenS.empty()) {	
			block->instruction.push_back(std::make_shared<Declaration>( dataType, newVar, nullptr ));
			return;
		}
		if (std::get<1>(tokenS[0]->token) != TokenType::ASSIGNMENT_OPERATOR)
			throw std::runtime_error("SYNTAX ERROR: Expecting '='");

		std::vector<std::shared_ptr<TokenStruct>> remainingToken = {tokenS.begin() + 1, tokenS.end()};

		std::shared_ptr<Expression> newExpression = this->_createExpression(
			this->_transformTokenStruct(remainingToken))[0]->expression;
		block->instruction.push_back(std::make_shared<Declaration>( dataType, newVar, newExpression ));
		// }
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

		std::vector<std::shared_ptr<TokenStruct>> remainingToken = {tokenS.begin() + 1, tokenS.end()};

		std::shared_ptr<Expression> newExpression = this->_createExpression(
			this->_transformTokenStruct(remainingToken))[0]->expression;

		block->instruction.push_back(std::make_shared<Assignment>( newVar, newExpression ));
	}

	// WORKING, WILL OPTIMIZE SOON
	inline const void _manageIfElseWhileStatement(
		const std::shared_ptr<Block      > &block,
		const std::vector<std::shared_ptr<TokenStruct>>& tokens,
		ControlFlowEnum control) {

		auto it = std::find_if(tokens.begin(), tokens.end(), [](const std::shared_ptr<TokenStruct> &tok){
			return std::get<1>(tok->token) == TokenType::ARROW_OPERATOR;
		});

		if (it != tokens.end()) {
			std::shared_ptr<Block> blockWillRun;
			std::shared_ptr<Expression> condition;
			if (it+1 != tokens.end()) {
				auto newTok = (*(it+1));
				if ( this->_isBlockCurly(newTok)) blockWillRun = this->_getBlock(newTok);
				else {
					blockWillRun = std::make_shared<Block>();
					this->predictInstruction(blockWillRun, {it+1, tokens.end()});
				}
			} else throw std::runtime_error("SYNTAX ERROR: Expecting a BLOCK");

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
		else throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");
	}

	// WORKING, WILL OPTIMIZE
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

			if (it + 1 != tokens.end()) {
				auto newTok = *(it + 1);
				// Check If it is a Block
				if (this->_isBlockCurly(newTok)) {
					blockWillRun = this->_getBlock(newTok);
				} else {
					// Create a new block and make it a one line Code
					blockWillRun = std::make_shared<Block>();
					this->predictInstruction(blockWillRun, {it + 1, tokens.end()});
				}
			} else {
				throw std::runtime_error("SYNTAX ERROR: Expecting a BLOCK");
			}

			std::vector<std::shared_ptr<TokenStruct>> remainingToken = {tokens.begin(), it};
			if (remainingToken.size() == 1 && std::get<1>(remainingToken[0]->token) == TokenType::OPEN_CASES)
				this->_setArguments(arguments, remainingToken[0]->child[0]->child);
			else this->_setArguments(arguments, remainingToken);

			if (arguments.size() < 1) throw std::runtime_error("SYNTAX ERROR: Expecting 'Arguments'");

			auto start = (arguments.size() >= 2) ? arguments[0] : nullptr;      // If there is more than 1, then he is arg[1]
			auto stop  = (arguments.size() >= 2) ? arguments[1] : arguments[0]; // If there is more than 1, then he must be arg[1]
			auto step  = (arguments.size() >= 3) ? arguments[2] : nullptr;      // If there is more than 2, then he is arg[2]

			block->instruction.push_back(std::make_shared<ForLoopStatement>( blockWillRun, newVar, start, stop, step ));			
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
					if (vec[0]->token->child.empty())     throw std::runtime_error("SYNTAX ERROR: Empty Open Case");
					if (vec[0]->token->child.size() != 1) throw std::runtime_error("SYNTAX ERROR: Unexpected ';'");

					if (std::get<0>(vec[0]->token->token) == "(")
						return this->_createExpression(this->_transformTokenStruct(vec[0]->token->child[0]->child));

					else if (std::get<0>(vec[0]->token->token) == "[") {

						auto expressionTok = std::make_shared<ExpressionToken>();
						auto newExpression = std::make_shared<Expression>();

						std::vector<std::shared_ptr<Expression>> listVec;
						this->_setArguments(listVec, vec[0]->token->child[0]->child);

						newExpression->firstValue = std::make_shared<ListObject>(listVec);
						expressionTok->expression = newExpression;

						return { expressionTok };

					} else if (std::get<0>(vec[0]->token->token) == "{") {

						auto expressionTok = std::make_shared<ExpressionToken>();
						auto newExpression = std::make_shared<Expression>();

						std::vector<std::shared_ptr<MapStruct>> mapVec;
						this->_setMapArguments(mapVec, vec[0]->token->child[0]->child);

						newExpression->firstValue = std::make_shared<MapObject>(mapVec);
						expressionTok->expression = newExpression;

						return { expressionTok };
					}

				} else {
					auto newExpression = std::make_shared<Expression>();
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
		else throw std::runtime_error("SYNTAX ERROR: Invalid Expression");
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
				if (isInVec(tokStr, targetString)) {
					int leftMinus = 1;
					int rightAdd  = 2;

					bool isLeftParen  = false;
					bool isRightParen = false;

					if (vec[index-1]->token != nullptr && std::get<1>(vec[index-1]->token->token) == TokenType::OPEN_CASES)
						isLeftParen = true;
					if (vec[index+1]->token != nullptr && std::get<1>(vec[index+1]->token->token) == TokenType::OPEN_CASES)
						isRightParen = true;

					newVec  = { vec.begin(), vec.begin() + (index - leftMinus) };
					std::shared_ptr<ExpressionToken> newExpressionTok = std::make_shared<ExpressionToken>();
					std::shared_ptr<Expression     > newExpression = std::make_shared<Expression>();

					if (vec[index - 1]->expression != nullptr) newExpression->firstExpression = vec[index - 1]->expression;
					else if (isLeftParen) newExpression->secondExpression = this->_createExpression( { vec[index - 1] } )[0]->expression;
					else this->_setValueObjects(newExpression->firstValue, vec[index - 1]->token);

					newExpression->opWillUse = vec[index]->token;

					if (vec[index + 1]->expression != nullptr) newExpression->secondExpression = vec[index + 1]->expression;
					else if (isRightParen) newExpression->secondExpression = this->_createExpression( { vec[index + 1] } )[0]->expression;
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

	inline const void _printExpression(const std::shared_ptr<Expression>& expr, const std::string &space = "", int depth=0) {
		/**
		 * Might need a little rework. But good for now.
		 * 
		 */
		if (!expr) return;
		if (depth == 0) Log << space << "EXPRESSION: {\n";

		if (expr->firstValue) {
			Log << space << "  Value: ";
			JDM::checkAndCallReturnStringValue(expr->firstValue);
			Log << '\n';
		} else {
			Log << space << "  Expression: {\n";
			this->_printExpression(expr->firstExpression, space+"  ", depth+1);
			Log << space << "  }\n";
		}

		// if operator exist then thier is 100% a second value or expression
		if (expr->opWillUse) {
			Log << space << "  Operator: " << std::get<0>(expr->opWillUse->token) << '\n';
			if (expr->secondValue) {
				Log << space << "  Value: ";
				JDM::checkAndCallReturnStringValue(expr->secondValue);
				Log << '\n';
			} else if (expr->secondExpression) {
				Log << space << "  Expression: {\n";
				this->_printExpression(expr->secondExpression, space+"  ", depth+1);
				Log << space << "  }\n";
			}
		}
		if (depth == 0) Log << space << "}\n";
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
			case TokenType::VARIABLE: value = std::make_shared<VariableObjects>(tok); return;
			case TokenType::DECIMAL : value = std::make_shared<DoubleObjects  >(tok); return;
			case TokenType::INTEGER : value = std::make_shared<IntegerObjects >(tok); return;
			case TokenType::STRING  : value = std::make_shared<StringObjects  >(tok); return;
			default: throw std::runtime_error("SYNTAX ERROR: Invalid Value");
		}
	}

	inline const void _setArguments(
		std::vector<std::shared_ptr<Expression>> &vec,
		const std::vector<std::shared_ptr<TokenStruct>> &trav) {

		if (trav.empty()) return;
		auto it = std::find_if(trav.begin(), trav.end(), [](const std::shared_ptr<TokenStruct>& tok) {
			return std::get<1>(tok->token) == TokenType::COMMA_OPERATOR; });

		if (it != trav.end()) {

			std::vector<std::shared_ptr<TokenStruct>> turnToExpression = { trav.begin(), it };
			vec.push_back(this->_createExpression(this->_transformTokenStruct(turnToExpression))[0]->expression);

			if (it+1 != trav.end()) _setArguments(vec, { it+1, trav.end() });

		} else vec.push_back(this->_createExpression(this->_transformTokenStruct(trav))[0]->expression);
	}

	inline const void _setMapArguments(
		std::vector<std::shared_ptr<MapStruct>> &vec,
		const std::vector<std::shared_ptr<TokenStruct>> &trav) {

		if (trav.empty()) return;
		auto it = std::find_if(trav.begin(), trav.end(), [](const std::shared_ptr<TokenStruct>& tok) {
			return std::get<1>(tok->token) == TokenType::COMMA_OPERATOR; });

		if (it != trav.end()) {
			std::vector<std::shared_ptr<TokenStruct>> turnToExpression = { trav.begin(), it };
			this->_addMapVec(vec, turnToExpression);

			if (it+1 != trav.end()) _setMapArguments(vec, { it+1, trav.end() });
		} else this->_addMapVec(vec, trav);
	}

	inline const void _addMapVec(
		std::vector<std::shared_ptr<MapStruct>>& vec,
		const std::vector<std::shared_ptr<TokenStruct>>& newVec) {

		if (newVec.empty()) return;
		auto it = std::find_if(newVec.begin(), newVec.end(), [](const std::shared_ptr<TokenStruct>& tok) {
			return std::get<1>(tok->token) == TokenType::ARROW_OPERATOR; });

		if (it == newVec.end()) throw std::runtime_error("SYNTAX ERROR: Expecting '=>'");

		auto map = std::make_shared<MapStruct>();
		map->key   = this->_createExpression(this->_transformTokenStruct( { newVec.begin(), it } ))[0]->expression;
		map->value = this->_createExpression(this->_transformTokenStruct( { it+1, newVec.end() } ))[0]->expression;
		vec.push_back(map);
	}

	inline const void _setVarArgs(
		std::vector<std::shared_ptr<VariableObjects>> &vec,
		const std::vector<std::shared_ptr<TokenStruct>> &trav) {

		for (const auto &tok : trav) {
			if (std::get<1>(tok->token) == TokenType::COMMA_OPERATOR) continue;
			if (std::get<1>(tok->token) == TokenType::VARIABLE) vec.push_back(std::make_shared<VariableObjects>(tok));
			else throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE");
		}
	}

	inline const void _setTypeVarArgs(
		std::vector<std::shared_ptr<Variable>> &vec,
		const std::vector<std::shared_ptr<TokenStruct>> &trav) {

		if (trav.empty()) return;
		auto it = std::find_if(trav.begin(), trav.end(), [](const std::shared_ptr<TokenStruct>& tok) {
			return std::get<1>(tok->token) == TokenType::COMMA_OPERATOR; });

		if (it != trav.end()) {

			vec.push_back(this->_createVariable( { trav.begin(), it } ));
			if (it+1 != trav.end()) _setTypeVarArgs(vec, { it+1, trav.end() });
		} else vec.push_back(this->_createVariable( trav ));
	}

	inline const std::shared_ptr<Variable> _createVariable(
		const std::vector<std::shared_ptr<TokenStruct>> &turnToVariable) {
		std::vector<std::shared_ptr<TokenStruct>> newVec = turnToVariable;

		std::shared_ptr<Variable> newVar = std::make_shared<Variable>();
		if (newVec.empty()) throw std::runtime_error("SYNTAX ERROR: Expecting DATATYPE | VARIABLE");

		if (std::get<1>(newVec[0]->token) == TokenType::DATA_TYPE) {
			newVar->dataType = newVec[0];
			newVec = { newVec.begin() + 1, newVec.end() };
		}
		if (newVec.empty() || std::get<1>(newVec[0]->token) != TokenType::VARIABLE)
			throw std::runtime_error("SYNTAX ERROR: Expecting VARIABLE");
		newVar->varName = std::make_shared<VariableObjects>(newVec[0]);
		if (newVec.size() > 1) {
			if (std::get<1>(newVec[1]->token) == TokenType::ASSIGNMENT_OPERATOR)
				newVar->varValue = this->_createExpression(this->_transformTokenStruct( { newVec.begin() + 2, newVec.end() } ))[0]->expression;
			else throw std::runtime_error("SYNTAX ERROR: Expecting '=' or None");
		}
		return newVar;
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
