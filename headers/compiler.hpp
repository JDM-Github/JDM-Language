#pragma once

#include "utils/HigherObject.hpp"
#include "library/NativeObject.hpp"
#include "library/ListObject.hpp"
#include "library/StringObject.hpp"
#include "library/MapObject.hpp"
#include "library/classes/baseClasses.hpp"

#include "utils/Comparator.hpp"

// #include "library/classes/fileClassObject.hpp"
// #include "library/classes/consoleClassObject.hpp"

class JDM_DLL Compiler
{
private:
	std::shared_ptr<Block> __mainBlock;

	// Collection of all include file, Stored in string
	std::vector<std::string> allInclude;
	std::unordered_map<std::string, std::shared_ptr<BaseNativeClass>> nativeClassMap =
	{
		// { "File"   , std::make_shared<FileClassFunctions>() },
		// { "Console", std::make_shared<ConsoleClass>() }
	};

	// Variable link
	struct VariableLink
	{
		std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> variables;
		std::unordered_map<std::string, std::shared_ptr<FunctionCall>> functionMap;
		std::shared_ptr<VariableLink> next;
		std::shared_ptr<VariableLink> prev;
	};
	std::shared_ptr<VariableLink> variable = std::make_shared<VariableLink>();
	bool breakLoop       = false;
	bool isAssigning     = false;
	int loopRunningCount = 0;

public:
	Compiler(const std::shared_ptr<Block> &mainBlock)
	 : __mainBlock(mainBlock)
	{
		this->compile(this->__mainBlock);
	}

	JDM_DLL const std::shared_ptr<HigherObject> compile(
		const std::shared_ptr<Block> &block,
		const std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_variables = {},
		const std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_AdditionalVariables = {},
		const std::unordered_map<std::string, std::shared_ptr<FunctionCall>> &_functionsAvail = {});

	JDM_DLL CVoid doDeclarationInstruction(const std::shared_ptr<Instruction> &instruction);
	JDM_DLL CVoid doAssignmentInstruction (const std::shared_ptr<Instruction> &instruction);
	JDM_DLL CVoid doLoggerInstruction     (const std::shared_ptr<Instruction> &instruction);
	JDM_DLL CVoid doCFunctionInstruction  (const std::shared_ptr<Instruction> &instruction);
	JDM_DLL CVoid processCreateFunction   (const std::shared_ptr<Instruction> &instruction);
	JDM_DLL CVoid processCallFunction     (const std::shared_ptr<Instruction> &instruction);

	JDM_DLL const std::shared_ptr<HigherObject> processIfStatement (const std::shared_ptr<Instruction> &instruction);
	JDM_DLL const std::shared_ptr<HigherObject> processForStatement(const std::shared_ptr<Instruction> &instruction);
	JDM_DLL const std::shared_ptr<HigherObject> processWhileLoop   (const std::shared_ptr<Instruction> &instruction);

	JDM_DLL const std::shared_ptr<HigherObject> processForEachStatement(const std::shared_ptr<Instruction> &instruction);

	JDM_DLL const std::shared_ptr<HigherObject> runFunction(
		const std::shared_ptr<FunctionCall> &newFunc,
		const std::vector<std::shared_ptr<HigherObject>> &arguments);

private:

	JDM_DLL const std::shared_ptr<HigherObject> handleNativeFunction(
		NativeFunction::NativeFunctionEnum nativeType,
		const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL const std::shared_ptr<HigherObject> recursivelyCall(
		const std::shared_ptr<CallObjects> &callObj,
		const std::shared_ptr<Expression> &expressionAssign = nullptr);
	JDM_DLL const std::shared_ptr<HigherObject> manageEndCall(
		const std::shared_ptr<CallObjects> &callObj,
		std::shared_ptr<HigherObject> &returnVal,
		const std::shared_ptr<Expression> &expressionAssign = nullptr);
	JDM_DLL const std::shared_ptr<HigherObject> manageCallBrackets(
		const std::shared_ptr<CallObjects > &next,
		const std::shared_ptr<HigherObject> &returnVal,
		const std::shared_ptr<Expression  > &expressionAssign = nullptr);

	// HANDLE VARIABLE, LIKE DECLARATION
	JDM_DLL const std::vector<std::shared_ptr<HigherObject>> getVectorHigherObject(
		const std::vector<std::shared_ptr<Expression>> &expr);
	JDM_DLL const void setupVariablePrevious();
	JDM_DLL const std::vector<std::string> getAllVarName(
		const std::unordered_map<std::string,
		std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> &_variables = {});
	JDM_DLL const void addVariable(
		const std::shared_ptr<Expression> &expr,
		DataTypeEnum dataT,
		const std::string &varName,
		bool isConst = false,
		bool isForce = false,
		const std::string &operation = "=");
	JDM_DLL const std::pair<DataTypeEnum, std::shared_ptr<HigherObject>> returnVariable(
		const std::shared_ptr<Expression> &expr,
		DataTypeEnum dataT);
	JDM_DLL const std::shared_ptr<HigherObject> castVariable(
		const std::shared_ptr<Expression> &expr,
		DataTypeEnum dataT,
		bool checkConstraint = true);
	JDM_DLL const std::shared_ptr<HigherObject> checkVariableConstraint(
		const std::shared_ptr<HigherObject> &var,
		DataTypeEnum dataT,
		bool checkConstraint = true);

	// Function to create a new operated object based on the provided operation and operands
	JDM_DLL std::shared_ptr<HigherObject> newOperatedObject(
		std::shared_ptr<HigherObject> &firstVal,
		const std::string &operation = "",
		const std::shared_ptr<HigherObject> &secondVal = nullptr);
	JDM_DLL void handleNewOperatedString(
		std::shared_ptr<HigherObject> &firstVal,
		const std::string &operation = "",
		const std::shared_ptr<HigherObject> &secondVal = nullptr);
	JDM_DLL void handleNewOperatedList  (
		std::shared_ptr<HigherObject> &firstVal,
		const std::string &operation = "",
		const std::shared_ptr<HigherObject> &secondVal = nullptr);
	JDM_DLL void handleNewOperatedMap   (
		std::shared_ptr<HigherObject> &firstVal,
		const std::string &operation = "",
		const std::shared_ptr<HigherObject> &secondVal = nullptr);
	JDM_DLL void handleNewOperatedFunc  (
		std::shared_ptr<HigherObject> &firstVal,
		const std::string &operation = "",
		const std::shared_ptr<HigherObject> &secondVal = nullptr);
	JDM_DLL void handleNewOperatedNumber(
		std::shared_ptr<HigherObject> &firstVal,
		const std::string &operation = "",
		const std::shared_ptr<HigherObject> &secondVal = nullptr);
	JDM_DLL void handleNewOperatedObject(
		std::shared_ptr<HigherObject> &firstVal,
		const std::string &operation = "",
		const std::shared_ptr<HigherObject> &secondVal = nullptr);

	// Used to get the HigherObject
	// This will return value of variable or expression
	JDM_DLL std::shared_ptr<HigherObject> getHigherObject(
		const std::shared_ptr<VarObjects> &Value,
		const std::shared_ptr<Expression> &Expression);
	JDM_DLL std::shared_ptr<HigherObject> &getVariableObject(const std::shared_ptr<Expression> &expr);
	JDM_DLL std::shared_ptr<HigherObject> evaluateExpression(const std::shared_ptr<Expression> &expr);
};
