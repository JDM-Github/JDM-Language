#pragma once

class JDM_DLL MathClass : public BaseNativeClass {
public:
	enum MathEnum
	{
		MATH_ADD,
		MATH_SUBTRACT,
		MATH_MULTIPLY,
		MATH_DIVIDE,
		MATH_MODULO,
		MATH_ABS,
		MATH_SQRT,
		MATH_POW,
		MATH_MAX,
		MATH_MIN,
		MATH_GCD,
		MATH_LCM,
		MATH_SIN,
		MATH_COS,
		MATH_TAN,
		MATH_LOG,
		MATH_EXP,
		MATH_FLOOR,
		MATH_CEIL,
		MATH_LOG10,
		MATH_LOG2,
		MATH_ASIN,
		MATH_ACOS,
		MATH_ATAN,
		MATH_ATAN2,
		MATH_SINH,
		MATH_COSH,
		MATH_TANH,
		MATH_MEAN,
		MATH_MEDIAN,
		MATH_VARIANCE,
		MATH_STDDEV,
		MATH_FACTORIAL,
		MATH_COMBIN,
		MATH_PERM,
		MATH_ROUND,
		MATH_GAMMA,
		MATH_BETA
	};

	JDM_DLL MathClass();
	JDM_DLL const void init();

	JDM_DLL inline std::shared_ptr<HigherObject> constructor(
		std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects)
	{
		throw std::runtime_error("Runtime Error: The 'Math' object is not callable.");
	}

	JDM_DLL std::shared_ptr<HigherObject> manageFunction(int funcType, std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL static int64_t factorial(int64_t value);
};
