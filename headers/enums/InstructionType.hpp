#pragma once

enum InstructionType
{
	DefaultInstruction,
	LoggerInstruction,
	DeclarationInstruction,
	AssignmentInstruction,
	BlockInstruction,
	IfStatementInstruction,
	CreateFunctionInstruction,
	CallInstruction,
	ForLoopStatementInstruction,
	WhileStatementInstruction,
	ForEachStatementInstruction,
	ReturnInstruction,
	BreakInstruction,
	ContinueInstruction,
	CFunctionInstruction
};