#include "utils/HigherObject.hpp"
#include "library/classes/Math.hpp"

JDM_DLL
MathClass::MathClass()
{
	this->mapFunctions["add"      ] = MathEnum::MATH_ADD;
	this->mapFunctions["subtract" ] = MathEnum::MATH_SUBTRACT;
	this->mapFunctions["multiply" ] = MathEnum::MATH_MULTIPLY;
	this->mapFunctions["divide"   ] = MathEnum::MATH_DIVIDE;
	this->mapFunctions["modulo"   ] = MathEnum::MATH_MODULO;
	this->mapFunctions["abs"      ] = MathEnum::MATH_ABS;
	this->mapFunctions["sqrt"     ] = MathEnum::MATH_SQRT;
	this->mapFunctions["pow"      ] = MathEnum::MATH_POW;
	this->mapFunctions["max"      ] = MathEnum::MATH_MAX;
	this->mapFunctions["min"      ] = MathEnum::MATH_MIN;
	this->mapFunctions["gcd"      ] = MathEnum::MATH_GCD;
	this->mapFunctions["lcm"      ] = MathEnum::MATH_LCM;
	this->mapFunctions["sin"      ] = MathEnum::MATH_SIN;
	this->mapFunctions["cos"      ] = MathEnum::MATH_COS;
	this->mapFunctions["tan"      ] = MathEnum::MATH_TAN;
	this->mapFunctions["log"      ] = MathEnum::MATH_LOG;
	this->mapFunctions["exp"      ] = MathEnum::MATH_EXP;
	this->mapFunctions["floor"    ] = MathEnum::MATH_FLOOR;
	this->mapFunctions["ceil"     ] = MathEnum::MATH_CEIL;
	this->mapFunctions["log10"    ] = MathEnum::MATH_LOG10;
    this->mapFunctions["log2"     ] = MathEnum::MATH_LOG2;
    this->mapFunctions["asin"     ] = MathEnum::MATH_ASIN;
    this->mapFunctions["acos"     ] = MathEnum::MATH_ACOS;
    this->mapFunctions["atan"     ] = MathEnum::MATH_ATAN;
    this->mapFunctions["atan2"    ] = MathEnum::MATH_ATAN2;
    this->mapFunctions["sinh"     ] = MathEnum::MATH_SINH;
    this->mapFunctions["cosh"     ] = MathEnum::MATH_COSH;
    this->mapFunctions["tanh"     ] = MathEnum::MATH_TANH;
    this->mapFunctions["mean"     ] = MathEnum::MATH_MEAN;
    this->mapFunctions["median"   ] = MathEnum::MATH_MEDIAN;
    this->mapFunctions["variance" ] = MathEnum::MATH_VARIANCE;
    this->mapFunctions["stddev"   ] = MathEnum::MATH_STDDEV;
    this->mapFunctions["factorial"] = MathEnum::MATH_FACTORIAL;
    this->mapFunctions["combin"   ] = MathEnum::MATH_COMBIN;
    this->mapFunctions["perm"     ] = MathEnum::MATH_PERM;
    this->mapFunctions["round"    ] = MathEnum::MATH_ROUND;
    this->mapFunctions["gamma"    ] = MathEnum::MATH_GAMMA;
    this->mapFunctions["beta"     ] = MathEnum::MATH_BETA;
}

JDM_DLL
std::shared_ptr<HigherObject> MathClass::manageFunction(
	int funcType,
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	MathEnum newfuncType = static_cast<MathEnum>(funcType);
	if (newfuncType == MathClass::MathEnum::MATH_ADD)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments (Number, Number).");

		long double sumOfTwo = 0;
		for (const auto& obj : objects)
		{
			auto active = obj->getCurrActive();
			if (active == ACTIVE_INTEGER)
				sumOfTwo += obj->integerValue;
			else if (active == ACTIVE_DECIMAL)
				sumOfTwo += obj->doubleValue;
			else if (active == ACTIVE_BOOLEAN)
				sumOfTwo += obj->booleanValue;
			else
				throw std::runtime_error("Runtime Error: Each argument must be a 'Number'");
		}
		return std::make_shared<HigherObject>(sumOfTwo);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_SUBTRACT)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments (Number, Number).");

		auto active1 = objects[0]->getCurrActive();
		auto active2 = objects[1]->getCurrActive();
		long double value1 = (active1 == ACTIVE_INTEGER) ? objects[0]->integerValue :
							 (active1 == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							 (active1 == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double value2 = (active2 == ACTIVE_INTEGER) ? objects[1]->integerValue :
							 (active2 == ACTIVE_DECIMAL) ? objects[1]->doubleValue :
							 (active2 == ACTIVE_BOOLEAN) ? objects[1]->booleanValue : 0;
		return std::make_shared<HigherObject>(value1 - value2);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_MULTIPLY)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments (Number, Number).");

		long double product = 1;
		for (const auto& obj : objects)
		{
			auto active = obj->getCurrActive();
			if (active == ACTIVE_INTEGER)
				product *= obj->integerValue;
			else if (active == ACTIVE_DECIMAL)
				product *= obj->doubleValue;
			else if (active == ACTIVE_BOOLEAN)
				product *= obj->booleanValue;
			else
				throw std::runtime_error("Runtime Error: Each argument must be a 'Number'");
		}
		return std::make_shared<HigherObject>(product);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_DIVIDE)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments (Number, Number).");

		auto active1 = objects[0]->getCurrActive();
		auto active2 = objects[1]->getCurrActive();
		long double value1 = (active1 == ACTIVE_INTEGER) ? objects[0]->integerValue :
							 (active1 == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							 (active1 == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double value2 = (active2 == ACTIVE_INTEGER) ? objects[1]->integerValue :
							 (active2 == ACTIVE_DECIMAL) ? objects[1]->doubleValue :
							 (active2 == ACTIVE_BOOLEAN) ? objects[1]->booleanValue : 0;
		if (value2 == 0)
			throw std::runtime_error("Runtime Error: Division by zero.");
			
		return std::make_shared<HigherObject>(value1 / value2);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_MODULO)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments (Number, Number).");

		auto active1 = objects[0]->getCurrActive();
		auto active2 = objects[1]->getCurrActive();
		long double value1 = (active1 == ACTIVE_INTEGER) ? objects[0]->integerValue :
							 (active1 == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							 (active1 == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double value2 = (active2 == ACTIVE_INTEGER) ? objects[1]->integerValue :
							 (active2 == ACTIVE_DECIMAL) ? objects[1]->doubleValue :
							 (active2 == ACTIVE_BOOLEAN) ? objects[1]->booleanValue : 0;
		if (value2 == 0)
			throw std::runtime_error("Runtime Error: Division by zero.");

		long double result = std::fmod(value1, value2);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_ABS)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double result = std::abs(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_SQRT)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		if (value < 0)
			throw std::runtime_error("Runtime Error: Cannot compute square root of a negative number.");
		
		long double result = std::sqrt(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_POW)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments (Number, Number).");

		auto active1 = objects[0]->getCurrActive();
		auto active2 = objects[1]->getCurrActive();
		long double base = (active1 == ACTIVE_INTEGER) ? objects[0]->integerValue :
						   (active1 == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
						   (active1 == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double exponent = (active2 == ACTIVE_INTEGER) ? objects[1]->integerValue :
							   (active2 == ACTIVE_DECIMAL) ? objects[1]->doubleValue :
							   (active2 == ACTIVE_BOOLEAN) ? objects[1]->booleanValue : 0;
		long double result = std::pow(base, exponent);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_MAX)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments (Number, Number).");

		auto active1 = objects[0]->getCurrActive();
		auto active2 = objects[1]->getCurrActive();
		long double value1 = (active1 == ACTIVE_INTEGER) ? objects[0]->integerValue :
							 (active1 == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							 (active1 == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double value2 = (active2 == ACTIVE_INTEGER) ? objects[1]->integerValue :
							 (active2 == ACTIVE_DECIMAL) ? objects[1]->doubleValue :
							 (active2 == ACTIVE_BOOLEAN) ? objects[1]->booleanValue : 0;
		long double result = std::max(value1, value2);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_MIN)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments (Number, Number).");

		auto active1 = objects[0]->getCurrActive();
		auto active2 = objects[1]->getCurrActive();
		long double value1 = (active1 == ACTIVE_INTEGER) ? objects[0]->integerValue :
							 (active1 == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							 (active1 == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double value2 = (active2 == ACTIVE_INTEGER) ? objects[1]->integerValue :
							 (active2 == ACTIVE_DECIMAL) ? objects[1]->doubleValue :
							 (active2 == ACTIVE_BOOLEAN) ? objects[1]->booleanValue : 0;
		long double result = std::min(value1, value2);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_GCD)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments (Number, Number).");

		auto active1 = objects[0]->getCurrActive();
		auto active2 = objects[1]->getCurrActive();
		int value1 = (active1 == ACTIVE_INTEGER) ? objects[0]->integerValue : 0;
		int value2 = (active2 == ACTIVE_INTEGER) ? objects[1]->integerValue : 0;
		if (value1 == 0 || value2 == 0)
			throw std::runtime_error("Runtime Error: Both arguments must be non-zero integers.");

		int64_t result = std::gcd(value1, value2);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_LCM)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments (Number, Number).");

		auto active1 = objects[0]->getCurrActive();
		auto active2 = objects[1]->getCurrActive();
		int value1 = (active1 == ACTIVE_INTEGER) ? objects[0]->integerValue : 0;
		int value2 = (active2 == ACTIVE_INTEGER) ? objects[1]->integerValue : 0;
		if (value1 == 0 || value2 == 0)
			throw std::runtime_error("Runtime Error: Both arguments must be non-zero integers.");

		int64_t result = std::lcm(value1, value2);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_SIN)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double result = std::sin(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_COS)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double result = std::cos(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_TAN)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double result = std::tan(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_LOG)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		if (value <= 0)
			throw std::runtime_error("Runtime Error: Logarithm of non-positive number is undefined.");

		long double result = std::log(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_EXP)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double result = std::exp(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_FLOOR)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double result = std::floor(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathClass::MathEnum::MATH_CEIL)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double result = std::ceil(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathEnum::MATH_LOG10)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		if (value <= 0)
			throw std::runtime_error("Runtime Error: Logarithm base 10 of non-positive number is undefined.");
		long double result = std::log10(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathEnum::MATH_LOG2)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		if (value <= 0)
			throw std::runtime_error("Runtime Error: Logarithm base 2 of non-positive number is undefined.");
		long double result = std::log2(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathEnum::MATH_ASIN)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		if (value < -1 || value > 1)
			throw std::runtime_error("Runtime Error: Arcsine domain error.");
		
		long double result = std::asin(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathEnum::MATH_ACOS)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		if (value < -1 || value > 1)
			throw std::runtime_error("Runtime Error: Arccosine domain error.");
		
		long double result = std::acos(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathEnum::MATH_ATAN)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double result = std::atan(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathEnum::MATH_ATAN2)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments (Number, Number).");

		auto active1 = objects[0]->getCurrActive();
		auto active2 = objects[1]->getCurrActive();
		long double y = (active1 == ACTIVE_INTEGER) ? objects[0]->integerValue :
						(active1 == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
						(active1 == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double x = (active2 == ACTIVE_INTEGER) ? objects[1]->integerValue :
						(active2 == ACTIVE_DECIMAL) ? objects[1]->doubleValue :
						(active2 == ACTIVE_BOOLEAN) ? objects[1]->booleanValue : 0;
		long double result = std::atan2(y, x);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathEnum::MATH_SINH)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double result = std::sinh(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathEnum::MATH_COSH)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double result = std::cosh(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathEnum::MATH_TANH)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double result = std::tanh(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathEnum::MATH_MEAN)
	{
		if (objects.empty())
			throw std::runtime_error("Runtime Error: Expecting at least 1 argument (Number).");

		long double sum = 0;
		for (const auto& obj : objects) {
			auto active = obj->getCurrActive();
			long double value = (active == ACTIVE_INTEGER) ? obj->integerValue :
								(active == ACTIVE_DECIMAL) ? obj->doubleValue :
								(active == ACTIVE_BOOLEAN) ? obj->booleanValue : 0;
			sum += value;
		}
		long double result = sum / objects.size();
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathEnum::MATH_MEDIAN)
	{
		if (objects.empty())
			throw std::runtime_error("Runtime Error: Expecting at least 1 argument (Number).");

		std::vector<long double> values;
		for (const auto& obj : objects)
		{
			auto active = obj->getCurrActive();
			long double value = (active == ACTIVE_INTEGER) ? obj->integerValue :
								(active == ACTIVE_DECIMAL) ? obj->doubleValue :
								(active == ACTIVE_BOOLEAN) ? obj->booleanValue : 0;
			values.push_back(value);
		}
		std::sort(values.begin(), values.end());
		long double median;
		size_t size = values.size();
		if (size % 2 == 0)
			median = (values[size / 2 - 1] + values[size / 2]) / 2;
		else
			median = values[size / 2];
		return std::make_shared<HigherObject>(median);
	}
	else if (newfuncType == MathEnum::MATH_VARIANCE)
	{
		if (objects.empty())
			throw std::runtime_error("Runtime Error: Expecting at least 1 argument (Number).");

		std::vector<long double> values;
		long double mean = 0;
		for (const auto& obj : objects)
		{
			auto active = obj->getCurrActive();
			long double value = (active == ACTIVE_INTEGER) ? obj->integerValue :
								(active == ACTIVE_DECIMAL) ? obj->doubleValue :
								(active == ACTIVE_BOOLEAN) ? obj->booleanValue : 0;
			values.push_back(value);
			mean += value;
		}
		mean /= values.size();
		long double variance = 0;
		for (const auto& value : values)
		{
			variance += std::pow(value - mean, 2);
		}
		variance /= values.size();
		return std::make_shared<HigherObject>(variance);
	}
	else if (newfuncType == MathEnum::MATH_STDDEV)
	{
		if (objects.empty())
			throw std::runtime_error("Runtime Error: Expecting at least 1 argument (Number).");

		auto varianceObj = this->manageFunction(MathEnum::MATH_VARIANCE, obj1, objects);

		long double variance = varianceObj->getCurrActive() == ACTIVE_DECIMAL ?
			varianceObj->doubleValue : varianceObj->integerValue;
		long double stddev = std::sqrt(variance);
		return std::make_shared<HigherObject>(stddev);
	}
	else if (newfuncType == MathEnum::MATH_FACTORIAL)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Integer).");

		auto active = objects[0]->getCurrActive();
		if (active != ACTIVE_INTEGER)
			throw std::runtime_error("Runtime Error: Argument must be an integer.");

		int64_t value = objects[0]->integerValue;
		return std::make_shared<HigherObject>(MathClass::factorial(objects[0]->integerValue));
	}
	else if (newfuncType == MathEnum::MATH_COMBIN)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments (Integer, Integer).");

		auto active1 = objects[0]->getCurrActive();
		auto active2 = objects[1]->getCurrActive();
		if (active1 != ACTIVE_INTEGER || active2 != ACTIVE_INTEGER)
			throw std::runtime_error("Runtime Error: Both arguments must be integers.");

		int64_t n = objects[0]->integerValue;
		int64_t k = objects[1]->integerValue;
		if (k > n || n < 0 || k < 0)
			throw std::runtime_error("Runtime Error: Invalid values for combination.");

		int64_t result = MathClass::factorial(n) / (MathClass::factorial(k) * MathClass::factorial(n - k));
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathEnum::MATH_PERM)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments (Integer, Integer).");

		auto active1 = objects[0]->getCurrActive();
		auto active2 = objects[1]->getCurrActive();
		if (active1 != ACTIVE_INTEGER || active2 != ACTIVE_INTEGER)
			throw std::runtime_error("Runtime Error: Both arguments must be integers.");

		int64_t n = objects[0]->integerValue;
		int64_t k = objects[1]->integerValue;
		if (k > n || n < 0 || k < 0)
			throw std::runtime_error("Runtime Error: Invalid values for permutation.");

		int64_t result = MathClass::factorial(n) / MathClass::factorial(n - k);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathEnum::MATH_ROUND)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");
		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double result = std::round(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathEnum::MATH_GAMMA)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument (Number).");

		auto active = objects[0]->getCurrActive();
		long double value = (active == ACTIVE_INTEGER) ? objects[0]->integerValue :
							(active == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
							(active == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double result = std::tgamma(value);
		return std::make_shared<HigherObject>(result);
	}
	else if (newfuncType == MathEnum::MATH_BETA)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments (Number, Number).");

		auto active1 = objects[0]->getCurrActive();
		auto active2 = objects[1]->getCurrActive();
		long double x = (active1 == ACTIVE_INTEGER) ? objects[0]->integerValue :
						(active1 == ACTIVE_DECIMAL) ? objects[0]->doubleValue :
						(active1 == ACTIVE_BOOLEAN) ? objects[0]->booleanValue : 0;
		long double y = (active2 == ACTIVE_INTEGER) ? objects[1]->integerValue :
						(active2 == ACTIVE_DECIMAL) ? objects[1]->doubleValue :
						(active2 == ACTIVE_BOOLEAN) ? objects[1]->booleanValue : 0;
		long double result = std::tgamma(x) * std::tgamma(y) / std::tgamma(x + y);
		return std::make_shared<HigherObject>(result);
	}
	throw std::runtime_error("Runtime Error: This function is not a member of class 'Math'.");
}

int64_t MathClass::factorial(int64_t value)
{
	if (value < 0)
		throw std::runtime_error("Runtime Error: Factorial of a negative number is undefined.");

	int64_t result = 1;
	for (int64_t i = 2; i <= value; ++i)
	{
		result *= i;
	}
	return result;
}
