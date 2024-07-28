#include "utils/HigherObject.hpp"
#include "library/ListObject.hpp"

JDM_DLL
std::unordered_map<std::string, ListHigherFunctions::ListFunction> ListHigherFunctions::listFunctions = {
	{"size"      , ListFunction::list_size      },
	{"sort"      , ListFunction::list_sort      },
    {"count"     , ListFunction::list_count     },
    {"search"    , ListFunction::list_search    },
    {"insert"    , ListFunction::list_insert    },
    {"delete"    , ListFunction::list_delete    },
    {"at"        , ListFunction::list_at        },
    {"reverse"   , ListFunction::list_reverse   },
    {"push_back" , ListFunction::list_push_back },
    {"push_front", ListFunction::list_push_front},
    {"pop_back"  , ListFunction::list_pop_back  },
    {"pop_front" , ListFunction::list_pop_front }
};

JDM_DLL
const std::shared_ptr<HigherObject> ListHigherFunctions::manageFunction(
	ListHigherFunctions::ListFunction listFuncType,
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (listFuncType == ListHigherFunctions::ListFunction::list_sort)
	{
		if (!objects.empty())
			throw std::runtime_error("Runtime Error: Expecting 0 arguments.");
		ListHigherFunctions::sort(obj1);
	}
	else if (listFuncType == ListHigherFunctions::ListFunction::list_reverse)
	{
		if (!objects.empty())
			throw std::runtime_error("Runtime Error: Expecting 0 arguments.");
		ListHigherFunctions::reverse(obj1);
	}
	else if (listFuncType == ListHigherFunctions::ListFunction::list_size)
	{
		if (!objects.empty())
			throw std::runtime_error("Runtime Error: Expecting 0 arguments.");

		return std::make_shared<HigherObject>(
			static_cast<int64_t>(ListHigherFunctions::size(obj1)));
	}
	else if (listFuncType == ListHigherFunctions::ListFunction::list_count)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments.");

		return std::make_shared<HigherObject>(
			static_cast<int64_t>(ListHigherFunctions::count(obj1, objects[0])));
	}
	else if (listFuncType == ListHigherFunctions::ListFunction::list_search)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments.");

		return std::make_shared<HigherObject>(
			static_cast<int64_t>(ListHigherFunctions::search(obj1, objects[0] )));
	}
	else if (listFuncType == ListHigherFunctions::ListFunction::list_insert)
	{
		if (objects.size() < 1 || objects.size() > 2)
			throw std::runtime_error("Runtime Error: Expecting 1 or 2 arguments.");

		int index = -1;
		if (objects.size() == 2)
		{
			objects[1]->castToInteger();
			index = objects[1]->integerValue;
		}
		ListHigherFunctions::insert(obj1, objects[0], index);
	}
	else if (listFuncType == ListHigherFunctions::ListFunction::list_delete)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 arguments.");

		objects[0]->castToInteger();
		ListHigherFunctions::del(obj1, objects[0]->integerValue);
	}
	else if (listFuncType == ListHigherFunctions::ListFunction::list_at)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 arguments.");
		objects[0]->castToInteger();
		return ListHigherFunctions::at(obj1, objects[0]->integerValue);
	}
	else if (listFuncType == ListHigherFunctions::ListFunction::list_push_back)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 arguments.");
		ListHigherFunctions::push_back(obj1, objects[0]);
	}
	else if (listFuncType == ListHigherFunctions::ListFunction::list_push_front)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 arguments.");
		ListHigherFunctions::push_front(obj1, objects[0]);
	}
	else if (listFuncType == ListHigherFunctions::ListFunction::list_pop_back)
	{
		if (!objects.empty())
			throw std::runtime_error("Runtime Error: Expecting 0 arguments.");
		ListHigherFunctions::pop_back(obj1);
	}
	else if (listFuncType == ListHigherFunctions::ListFunction::list_pop_front)
	{
		if (!objects.empty())
			throw std::runtime_error("Runtime Error: Expecting 0 arguments.");
		ListHigherFunctions::pop_front(obj1);
	}
	else
	{
		throw std::runtime_error("Runtime Error: This function is not a member of class 'jlist'.");
	}
	return nullptr;
}

JDM_DLL
const void ListHigherFunctions::sort(std::shared_ptr<HigherObject> &obj1)
{
	if (obj1->isConstant)
		throw std::runtime_error("Runtime Error: Variable is Constant.");
	obj1->sortList();
}

JDM_DLL
const int ListHigherFunctions::size(std::shared_ptr<HigherObject> &obj1)
{
	return obj1->listValue.size();
}

JDM_DLL
const int ListHigherFunctions::count(
	std::shared_ptr<HigherObject> &obj1,
	const std::shared_ptr<HigherObject> &obj2)
{
	int countOccurrence = 0;
	for (const auto &li : obj1->listValue)
		if (li->compareHigherObject(obj2))
			countOccurrence++;
	return countOccurrence;
}

JDM_DLL
const int ListHigherFunctions::search(
	std::shared_ptr<HigherObject> &obj1,
	const std::shared_ptr<HigherObject> &obj2)
{
	int index = 0;
	for (const auto &li : obj1->listValue)
		if (li->compareHigherObject(obj2))
			return index;
		else
			index++;
	return -1;
}

JDM_DLL
const void ListHigherFunctions::reverse(std::shared_ptr<HigherObject> &obj1)
{
	if (obj1->isConstant)
		throw std::runtime_error("Runtime Error: Variable is Constant.");

	std::vector<std::shared_ptr<HigherObject>> result;
	for (int i = obj1->listValue.size() - 1; i >= 0; i--)
		result.push_back(obj1->listValue[i]);

	obj1->listValue.clear();
	obj1->listValue = result;
}

JDM_DLL
const void ListHigherFunctions::insert(
	std::shared_ptr<HigherObject> &obj1,
	const std::shared_ptr<HigherObject> &obj2,
	int index)
{
	if (obj1->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
	if (index < 0)   index += obj1->listValue.size();
	if (index < 0 || index >  obj1->listValue.size())
   		throw std::runtime_error("Runtime Error: Index out of bounds.");

	obj1->listValue.insert(obj1->listValue.begin()+index, obj2);
}

JDM_DLL
const void ListHigherFunctions::del(
	std::shared_ptr<HigherObject> &obj1,
	int index)
{
	if (obj1->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
	if (index < 0)   index += obj1->listValue.size();
	if (index < 0 || index >= obj1->listValue.size())
   		throw std::runtime_error("Runtime Error: Index out of bounds.");

	obj1->listValue.erase(obj1->listValue.begin()+index);
}

JDM_DLL
const std::shared_ptr<HigherObject> ListHigherFunctions::at(
	std::shared_ptr<HigherObject> &obj1,
	int index)
{
	if (index < 0)   index += obj1->listValue.size();
	if (index < 0 || index >= obj1->listValue.size())
   		throw std::runtime_error("Runtime Error: Index out of bounds.");

   	return obj1->listValue[index];
}

JDM_DLL
const void ListHigherFunctions::push_back(
	std::shared_ptr<HigherObject> &obj1,
	const std::shared_ptr<HigherObject> &obj2)
{
	if (obj1->isConstant)
		throw std::runtime_error("Runtime Error: Variable is Constant.");
	obj1->listValue.push_back(obj2);
}

JDM_DLL
const void ListHigherFunctions::push_front(
	std::shared_ptr<HigherObject> &obj1,
	const std::shared_ptr<HigherObject> &obj2)
{
	if (obj1->isConstant)
		throw std::runtime_error("Runtime Error: Variable is Constant.");
	obj1->listValue.insert(obj1->listValue.begin(), obj2);
}

JDM_DLL
const void ListHigherFunctions::pop_back(std::shared_ptr<HigherObject> &obj1)
{
	if (obj1->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
	if (obj1->listValue.empty()) return;
	obj1->listValue.pop_back();
}

JDM_DLL
const void ListHigherFunctions::pop_front(std::shared_ptr<HigherObject> &obj1)
{
	if (obj1->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
	if (obj1->listValue.empty()) return;
	obj1->listValue.erase(obj1->listValue.begin());
}

JDM_DLL
const void ListHigherFunctions::rdup(std::shared_ptr<HigherObject> &obj1)
{
	if (obj1->isConstant)
		throw std::runtime_error("Runtime Error: Variable is Constant.");
}
