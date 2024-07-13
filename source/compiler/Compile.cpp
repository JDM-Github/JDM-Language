#include "Compiler.hpp"

JDM_DLL
const std::shared_ptr<HigherObject> Compiler::compile(
	const std::shared_ptr<Block> &block,
	const std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_variables,
	const std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_AdditionalVariables,
	const std::unordered_map<std::string, std::shared_ptr<FunctionCall>> &_functionsAvail)
{
	// Use for scoping
	if (this->variable == nullptr)
		this->variable = std::make_shared<VariableLink>();
	else
	{
		this->variable->next = std::make_shared<VariableLink>();
		this->variable->next->prev = this->variable;
		this->variable = this->variable->next;
	}
	this->variable->variables = _variables;
	for (const auto& element : _AdditionalVariables)
	{
		this->variable->variables[element.first] = element.second;
	}
	this->variable->functionMap = _functionsAvail;


	bool willReturn = false;
	std::shared_ptr<HigherObject> valueToReturn = nullptr;
	for (const auto &instruction : block->instruction)
	{
		// Reset the break and continue
		this->breakLoop   = false;
		this->isAssigning = false;
		if      (instruction->getType() == DeclarationInstruction   ) this->doDeclarationInstruction(instruction);
		else if (instruction->getType() == AssignmentInstruction    ) this->doAssignmentInstruction(instruction);
		else if (instruction->getType() == LoggerInstruction        ) this->doLoggerInstruction(instruction);
		else if (instruction->getType() == CFunctionInstruction     ) this->doCFunctionInstruction(instruction);
		else if (instruction->getType() == CreateFunctionInstruction) this->processCreateFunction(instruction);
		else if (instruction->getType() == CallInstruction          ) this->processCallFunction(instruction);
		else if (instruction->getType() == IfStatementInstruction   )
		{
			valueToReturn = this->processIfStatement(instruction);
			if (valueToReturn != nullptr)
				willReturn = true;
		}
		else if (instruction->getType() == ForLoopStatementInstruction)
		{
			valueToReturn = this->processForStatement(instruction);
			if (valueToReturn != nullptr)
				willReturn = true;
		}
		else if (instruction->getType() == ForEachStatementInstruction)
		{
			valueToReturn = this->processForEachStatement(instruction);
			if (valueToReturn != nullptr)
				willReturn = true;
		}
		else if (instruction->getType() == WhileStatementInstruction)
		{
			auto valueToReturn = this->processWhileLoop(instruction);
			if (valueToReturn != nullptr)
				willReturn = true;
		}
		else if (instruction->getType() == BreakInstruction)
		{
			if (this->loopRunningCount > 0)
			{
				willReturn = true;
				this->breakLoop = true;
			}
		}
		else if (instruction->getType() == ContinueInstruction)
		{
			if (this->loopRunningCount > 0)
				willReturn = true;
		}
		else if (instruction->getType() == ReturnInstruction)
		{
			if (block == this->__mainBlock)
				throw std::runtime_error("Runtime Error: Cannot return outside Function.");

			auto returnObj = std::dynamic_pointer_cast<Return>(instruction);
			valueToReturn  = this->evaluateExpression(returnObj->returnValue);
			willReturn     = true;
		}
		if (willReturn)
		{
			this->setupVariablePrevious();
			return valueToReturn;
		}
	}
	this->setupVariablePrevious();
	return nullptr;
}
