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
	SharedTokenStruct         __rootTokens;
	Vector<SharedTokenStruct> __blockTokens;
	SharedPtr<Block         > __mainBlock;

	struct IfStatementLink {
		SharedPtr<IfStatement    > current;
		SharedPtr<IfStatementLink> next;
		SharedPtr<IfStatementLink> prev;
	};
	SharedPtr<IfStatementLink> __currIfLink;

public:
	JDM_DLL       Parser    (CSharedTokenStructRef tokens);
	JDM_DLL CVoid analyzeAST(CSharedPtrRef<Block> block, SpaceString space = "");
	JDM_DLL SharedPtr<Block> getAST() { return this->__mainBlock; }

private:
	JDM_DLL CSharedPtr<Block> _getBlock           (CSharedTokenLinkRef tokenS);
	JDM_DLL CVoid             _predictInstruction (CSharedPtrRef<Block> block, CVectorRef<SharedTokenStruct> tokens);

/**
 * 
 * MANAGE TOKENS
 * 
 */
private:
	JDM_DLL CVoid _manageControlFlow         (CSharedPtrRef<Block> block, CSharedTokenStructRef controlType, CVectorRef<SharedTokenStruct> tokenS);
	JDM_DLL CVoid _manageCreateFunction      (CSharedPtrRef<Block> block, CSharedTokenStructRef funcName, CVectorRef<SharedTokenStruct> tokens);
	JDM_DLL CVoid _manageForEachLoop         (CSharedPtrRef<Block> block, CVectorRef<SharedTokenStruct> tokens);
	JDM_DLL CVoid _manageDataType            (CSharedPtrRef<Block> block, CSharedTokenStructRef dataType, CSharedTokenStructRef varName, CVectorRef<SharedTokenStruct> tokenS);
	JDM_DLL CVoid _manageVariable            (CSharedPtrRef<Block> block, CVectorRef<SharedTokenStruct> tokens);
	JDM_DLL CVoid _manageIfElseWhileStatement(CSharedPtrRef<Block> block, CVectorRef<SharedTokenStruct> tokens, ControlFlowEnum control);
	JDM_DLL CVoid _manageForloop             (CSharedPtrRef<Block> block, CSharedTokenStructRef varName, CVectorRef<SharedTokenStruct> tokens);
	JDM_DLL CVoid _manageLogger              (CSharedPtrRef<Block> block, CVectorRef<SharedTokenStruct> tokenS, VecSharedPtrRef<Expression> expressions);

/**
 * 
 * USEFUL FUNCTIONS
 * 
 */
private:
	JDM_DLL CBool                          _isBlockCurly            (CSharedTokenStructRef token);
	JDM_DLL CBool                          _findAndReplaceLambdaCall(CVecSharedPtrRef<ExpressionToken> vec, VecSharedPtrRef<ExpressionToken> newVec, bool willNeedToCall = true);
	JDM_DLL CVecSharedPtr<ExpressionToken> _getAllTokenFromSide     (CVecSharedPtrRef<ExpressionToken> vec, int index, bool isLeft = false);
	JDM_DLL CVecSharedPtr<ExpressionToken> _transformTokenStruct    (CVectorRef<SharedTokenStruct> tokenS);
	JDM_DLL CBool                          _findAndReplaceExpression(CVecTokenStrRef targetString, CVecSharedPtrRef<ExpressionToken> vec, VecSharedPtrRef<ExpressionToken> newVec);
	JDM_DLL CVoid                          _getExpressionFromTokens (CVecSharedPtrRef<ExpressionToken> exprToken, SharedPtrRef<Expression> expression, SharedPtrRef<VarObjects> value);
	JDM_DLL CVoid                          _setValueObjects         (SharedPtrRef<VarObjects> value, CSharedTokenStructRef tok);

/*
 * 
 * SETTING VALUES
 * 
 */
private:
	JDM_DLL CVoid _setVarArgs     (VecSharedPtrRef<VariableObjects> vec, CVectorRef<SharedTokenStruct> trav);
	JDM_DLL CVoid _setArguments   (VecSharedPtrRef<Expression> vec, CVectorRef<SharedTokenStruct> trav);
	JDM_DLL CVoid _setMapArguments(VecSharedPtrRef<MapStruct> vec, CVectorRef<SharedTokenStruct> trav);
	JDM_DLL CVoid _setTypeVarArgs (VecSharedPtrRef<Variable> vec, CVectorRef<SharedTokenStruct> trav);

/**
 * 
 * CREATE PURPOSE
 * 
 */
private:
	JDM_DLL CSharedPtr<Block      > _createBlockOrLine   (CSharedTokenStructRef currToken, CVectorRef<SharedTokenStruct> tokens);
	JDM_DLL CSharedPtr<Expression > _createListExpression(CVectorRef<SharedTokenStruct> tokens);
	JDM_DLL CSharedPtr<Expression > _createMapExpression (CVectorRef<SharedTokenStruct> tokens);
	JDM_DLL CSharedPtr<CallObjects> _createCallObject    (CVecSharedPtrRef<ExpressionToken> turnToCallObject, CSharedPtrRef<CallObjects> previous = nullptr, CBool isAssigning = false);
	JDM_DLL CSharedPtr<MapStruct  > _createMap           (CVectorRef<SharedTokenStruct> turnToMap);
	JDM_DLL CSharedPtr<Variable   > _createVariable      (CVectorRef<SharedTokenStruct> turnToVariable);
	JDM_DLL CVecSharedPtr<ExpressionToken> _createExpression(
		CVecSharedPtrRef<ExpressionToken> vec,
		bool checkInvalid = true, bool lambdaBool = true,
		bool powerBool    = true, bool mulDivBool = true,
		bool addSubBool   = true, bool notBool    = true,
		bool shiftBool    = true, bool addBool    = true,
		bool xorBool      = true, bool orBool     = true,
		bool logicalBool  = true, bool relNotBool = true,
		bool relAndBool   = true, bool relOrBool  = true
	);

/**
 * 
 * DEBUGGING PURPOSE
 * 
 */
private:
	JDM_DLL CVoid _analyzeAllIfElse(CSharedPtrRef<IfStatement> instruction, SpaceString space);
	JDM_DLL const char *_instructionToString(InstructionType instruction);
	JDM_DLL CVoid _printExpression(CSharedPtrRef<Expression> expr, SpaceString space = "", int depth=0);
};
