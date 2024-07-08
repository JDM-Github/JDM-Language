#pragma once

class JDM_DLL NativeFunction
{
public:
	enum NativeFunctionEnum
	{
		NATIVE_SPLIT,
		NATIVE_INPUT,
		NATIVE_LENGTH,
		NATIVE_ABS,
		NATIVE_CEIL,
		NATIVE_FLOOR,
		NATIVE_MAX,
		NATIVE_MIN,
		NATIVE_REVERSE,
		NATIVE_RANDOM_INT,
		NATIVE_RANDOM_FLOAT,
		NATIVE_SORT,
		NATIVE_ANY,
		NATIVE_ALL,
		NATIVE_ROUND,
		NATIVE_SUM,
		NATIVE_JOIN,
		NATIVE_INDEX,
		NATIVE_FILTER,
		NATIVE_UNIQUE,
		NATIVE_REDUCE,
		NATIVE_MAP,
		NATIVE_CHAIN,
		NATIVE_PARTIAL,
		NATIVE_SORT_IF,
		NATIVE_REPLACE,
		NATIVE_COMPARE,
		NATIVE_COUNT,
		NATIVE_GETTYPE,
		NATIVE_CAST,
		NATIVE_REFERENCE
	};
	JDM_DLL static std::unordered_map<std::string, NativeFunctionEnum> allNativeFunction;

	JDM_DLL static const std::shared_ptr<HigherObject> manageFunction(
		NativeFunction::NativeFunctionEnum nativeFuncType,
		const std::vector<std::shared_ptr<HigherObject>> &objects);

	JDM_DLL static const std::shared_ptr<HigherObject> split(
		const std::shared_ptr<HigherObject> &obj1,
		const std::string &stringObj = " ");

	JDM_DLL static const std::shared_ptr<HigherObject> input        (const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL static const std::shared_ptr<HigherObject> len          (const std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const std::shared_ptr<HigherObject> absolute     (const std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const std::shared_ptr<HigherObject> ceiling      (const std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const std::shared_ptr<HigherObject> floored      (const std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const std::shared_ptr<HigherObject> minimum      (const std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const std::shared_ptr<HigherObject> maxed        (const std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const std::shared_ptr<HigherObject> reverse      (const std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const std::shared_ptr<HigherObject> randint      (int start, int end);
	JDM_DLL static const std::shared_ptr<HigherObject> randfloat    (long double start, long double end);
	JDM_DLL static const std::shared_ptr<HigherObject> sort         (const std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const std::shared_ptr<HigherObject> anyOperator  (const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL static const std::shared_ptr<HigherObject> allOperator  (const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL static const std::shared_ptr<HigherObject> chain        (const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL static const std::shared_ptr<HigherObject> roundOperator(const std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const std::shared_ptr<HigherObject> sumOperator  (const std::shared_ptr<HigherObject> &obj1);

	JDM_DLL static const std::shared_ptr<HigherObject> joinOperator(
		const std::shared_ptr<HigherObject> &obj1,
		const std::string &stringObj = "");

	JDM_DLL static const std::shared_ptr<HigherObject> indexOperator(
		const std::shared_ptr<HigherObject> &obj1,
		const std::shared_ptr<HigherObject> &obj2);
};


