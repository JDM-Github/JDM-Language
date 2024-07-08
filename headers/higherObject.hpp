#pragma once
#include "instruction.hpp"

class JDM_DLL HigherObject
{
public:
	struct FunctionCall
	{
		StringStd funcName;
		std::shared_ptr<Block> blockWillRun;
		std::vector<StringStd> varNameAccesible;
		std::vector<std::shared_ptr<HigherObject>> preArgs;
		std::unordered_map<
			StringStd,
			std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>
		> parameters;
	};
	struct ClassObject
	{
		StringStd className;
		std::unordered_map<StringStd, std::shared_ptr<HigherObject>> members;
		std::unordered_map<StringStd, std::shared_ptr<FunctionCall>> methods;
		bool fromMainSource = true;
	};
	enum ActiveDataType
	{
		NON_NONE,
		ACTIVE_STRING,
		ACTIVE_INTEGER,
		ACTIVE_DECIMAL,
		ACTIVE_BOOLEAN,
		ACTIVE_FUNCTION,
		ACTIVE_LIST,
		ACTIVE_MAP,
		ACTIVE_OBJECT
	};

	bool isListSorted = false;
	bool isString     = false;
	bool isInteger    = false;
	bool isDecimal    = false;
	bool isBoolean    = false;
	bool isFunc       = false;
	bool isList       = false;
	bool isMap        = false;
	bool isObject     = false;

	ActiveDataType currActive = NON_NONE;
	bool isForcedConstraint = false;
	bool isConstant         = false;

	std::vector<DataTypeEnum> listConstraint = {};
	std::vector<DataTypeEnum> mapConstraint  = {};

	StringStd      stringValue  = "";
	int64_t     integerValue = 0;
	long double doubleValue  = 0.0;
	bool        booleanValue = false;

	std::shared_ptr<FunctionCall> funcValue;
	std::shared_ptr<ClassObject > objectValue;

	std::vector<std::shared_ptr<HigherObject>> listValue = {};
	std::unordered_map<std::shared_ptr<HigherObject>,
		std::shared_ptr<HigherObject>> mapValue  = {};

public:
	JDM_DLL HigherObject();
	JDM_DLL HigherObject(CStringRef value );
	JDM_DLL HigherObject(const int64_t value      );
	JDM_DLL HigherObject(const long double value  );
	JDM_DLL HigherObject(const bool value         );
	JDM_DLL HigherObject(const std::shared_ptr<FunctionCall> &value);
	JDM_DLL HigherObject(const std::shared_ptr<ClassObject > &value);
	JDM_DLL HigherObject(const std::vector<std::shared_ptr<HigherObject>> &value);
	JDM_DLL HigherObject(const std::unordered_map<std::shared_ptr<HigherObject>, std::shared_ptr<HigherObject>> &value);
	JDM_DLL HigherObject(const std::shared_ptr<HigherObject> &obj);
	virtual ~HigherObject() {}

	JDM_DLL const void setHigherObject(const std::shared_ptr<HigherObject> &obj);
	JDM_DLL const DataTypeEnum getDatatypeEnum();
	JDM_DLL CString getType();
	JDM_DLL CString _getStringValue();

	JDM_DLL const void logValue();
	JDM_DLL const void refalseAll();
	JDM_DLL const bool isInList(const std::shared_ptr<HigherObject> &obj);
	JDM_DLL const void sortList();
	JDM_DLL const bool compareHigherObject(const std::shared_ptr<HigherObject> &obj);

	JDM_DLL const std::shared_ptr<HigherObject> isInMap(const std::shared_ptr<HigherObject> &obj, const bool getFirst = false);

	JDM_DLL const void castToString();
	JDM_DLL const void castToInteger();
	JDM_DLL const void castToDecimal();
	JDM_DLL const void castToBoolean();
	JDM_DLL const void castToFunction();
	JDM_DLL const void castToObject();
	JDM_DLL const void castToList();
	JDM_DLL const void castToMap() ;
};
