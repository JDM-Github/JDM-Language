#pragma once

#include "utils/HigherObject.hpp"
#include "library/NativeObject.hpp"
#include "library/classes/BaseClasses.hpp"
#include "struct/VariableLink.hpp"

class JDM_DLL Compiler
{
public:
	JDM_DLL Compiler(const std::shared_ptr<Block> &mainBlock);
	JDM_DLL CSharedHigherObject compile(
		const std::shared_ptr<Block> &block,
		const std::unordered_map<std::string, std::pair<DataTypeEnum, SharedHigherObject>> &_variables = {},
		const std::unordered_map<std::string, std::pair<DataTypeEnum, SharedHigherObject>> &_AdditionalVariables = {},
		const std::unordered_map<std::string, std::shared_ptr<FunctionCall>> &_functionsAvail = {});

private:
	std::shared_ptr<Block> __mainBlock;

	// Collection of all include file, Stored in string
	std::vector<std::string> __allInclude;
	std::unordered_map<std::string, std::shared_ptr<BaseNativeClass>> __nativeClassMap;
	std::shared_ptr<VariableLink> __variable = std::make_shared<VariableLink>();

	bool __breakLoop        = false;
	bool __isAssigning      = false;
	int  __loopRunningCount = 0;

private:
	JDM_DLL const void _initializeVariables();
	JDM_DLL const void setupVariablePrevious();

	JDM_DLL CVoid _doDeclarationInstruction(const std::shared_ptr<Instruction> &instruction);
	JDM_DLL CVoid _doAssignmentInstruction (const std::shared_ptr<Instruction> &instruction);
	JDM_DLL CVoid _doLoggerInstruction     (const std::shared_ptr<Instruction> &instruction);
	JDM_DLL CVoid _doCFunctionInstruction  (const std::shared_ptr<Instruction> &instruction);
	JDM_DLL CVoid _processCreateFunction   (const std::shared_ptr<Instruction> &instruction);
	JDM_DLL CVoid _processCallFunction     (const std::shared_ptr<Instruction> &instruction);

	JDM_DLL CSharedHigherObject _processIfStatement     (const std::shared_ptr<Instruction> &instruction);
	JDM_DLL CSharedHigherObject _processForStatement    (const std::shared_ptr<Instruction> &instruction);
	JDM_DLL CSharedHigherObject _processWhileLoop       (const std::shared_ptr<Instruction> &instruction);
	JDM_DLL CSharedHigherObject _processForEachStatement(const std::shared_ptr<Instruction> &instruction);

	JDM_DLL CSharedHigherObject _runFunction(
		const std::shared_ptr<FunctionCall> &newFunc,
		const std::vector<SharedHigherObject> &arguments);

	JDM_DLL CSharedHigherObject _handleNativeFunction(NativeFunction::NativeFunctionEnum nativeType, const std::vector<SharedHigherObject> &objects);
	JDM_DLL CSharedHigherObject _recursivelyCall     (const std::shared_ptr<CallObjects> &callObj, const std::shared_ptr<Expression> &expressionAssign = nullptr);
	JDM_DLL CSharedHigherObject _manageEndCall       (const std::shared_ptr<CallObjects> &callObj, SharedHigherObject &returnVal, const std::shared_ptr<Expression> &expressionAssign = nullptr);
	JDM_DLL CSharedHigherObject _manageCallBrackets  (const std::shared_ptr<CallObjects > &next, CSharedHigherObject &returnVal, const std::shared_ptr<Expression  > &expressionAssign = nullptr);

	// HANDLE VARIABLE, LIKE DECLARATION
	JDM_DLL const std::vector<SharedHigherObject> _getVectorHigherObject(const std::vector<std::shared_ptr<Expression>> &expr);

	JDM_DLL const std::vector<std::string> _getAllVarName(const std::unordered_map<std::string, std::pair<DataTypeEnum, SharedHigherObject>> &_variables = {});
	JDM_DLL const void _addVariable(const std::shared_ptr<Expression> &expr, DataTypeEnum dataT, const std::string &varName, bool isConst = false, bool isForce = false, const std::string &operation = "=");
	JDM_DLL const std::pair<DataTypeEnum, SharedHigherObject> _returnVariable(const std::shared_ptr<Expression> &expr, DataTypeEnum dataT);
	JDM_DLL CSharedHigherObject _castVariable(const std::shared_ptr<Expression> &expr, DataTypeEnum dataT, bool checkConstraint = true);
	JDM_DLL CSharedHigherObject _checkVariableConstraint(CSharedHigherObject &var, DataTypeEnum dataT, bool checkConstraint = true);

	// Function to create a new operated object based on the provided operation and operands
	JDM_DLL SharedHigherObject _newOperatedObject(SharedHigherObject &firstVal, const std::string &operation = "", CSharedHigherObject &secondVal = nullptr);
	JDM_DLL void _handleNewOperatedString(SharedHigherObject &firstVal, const std::string &operation = "", CSharedHigherObject &secondVal = nullptr);
	JDM_DLL void _handleNewOperatedList  (SharedHigherObject &firstVal, const std::string &operation = "", CSharedHigherObject &secondVal = nullptr);
	JDM_DLL void _handleNewOperatedMap   (SharedHigherObject &firstVal, const std::string &operation = "", CSharedHigherObject &secondVal = nullptr);
	JDM_DLL void _handleNewOperatedFunc  (SharedHigherObject &firstVal, const std::string &operation = "", CSharedHigherObject &secondVal = nullptr);
	JDM_DLL void _handleNewOperatedNumber(SharedHigherObject &firstVal, const std::string &operation = "", CSharedHigherObject &secondVal = nullptr);
	JDM_DLL void _handleNewOperatedObject(SharedHigherObject &firstVal, const std::string &operation = "", CSharedHigherObject &secondVal = nullptr);

	// Used to get the HigherObject
	// This will return value of variable or expression
	JDM_DLL SharedHigherObject _getHigherObject    (const std::shared_ptr<VarObjects> &Value, const std::shared_ptr<Expression> &Expression);
	JDM_DLL SharedHigherObject &_getVariableObject (const std::shared_ptr<Expression> &expr);
	JDM_DLL SharedHigherObject _evaluateExpression(const std::shared_ptr<Expression> &expr);
};
