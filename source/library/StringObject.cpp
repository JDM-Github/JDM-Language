#include "utils/HigherObject.hpp"
#include "library/StringObject.hpp"

JDM_DLL
std::unordered_map<std::string, StringHigherFunctions::StringFunction> StringHigherFunctions::stringFunctions = {
	{"size"    , StringFunction::string_size             },
	{"replace" , StringFunction::string_replace          },
	{"rreplace", StringFunction::string_rreplace         },
	{"concat"  , StringFunction::string_concat           },
	{"rconcat" , StringFunction::string_rconcat          },
	{"upper"   , StringFunction::string_to_upper         },
	{"lower"   , StringFunction::string_to_lower         },
	{"rupper"  , StringFunction::string_rto_upper        },
	{"rlower"  , StringFunction::string_rto_lower        },
	{"reverse" , StringFunction::string_reverse          },
	{"rreverse", StringFunction::string_rreverse         },
	{"compare" , StringFunction::string_compare          },
	{"substr"  , StringFunction::string_substr           },
	{"rsubstr" , StringFunction::string_rsubstr          },
	{"find"    , StringFunction::string_find             },
	{"count"   , StringFunction::string_count_occurrences},
	{"empty"   , StringFunction::string_is_empty         },
	{"startsw" , StringFunction::string_starts_with      },
	{"endsw"   , StringFunction::string_ends_with        },
	{"insert"  , StringFunction::string_insert           },
	{"erase"   , StringFunction::string_erase            },
	{"rinsert" , StringFunction::string_rinsert          },
	{"rerase"  , StringFunction::string_rerase           },
	{"trim"    , StringFunction::string_trim             },
	{"rtrim"   , StringFunction::string_rtrim            },
	{"tokenize", StringFunction::string_tokenize         },
	{"split"   , StringFunction::string_split            },
	{"join"    , StringFunction::string_join             },
	{"rjoin"   , StringFunction::string_rjoin            },
	{"tolist"  , StringFunction::string_to_list          },
};

JDM_DLL
const std::shared_ptr<HigherObject> StringHigherFunctions::manageFunction(
	StringHigherFunctions::StringFunction stringFuncType,
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	std::shared_ptr<HigherObject> newReturn;
	if (stringFuncType == StringHigherFunctions::StringFunction::string_size)
	{
		if (!objects.empty())
			throw std::runtime_error("Runtime Error: Expecting 0 argument");
		newReturn = std::make_shared<HigherObject>(static_cast<int64_t>(StringHigherFunctions::size(obj1)));
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_is_empty)
	{
		if (!objects.empty())
			throw std::runtime_error("Runtime Error: Expecting 0 argument");
		newReturn = std::make_shared<HigherObject>(StringHigherFunctions::empty(obj1));
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_to_list)
	{
		if (!objects.empty())
			throw std::runtime_error("Runtime Error: Expecting 0 argument");
		newReturn = std::make_shared<HigherObject>(StringHigherFunctions::to_list(obj1));
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_find)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument");
		newReturn = std::make_shared<HigherObject>(static_cast<int64_t>(StringHigherFunctions::find(obj1, objects[0])));
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_replace)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments.");
		StringHigherFunctions::replace(obj1, objects[0], objects[1]);
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_rreplace)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments.");
		newReturn = std::make_shared<HigherObject>(StringHigherFunctions::rreplace(obj1, objects[0], objects[1]));
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_concat)
	{
		if (objects.size() == 0)
			throw std::runtime_error("Runtime Error: Expecting atleast 1 argument.");
		StringHigherFunctions::concat(obj1, objects);
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_rconcat)
	{
		if (objects.size() == 0)
			throw std::runtime_error("Runtime Error: Expecting atleast 1 argument.");
		newReturn = std::make_shared<HigherObject>(StringHigherFunctions::rconcat(obj1, objects));
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_to_lower)
	{
		if (objects.size() != 0)
			throw std::runtime_error("Runtime Error: Expecting 0 argument.");
		StringHigherFunctions::to_lower(obj1);
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_rto_lower)
	{
		if (objects.size() != 0)
			throw std::runtime_error("Runtime Error: Expecting 0 argument.");
		newReturn = std::make_shared<HigherObject>(StringHigherFunctions::rto_lower(obj1));
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_to_upper)
	{
		if (objects.size() != 0)
			throw std::runtime_error("Runtime Error: Expecting 0 argument.");
		StringHigherFunctions::to_upper(obj1);
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_rto_upper)
	{
		if (objects.size() != 0)
			throw std::runtime_error("Runtime Error: Expecting 0 argument.");
		newReturn = std::make_shared<HigherObject>(StringHigherFunctions::rto_upper(obj1));
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_reverse)
	{
		if (objects.size() != 0)
			throw std::runtime_error("Runtime Error: Expecting 0 argument.");
		StringHigherFunctions::reverse(obj1);
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_rreverse)
	{
		if (objects.size() != 0)
			throw std::runtime_error("Runtime Error: Expecting 0 argument.");
		newReturn = std::make_shared<HigherObject>(StringHigherFunctions::rreverse(obj1));
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_compare)
	{
		if (objects.size() == 0)
			throw std::runtime_error("Runtime Error: Expecting atleast 1 argument.");
		newReturn = std::make_shared<HigherObject>(StringHigherFunctions::compare(obj1, objects));
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_substr)
	{
		if (objects.size() == 0)
			throw std::runtime_error("Runtime Error: Expecting atleast 1 argument.");
		if (objects.size()  > 2)
			throw std::runtime_error("Runtime Error: Expecting only 2 arguments.");

		if (objects.size() == 1)
		{
			if (objects[0]->getCurrActive() == ACTIVE_INTEGER || objects[0]->getCurrActive() == ACTIVE_DECIMAL)
			{
				objects[0]->castToInteger();
				StringHigherFunctions::substr(obj1, 0, objects[0]->integerValue);
			}
			else
				throw std::runtime_error("Runtime Error: Expecting a argument to be a NUMBER.");
		}
		else
		{
			if ((objects[0]->getCurrActive() == ACTIVE_INTEGER || objects[0]->getCurrActive() == ACTIVE_DECIMAL)
			 && (objects[1]->getCurrActive() == ACTIVE_INTEGER || objects[1]->getCurrActive() == ACTIVE_DECIMAL))
			{
				objects[0]->castToInteger();
				objects[1]->castToInteger();
				StringHigherFunctions::substr(obj1, objects[0]->integerValue, objects[1]->integerValue);
			}
			else
				throw std::runtime_error("Runtime Error: Expecting a arguments to be a NUMBER.");
		}
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_rsubstr)
	{
		if (objects.size() == 0)
			throw std::runtime_error("Runtime Error: Expecting atleast 1 argument.");
		if (objects.size()  > 2)
			throw std::runtime_error("Runtime Error: Expecting only 2 arguments.");

		if (objects.size() == 1)
		{
			if (objects[0]->getCurrActive() == ACTIVE_INTEGER || objects[0]->getCurrActive() == ACTIVE_DECIMAL)
			{
				objects[0]->castToInteger();
				newReturn = std::make_shared<HigherObject>(StringHigherFunctions::rsubstr(obj1, 0, objects[0]->integerValue));
			}
			else
				throw std::runtime_error("Runtime Error: Expecting a argument to be a NUMBER.");
		}
		else
		{
			if ((objects[0]->getCurrActive() == ACTIVE_INTEGER || objects[0]->getCurrActive() == ACTIVE_DECIMAL)
			 && (objects[1]->getCurrActive() == ACTIVE_INTEGER || objects[1]->getCurrActive() == ACTIVE_DECIMAL))
			{
				objects[0]->castToInteger();
				objects[1]->castToInteger();
				newReturn = std::make_shared<HigherObject>(StringHigherFunctions::rsubstr(obj1, objects[0]->integerValue, objects[1]->integerValue));
			}
			else
				throw std::runtime_error("Runtime Error: Expecting a arguments to be a NUMBER.");
		}
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_count_occurrences)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument.");
		newReturn = std::make_shared<HigherObject>(static_cast<int64_t>(StringHigherFunctions::count(obj1, objects[0])));
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_starts_with)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument.");
		newReturn = std::make_shared<HigherObject>(StringHigherFunctions::startsWith(obj1, objects[0]));
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_ends_with)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument.");
		newReturn = std::make_shared<HigherObject>(StringHigherFunctions::endsWith(obj1, objects[0]));
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_insert)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments.");
		if (!(objects[0]->getCurrActive() == ACTIVE_INTEGER || objects[0]->getCurrActive() == ACTIVE_DECIMAL))
			throw std::runtime_error("Runtime Error: Expecting first argument to be a NUMBER.");

		objects[0]->castToInteger();
		StringHigherFunctions::insert(obj1, objects[0]->integerValue, objects[1]);
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_rinsert)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments.");
		if (!(objects[0]->getCurrActive() == ACTIVE_INTEGER || objects[0]->getCurrActive() == ACTIVE_DECIMAL))
			throw std::runtime_error("Runtime Error: Expecting first argument to be a NUMBER.");

		objects[0]->castToInteger();
		newReturn = std::make_shared<HigherObject>(StringHigherFunctions::rinsert(obj1, objects[0]->integerValue, objects[1]));
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_erase)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments.");
		if (!(objects[0]->getCurrActive() == ACTIVE_INTEGER || objects[0]->getCurrActive() == ACTIVE_DECIMAL)
		 || !(objects[1]->getCurrActive() == ACTIVE_INTEGER || objects[1]->getCurrActive() == ACTIVE_DECIMAL))
			throw std::runtime_error("Runtime Error: Expecting arguments to be a NUMBER.");

		objects[0]->castToInteger();
		objects[1]->castToInteger();
		StringHigherFunctions::erase(obj1, objects[0]->integerValue, objects[1]->integerValue);
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_rerase)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments.");
		if (!(objects[0]->getCurrActive() == ACTIVE_INTEGER || objects[0]->getCurrActive() == ACTIVE_DECIMAL)
		 || !(objects[1]->getCurrActive() == ACTIVE_INTEGER || objects[1]->getCurrActive() == ACTIVE_DECIMAL))
			throw std::runtime_error("Runtime Error: Expecting arguments to be a NUMBER.");

		objects[0]->castToInteger();
		objects[1]->castToInteger();
		newReturn = std::make_shared<HigherObject>(StringHigherFunctions::rerase(obj1, objects[0]->integerValue, objects[1]->integerValue));
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_trim)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments.");
		if (!(objects[0]->getCurrActive() == ACTIVE_INTEGER || objects[0]->getCurrActive() == ACTIVE_DECIMAL)
		 || !(objects[1]->getCurrActive() == ACTIVE_INTEGER || objects[1]->getCurrActive() == ACTIVE_DECIMAL))
			throw std::runtime_error("Runtime Error: Expecting arguments to be a NUMBER.");

		objects[0]->castToInteger();
		objects[1]->castToInteger();
		StringHigherFunctions::trim(obj1, objects[0]->integerValue, objects[1]->integerValue);
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_rtrim)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments.");
		if (!(objects[0]->getCurrActive() == ACTIVE_INTEGER || objects[0]->getCurrActive() == ACTIVE_DECIMAL)
		 || !(objects[1]->getCurrActive() == ACTIVE_INTEGER || objects[1]->getCurrActive() == ACTIVE_DECIMAL))
			throw std::runtime_error("Runtime Error: Expecting arguments to be a NUMBER.");

		objects[0]->castToInteger();
		objects[1]->castToInteger();
		newReturn = std::make_shared<HigherObject>(StringHigherFunctions::rtrim(obj1, objects[0]->integerValue, objects[1]->integerValue));
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_tokenize)
	{
		if (objects.size() > 1)
			throw std::runtime_error("Runtime Error: Expecting 0 or 1 argument.");

		if (objects.size() == 0)
			newReturn = std::make_shared<HigherObject>(StringHigherFunctions::tokenize(obj1));
		else
		{
			if (objects[0]->getCurrActive() != ACTIVE_STRING)
				throw std::runtime_error("Runtime Error: Expecting a 'jstring' argument.");
			newReturn = std::make_shared<HigherObject>(StringHigherFunctions::tokenize(obj1, objects[0]->stringValue));
		}
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_split)
	{
		if (objects.size() > 1)
			throw std::runtime_error("Runtime Error: Expecting 0 or 1 argument.");

		if (objects.size() == 0)
			newReturn = std::make_shared<HigherObject>(StringHigherFunctions::split(obj1));
		else
		{
			objects[0]->castToString();
			newReturn = std::make_shared<HigherObject>(StringHigherFunctions::split(obj1, objects[0]->stringValue));
		}
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_join)
	{
		if (objects.empty() || objects.size() > 2)
			throw std::runtime_error("Runtime Error: Expecting 1 or 2 argument.");

		if (objects.size() == 1)
			StringHigherFunctions::join(obj1, objects[0]);
		else
		{
			objects[1]->castToString();
			StringHigherFunctions::join(obj1, objects[0], objects[1]->stringValue);
		}
	}
	else if (stringFuncType == StringHigherFunctions::StringFunction::string_rjoin)
	{
		if (objects.empty() || objects.size() > 2)
			throw std::runtime_error("Runtime Error: Expecting 1 or 2 argument.");

		if (objects.size() == 1)
			newReturn = std::make_shared<HigherObject>(StringHigherFunctions::rjoin(obj1, objects[0]));
		else
		{
			objects[1]->castToString();
			newReturn = std::make_shared<HigherObject>(StringHigherFunctions::rjoin(obj1, objects[0], objects[1]->stringValue));
		}
	}
	else
		throw std::runtime_error("Runtime Error: This function is not a member of class 'jstring'.");
	return newReturn;
}

JDM_DLL
const int StringHigherFunctions::size(std::shared_ptr<HigherObject> &obj1)
{
	return obj1->stringValue.size();
}

JDM_DLL
const bool StringHigherFunctions::empty(std::shared_ptr<HigherObject> &obj1)
{
	return obj1->stringValue.empty();
}

JDM_DLL
const std::vector<std::shared_ptr<HigherObject>> StringHigherFunctions::to_list(std::shared_ptr<HigherObject> &obj1)
{
	std::vector<std::shared_ptr<HigherObject>> result;
	for (const auto s : obj1->stringValue)
		result.push_back(std::make_shared<HigherObject>(std::string(1, s)));
	return result;
}

JDM_DLL
const int StringHigherFunctions::find(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &findStr)
{
	findStr->castToString();
	return obj1->stringValue.find(findStr->stringValue);
}

JDM_DLL
const int StringHigherFunctions::count(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &findStr)
{
	findStr->castToString();
	int result = 0;
	size_t start_pos = 0;
	while ((start_pos = obj1->stringValue.find(findStr->stringValue, start_pos)) != std::string::npos)
	{
		result++; start_pos += findStr->stringValue.length();
	}
	return result;
}

JDM_DLL
const std::vector<std::shared_ptr<HigherObject>> StringHigherFunctions::tokenize(
	std::shared_ptr<HigherObject> &obj1, const std::string &regexPattern)
{
	std::vector<std::shared_ptr<HigherObject>> result;
	std::regex pattern(regexPattern);
   	std::sregex_token_iterator iter(obj1->stringValue.begin(), obj1->stringValue.end(), pattern, -1);
   	std::sregex_token_iterator end;
   	while (iter != end)
   	{
        result.push_back(std::make_shared<HigherObject>(*iter));
   	    ++iter;
   	}
   	return result;
}

JDM_DLL
const std::vector<std::shared_ptr<HigherObject>> StringHigherFunctions::split(
	const std::shared_ptr<HigherObject> &obj1, const std::string &stringObj)
{
	std::string temp = "";
	std::vector<std::shared_ptr<HigherObject>> result;
	for (int i = 0; i < obj1->stringValue.size(); i++)
	{
		temp += obj1->stringValue[i];
		bool isEnd = i + 1 >= obj1->stringValue.size();
   		if (isEnd || (temp.size() >= stringObj.size() && temp.substr(temp.size() - stringObj.size()) == stringObj))
   		{
   			if (temp.size() != stringObj.size())
       			result.push_back(std::make_shared<HigherObject>(
       				isEnd ? temp : temp.substr(0, temp.size() - stringObj.size())));
       		temp.clear();
   		}
	}
	return result;
}

JDM_DLL
const void StringHigherFunctions::join(
	const std::shared_ptr<HigherObject> &obj1,
	const std::shared_ptr<HigherObject> &listObj,
	const std::string &stringObj)
{
	if (obj1->isConstant)
		throw std::runtime_error("Runtime Error: Variable is Constant.");

	listObj->castToList();
	if (listObj->listValue.empty())
		throw std::runtime_error("Runtime Error: Using 'join' on empty ITERABLE.");

	for (size_t i = 0; i < listObj->listValue.size(); i++)
	{
		auto newValue = std::make_shared<HigherObject>(listObj->listValue[i]); newValue->castToString();
		obj1->stringValue += (newValue->stringValue + ((listObj->listValue.size()-1 <= i) ? "" : stringObj));
	}
}

JDM_DLL
const std::string StringHigherFunctions::rjoin(
	const std::shared_ptr<HigherObject> &obj1,
	const std::shared_ptr<HigherObject> &listObj,
	const std::string &stringObj)
{
	listObj->castToList();
	if (listObj->listValue.empty())
		throw std::runtime_error("Runtime Error: Using 'join' on empty ITERABLE.");

	std::string result = obj1->stringValue;
	for (size_t i = 0; i < listObj->listValue.size(); i++)
	{
		auto newValue = std::make_shared<HigherObject>(listObj->listValue[i]); newValue->castToString();
		result += (newValue->stringValue + ((listObj->listValue.size()-1 <= i) ? "" : stringObj));
	}
	return result;
}

JDM_DLL
const bool StringHigherFunctions::startsWith(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &prefix)
{
	prefix->castToString();
	return obj1->stringValue.compare(0, prefix->stringValue.length(), prefix->stringValue) == 0;
}

JDM_DLL
const bool StringHigherFunctions::endsWith(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &suffix)
{
	suffix->castToString();
	if (obj1->stringValue.length() >= suffix->stringValue.length())
		return obj1->stringValue.compare(obj1->stringValue.length() - suffix->stringValue.length(),
			suffix->stringValue.length(), suffix->stringValue) == 0;
	return false;
}

JDM_DLL
const void StringHigherFunctions::insert(std::shared_ptr<HigherObject> &obj1,
	int start, const std::shared_ptr<HigherObject> &insertStr)
{
	if (obj1->isConstant)
		throw std::runtime_error("Runtime Error: Variable is Constant.");

	insertStr->castToString();
	obj1->stringValue.insert(start, insertStr->stringValue);
}

JDM_DLL
const void StringHigherFunctions::erase(std::shared_ptr<HigherObject> &obj1, int start, int end)
{
	if (obj1->isConstant)
		throw std::runtime_error("Runtime Error: Variable is Constant.");
	obj1->stringValue.erase(start, end);
}

JDM_DLL
const std::string StringHigherFunctions::rinsert(std::shared_ptr<HigherObject> &obj1,
	int start, const std::shared_ptr<HigherObject> &insertStr)
{
	insertStr->castToString();
	std::string result = obj1->stringValue;
	result.insert(start, insertStr->stringValue);
	return result;
}

JDM_DLL
const std::string StringHigherFunctions::rerase(std::shared_ptr<HigherObject> &obj1, int start, int end)
{
	std::string result = obj1->stringValue;
	result.erase(start, end);
	return result;
}

JDM_DLL
const void StringHigherFunctions::trim(std::shared_ptr<HigherObject> &obj1, int start, int end)
{
	if (obj1->isConstant)
		throw std::runtime_error("Runtime Error: Variable is Constant.");

	obj1->stringValue.erase(0, start);
	obj1->stringValue.erase(obj1->stringValue.size()-end, end);
}

JDM_DLL
const std::string StringHigherFunctions::rtrim(std::shared_ptr<HigherObject> &obj1, int start, int end)
{
	std::string result = obj1->stringValue;
	result.erase(0, start);
	result.erase(result.size()-end, end);
	return result;
}

JDM_DLL
const void StringHigherFunctions::replace(
	std::shared_ptr<HigherObject> &obj1,
	const std::shared_ptr<HigherObject> &from,
	const std::shared_ptr<HigherObject> &to)
{
	if (obj1->isConstant)
		throw std::runtime_error("Runtime Error: Variable is Constant.");

	from->castToString();
	to->castToString();
	std::string result = obj1->stringValue;
	size_t start_pos = 0;
	while ((start_pos = result.find(from->stringValue, start_pos)) != std::string::npos)
	{
		result.replace(start_pos, from->stringValue.length(), to->stringValue);
		start_pos += to->stringValue.length();
	}
	obj1->stringValue = result;
}

JDM_DLL
const std::string StringHigherFunctions::rreplace(
	std::shared_ptr<HigherObject> &obj1,
	const std::shared_ptr<HigherObject> &from,
	const std::shared_ptr<HigherObject> &to)
{
	from->castToString();
	to->castToString();
	std::string result = obj1->stringValue;
	size_t start_pos = 0;
	while ((start_pos = result.find(from->stringValue, start_pos)) != std::string::npos)
	{
		result.replace(start_pos, from->stringValue.length(), to->stringValue);
		start_pos += to->stringValue.length();
	}
	return result;
}

JDM_DLL
const void StringHigherFunctions::concat(
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (obj1->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
	for (const auto e : objects)
	{
		e->castToString();
		obj1->stringValue += e->stringValue;
	}
}

JDM_DLL
const std::string StringHigherFunctions::rconcat(
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	std::string result = obj1->stringValue;
	for (const auto e : objects)
	{
		e->castToString();
		result += e->stringValue;
	}
	return result;
}

JDM_DLL
const void StringHigherFunctions::to_lower(
	std::shared_ptr<HigherObject> &obj1)
{
	if (obj1->isConstant)
		throw std::runtime_error("Runtime Error: Variable is Constant.");
	std::transform(obj1->stringValue.begin(), obj1->stringValue.end(), obj1->stringValue.begin(), tolower);
}

JDM_DLL
const std::string StringHigherFunctions::rto_lower(
	std::shared_ptr<HigherObject> &obj1)
{
	std::string result = obj1->stringValue;
	std::transform(result.begin(), result.end(), result.begin(), tolower);
	return result;
}

JDM_DLL
const void StringHigherFunctions::to_upper(
	std::shared_ptr<HigherObject> &obj1)
{
	if (obj1->isConstant)
		throw std::runtime_error("Runtime Error: Variable is Constant.");
	std::transform(obj1->stringValue.begin(), obj1->stringValue.end(), obj1->stringValue.begin(), toupper);
}

JDM_DLL
const std::string StringHigherFunctions::rto_upper(
	std::shared_ptr<HigherObject> &obj1)
{
	std::string result = obj1->stringValue;
	std::transform(result.begin(), result.end(), result.begin(), toupper);
	return result;
}

JDM_DLL
const bool StringHigherFunctions::compare(
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	for (const auto e : objects)
	{
		e->castToString();
		if (!obj1->compareHigherObject(e))
			return false;
	}
	return true;
}

JDM_DLL
const void StringHigherFunctions::reverse(
	std::shared_ptr<HigherObject> &obj1)
{
	if (obj1->isConstant)
		throw std::runtime_error("Runtime Error: Variable is Constant.");
	std::reverse(obj1->stringValue.begin(), obj1->stringValue.end());
}

JDM_DLL
const std::string StringHigherFunctions::rreverse( std::shared_ptr<HigherObject> &obj1 )
{
	std::string result = obj1->stringValue;
	std::reverse(result.begin(), result.end());
	return result;
}

JDM_DLL
const void StringHigherFunctions::substr( std::shared_ptr<HigherObject> &obj1, int start, int end )
{
	if (obj1->isConstant)
		throw std::runtime_error("Runtime Error: Variable is Constant.");

	obj1->stringValue = obj1->stringValue.substr(start, end);
}

JDM_DLL
const std::string StringHigherFunctions::rsubstr( std::shared_ptr<HigherObject> &obj1, int start, int end )
{
	std::string result = obj1->stringValue.substr(start, end);
	return result;
}
