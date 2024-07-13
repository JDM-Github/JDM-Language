#include "utils/HigherObject.hpp"
#include "library/NativeObject.hpp"

JDM_DLL
std::unordered_map<std::string, NativeFunction::NativeFunctionEnum> NativeFunction::allNativeFunction = {
	{"split"    , NATIVE_SPLIT       },
	{"input"    , NATIVE_INPUT       },
	{"len"      , NATIVE_LENGTH      },
	{"abs"      , NATIVE_ABS         },
	{"ceil"     , NATIVE_CEIL        },
	{"floor"    , NATIVE_FLOOR       },
	{"max"      , NATIVE_MAX         },
	{"min"      , NATIVE_MIN         },
	{"reverse"  , NATIVE_REVERSE     },
	{"randint"  , NATIVE_RANDOM_INT  },
	{"randfloat", NATIVE_RANDOM_FLOAT},
	{"sort"     , NATIVE_SORT        },
	{"any"      , NATIVE_ANY         },
	{"all"      , NATIVE_ALL         },
	{"round"    , NATIVE_ROUND       },
	{"sum"      , NATIVE_SUM         },
	{"join"     , NATIVE_JOIN        },
	{"index"    , NATIVE_INDEX       },
	{"filter"   , NATIVE_FILTER      },
	{"unique"   , NATIVE_UNIQUE      },
	{"reduce"   , NATIVE_REDUCE      },
	{"map"      , NATIVE_MAP         },
	{"chain"    , NATIVE_CHAIN       },
	{"partial"  , NATIVE_PARTIAL     },
	{"sort_if"  , NATIVE_SORT_IF     },
	{"gettype"  , NATIVE_GETTYPE     },
	{"cast"     , NATIVE_CAST        },
	{"ref"      , NATIVE_REFERENCE   },
};

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::manageFunction(
	NativeFunction::NativeFunctionEnum nativeFuncType,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_SPLIT)
	{
		if (objects.size() < 1)
			throw std::runtime_error("Runtime Error: Expecting atleast 1 argument. Target (jstring).");
		if (objects.size() > 2)
			throw std::runtime_error("Runtime Error: Too many arguments. Target (jstring) and sepStr (jstring).");
		if (objects.size() == 2)
		{
			objects[1]->castToString();
			return NativeFunction::split(objects[0], objects[1]->stringValue);
		}
		else
			return NativeFunction::split(objects[0]);
	}
	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_INPUT)
		return NativeFunction::input(objects);

	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_LENGTH)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument. Target (jany).");
		return NativeFunction::len(objects[0]);
	}
	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_ABS)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument. Target (NUMBER).");
		return NativeFunction::absolute(objects[0]);
	}
	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_CEIL)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument. Target (jdouble).");
		return NativeFunction::ceiling(objects[0]);
	}
	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_FLOOR)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument. Target (jdouble).");
		return NativeFunction::floored(objects[0]);
	}
	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_MAX)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument. Target (jlist).");
		return NativeFunction::maxed(objects[0]);
	}
	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_MIN)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument. Target (jlist).");
		return NativeFunction::minimum(objects[0]);
	}
	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_REVERSE)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument. Target (jlist).");
		return NativeFunction::reverse(objects[0]);
	}
	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_RANDOM_INT)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments. Start (jint) and End (jint).");
		objects[0]->castToInteger();
		objects[1]->castToInteger();
		int start = objects[0]->integerValue;
		int end   = objects[1]->integerValue;
		return NativeFunction::randint(start, end);
	}
	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_RANDOM_FLOAT)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments. Start (jdouble) and End (jdouble).");
		objects[0]->castToDecimal();
		objects[1]->castToDecimal();
		long double start = objects[0]->doubleValue;
		long double end   = objects[1]->doubleValue;
		return NativeFunction::randfloat(start, end);
	}
	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_SORT)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument. Target (jlist).");
		return NativeFunction::sort(objects[0]);
	}
	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_ANY)
	{
		return NativeFunction::anyOperator(objects);
	}
	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_ALL)
	{
		return NativeFunction::allOperator(objects);
	}
	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_ROUND)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument. Target (jdouble).");
		return NativeFunction::roundOperator(objects[0]);
	}
	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_INDEX)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 argument. Target (ITERABLE) and Key (jany).");
		return NativeFunction::indexOperator(objects[0], objects[1]);
	}
	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_SUM)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument. Target (jlist).");
   		return NativeFunction::sumOperator(objects[0]);
   	}
   	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_JOIN)
   	{
		if (objects.size() < 1)
			throw std::runtime_error("Runtime Error: Expecting atleast 1 argument. Target (jlist).");
		if (objects.size() > 2)
			throw std::runtime_error("Runtime Error: Too many arguments. Target (jlist) and AddStr (jstring).");
		if (objects.size() == 2)
		{
			objects[1]->castToString();
			return NativeFunction::joinOperator(objects[0], objects[1]->stringValue);
		}
		return NativeFunction::joinOperator(objects[0]);
	}
	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_GETTYPE)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument.");

   		return std::make_shared<HigherObject>(objects[0]->getType());
	}
	else if (nativeFuncType == NativeFunction::NativeFunctionEnum::NATIVE_CHAIN)
		return NativeFunction::chain(objects);
	return nullptr;
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::split(
	const std::shared_ptr<HigherObject> &obj1,
	const std::string &stringObj)
{
	if (obj1->getCurrActive() != ACTIVE_STRING)
		return obj1;

	std::string temp = "";
	std::vector<std::shared_ptr<HigherObject>> objects;
	for (int i = 0; i < obj1->stringValue.size(); i++)
	{
		temp += obj1->stringValue[i];
		bool isEnd = i + 1 >= obj1->stringValue.size();
		if (isEnd || (temp.size() >= stringObj.size() && temp.substr(temp.size() - stringObj.size()) == stringObj))
		{
			if (temp.size() != stringObj.size())
				objects.push_back(std::make_shared<HigherObject>(isEnd ? temp
					: temp.substr(0, temp.size() - stringObj.size())));
			temp.clear();
		}
	}
	return std::make_shared<HigherObject>(objects);
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::input(
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	for (const auto &param : objects)
	{
		if (param == nullptr)
			throw std::runtime_error("Runtime Error: Invalid Expression.");

		param->logValue();
	}

	std::string input;
	std::getline(std::cin, input);
	return std::make_shared<HigherObject>(input);
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::len(
	const std::shared_ptr<HigherObject> &obj1)
{
	if (obj1->getCurrActive() == ACTIVE_STRING)
		return std::make_shared<HigherObject>( static_cast<int64_t>( obj1->stringValue.size()) );
	if (obj1->getCurrActive() == ACTIVE_LIST  )
		return std::make_shared<HigherObject>( static_cast<int64_t>( obj1->listValue  .size()) );
	if (obj1->getCurrActive() == ACTIVE_MAP   )
		return std::make_shared<HigherObject>( static_cast<int64_t>( obj1->mapValue   .size()) );

	return std::make_shared<HigherObject>( static_cast<int64_t> (1) );
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::absolute(
	const std::shared_ptr<HigherObject> &obj1)
{
	if (obj1->getCurrActive() == ACTIVE_INTEGER)
		return std::make_shared<HigherObject>( static_cast<int64_t>( abs(obj1->integerValue) ));
	if (obj1->getCurrActive() == ACTIVE_DECIMAL)
		return std::make_shared<HigherObject>( static_cast<long double>( abs(obj1->doubleValue) ));

	return obj1;
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::ceiling(
	const std::shared_ptr<HigherObject> &obj1)
{
	if (obj1->getCurrActive() == ACTIVE_DECIMAL)
		return std::make_shared<HigherObject>( static_cast<long double>( ceil(obj1->doubleValue) ));

	return obj1;
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::floored(
	const std::shared_ptr<HigherObject> &obj1)
{
	if (obj1->getCurrActive() == ACTIVE_DECIMAL)
		return std::make_shared<HigherObject>( static_cast<long double>( static_cast<int>(obj1->doubleValue) ));

	return obj1;
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::minimum(
	const std::shared_ptr<HigherObject> &obj1)
{
	if (obj1->getCurrActive() != ACTIVE_LIST && obj1->getCurrActive() != ACTIVE_MAP)
		return obj1;

	std::vector<std::shared_ptr<HigherObject>> stringObject;
	std::vector<std::shared_ptr<HigherObject>> DecimalObject;
	std::vector<std::shared_ptr<HigherObject>> ListObject;
	std::vector<std::shared_ptr<HigherObject>> MapObject;
	std::vector<std::shared_ptr<HigherObject>> otherObject;

	auto newList = std::make_shared<HigherObject>(obj1); newList->castToList();
	if (newList->listValue.empty())
		throw std::runtime_error("Runtime Error: Using 'min' on empty ITERABLE.");

	if (newList->getCurrActive() == ACTIVE_LIST)
	{
		for (const auto &obj : newList->listValue)
		{
			if      (obj->getCurrActive() == ACTIVE_STRING)
			{
				stringObject .push_back(obj);
			}
			else if (obj->getCurrActive() == ACTIVE_INTEGER
				  || obj->getCurrActive() == ACTIVE_DECIMAL
				  || obj->getCurrActive() == ACTIVE_BOOLEAN)
			{
				auto newValue = std::make_shared<HigherObject>(obj);
				newValue->castToDecimal();
				DecimalObject.push_back(newValue);
			}
			else if (obj->getCurrActive() == ACTIVE_LIST)
				ListObject   .push_back(obj);
			else if (obj->getCurrActive() == ACTIVE_MAP )
				MapObject    .push_back(obj);
			else otherObject.push_back(obj);
		}
	}
	if (!DecimalObject.empty())
	{
		std::sort(DecimalObject.begin(), DecimalObject.end(),
		[](const std::shared_ptr<HigherObject> &a, const std::shared_ptr<HigherObject> &b)
		{
			return a->doubleValue < b->doubleValue;
		});
		return DecimalObject[0];
	}
	if (!stringObject.empty())
	{
		std::sort(stringObject.begin(), stringObject.end(),
		[](const std::shared_ptr<HigherObject> &a, const std::shared_ptr<HigherObject> &b)
		{
			return a->stringValue < b->stringValue;
		});
		return stringObject[0];
	}
	if (!ListObject.empty())
	{
		std::sort(ListObject.begin(), ListObject.end(),
		[](const std::shared_ptr<HigherObject> &a, const std::shared_ptr<HigherObject> &b)
		{
			return a->listValue.size() < b->listValue.size();
		});
		return ListObject[0];
	}
	if (!MapObject.empty())
	{
		std::sort(MapObject.begin(), MapObject.end(),
		[](const std::shared_ptr<HigherObject> &a, const std::shared_ptr<HigherObject> &b)
		{
			return a->mapValue.size() < b->mapValue.size();
		});
		return MapObject[0];
	}
	return otherObject[0];
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::maxed(
	const std::shared_ptr<HigherObject> &obj1)
{
	if (obj1->getCurrActive() != ACTIVE_LIST && obj1->getCurrActive() != ACTIVE_MAP)
		return obj1;

	std::vector<std::shared_ptr<HigherObject>> stringObject;
	std::vector<std::shared_ptr<HigherObject>> DecimalObject;
	std::vector<std::shared_ptr<HigherObject>> ListObject;
	std::vector<std::shared_ptr<HigherObject>> MapObject;
	std::vector<std::shared_ptr<HigherObject>> otherObject;

	auto newList = std::make_shared<HigherObject>(obj1); newList->castToList();
	if (newList->listValue.empty())
		throw std::runtime_error("Runtime Error: Using 'max' on empty ITERABLE.");

	if (newList->getCurrActive() == ACTIVE_LIST)
	{
		for (const auto &obj : newList->listValue)
		{
			if      (obj->getCurrActive() == ACTIVE_STRING)
			{
				stringObject .push_back(obj);
			}
			else if (obj->getCurrActive() == ACTIVE_INTEGER
				  || obj->getCurrActive() == ACTIVE_DECIMAL
				  || obj->getCurrActive() == ACTIVE_BOOLEAN)
			{
				auto newValue = std::make_shared<HigherObject>(obj);
				newValue->castToDecimal();
				DecimalObject.push_back(newValue);
			}
			else if (obj->getCurrActive() == ACTIVE_LIST)
				ListObject   .push_back(obj);
			else if (obj->getCurrActive() == ACTIVE_MAP )
				MapObject    .push_back(obj);
			else otherObject.push_back(obj);
		}
	}
	if (!MapObject.empty())
	{
		std::sort(MapObject.begin(), MapObject.end(),
		[](const std::shared_ptr<HigherObject> &a, const std::shared_ptr<HigherObject> &b)
		{
			return a->mapValue.size() < b->mapValue.size();
		});
		return MapObject[MapObject.size()-1];
	}
	if (!ListObject.empty())
	{
		std::sort(ListObject.begin(), ListObject.end(),
		[](const std::shared_ptr<HigherObject> &a, const std::shared_ptr<HigherObject> &b)
		{
			return a->listValue.size() < b->listValue.size();
		});
		return ListObject[ListObject.size()-1];
	}
	if (!stringObject.empty())
	{
		std::sort(stringObject.begin(), stringObject.end(),
		[](const std::shared_ptr<HigherObject> &a, const std::shared_ptr<HigherObject> &b)
		{
			return a->stringValue < b->stringValue;
		});
		return stringObject[stringObject.size()-1];
	}
	if (!DecimalObject.empty())
	{
		std::sort(DecimalObject.begin(), DecimalObject.end(),
		[](const std::shared_ptr<HigherObject> &a, const std::shared_ptr<HigherObject> &b)
		{
			return a->doubleValue < b->doubleValue;
		});
		return DecimalObject[DecimalObject.size()-1];
	}
	return otherObject[0];
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::reverse(
	const std::shared_ptr<HigherObject> &obj1)
{
	auto newList = std::make_shared<HigherObject>(obj1);
	newList->castToList();

	std::vector<std::shared_ptr<HigherObject>> result;
	for (int i = obj1->listValue.size() - 1; i >= 0; i--)
		result.push_back(obj1->listValue[i]);

	return std::make_shared<HigherObject>(result);
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::randint(
	int start,
	int end)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(start, end);
	int64_t result = dist(gen);
	return std::make_shared<HigherObject>(result);
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::randfloat(
	long double start,
	long double end)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<long double> dist(start, end);
	long double result = dist(gen);
	return std::make_shared<HigherObject>(result);
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::sort(
	const std::shared_ptr<HigherObject> &obj1)
{
	auto newList = std::make_shared<HigherObject>(obj1); newList->castToList();
	newList->sortList();
	return newList;
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::anyOperator(
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	for (const auto &obj : objects)
	{
		obj->castToBoolean();
		if (obj->booleanValue)
			return std::make_shared<HigherObject>(true);
	}
	return std::make_shared<HigherObject>(false);
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::allOperator(
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	for (const auto &obj : objects)
	{
		obj->castToBoolean();
		if (!obj->booleanValue)
			return std::make_shared<HigherObject>(false);
	}
	return std::make_shared<HigherObject>(true);
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::chain(
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	std::vector<std::shared_ptr<HigherObject>> newList;
	for (const auto &obj : objects)
	{
		obj->castToList();
		newList.insert(
			newList.end(),
			obj->listValue.begin(),
			obj->listValue.end()
		);
	}
	return std::make_shared<HigherObject>(newList);
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::roundOperator(
	const std::shared_ptr<HigherObject> &obj1)
{
	if (obj1->getCurrActive() == ACTIVE_DECIMAL)
		return std::make_shared<HigherObject>( static_cast<int64_t>(round(obj1->doubleValue)) );

	return obj1;
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::sumOperator(
	const std::shared_ptr<HigherObject> &obj1)
{
	auto newList = std::make_shared<HigherObject>(obj1); newList->castToList();
	if (newList->listValue.empty())
		throw std::runtime_error("Runtime Error: Using 'sum' on empty ITERABLE.");

	long double result = 0;
	for (const auto &obj : newList->listValue)
	{
		auto newValue = std::make_shared<HigherObject>(obj);
		newValue->castToDecimal();
		result += newValue->doubleValue;
	}
	return std::make_shared<HigherObject>(result);
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::joinOperator(
	const std::shared_ptr<HigherObject> &obj1, const std::string &stringObj)
{
	auto newList = std::make_shared<HigherObject>(obj1); newList->castToList();
	if (newList->listValue.empty())
		throw std::runtime_error("Runtime Error: Using 'join' on empty ITERABLE.");

	std::string result;
	for (const auto &obj : newList->listValue)
	{
		auto newValue = std::make_shared<HigherObject>(obj); newValue->castToString();
		result += (newValue->stringValue + stringObj);
	}
	return std::make_shared<HigherObject>(result);
}

JDM_DLL
const std::shared_ptr<HigherObject> NativeFunction::indexOperator(
	const std::shared_ptr<HigherObject> &obj1,
	const std::shared_ptr<HigherObject> &obj2)
{
	if (obj1->getCurrActive() != ACTIVE_LIST && obj1->getCurrActive() != ACTIVE_MAP)
		return std::make_shared<HigherObject>( static_cast<int64_t>(-1) );

	auto newList = std::make_shared<HigherObject>(obj1); newList->castToList();
	if (newList->listValue.empty())
		throw std::runtime_error("Runtime Error: Using 'index' on empty ITERABLE.");

	int64_t index = 0;
	for (const auto &obj : newList->listValue)
	{
		if (obj->compareHigherObject(obj2)) return std::make_shared<HigherObject>( index );
		index++;
	}
	return std::make_shared<HigherObject>( static_cast<int64_t>(-1) );
}
