#include "higherObject.hpp"
#include "library/mapObject.hpp"

JDM_DLL
std::unordered_map<std::string, MapHigherFunctions::MapFunction> MapHigherFunctions::mapFunctions = {
	{"size"       , MapFunction::map_size       },
    {"atKey"      , MapFunction::map_atKey      },
    {"atValue"    , MapFunction::map_atValue    },
    {"searchKey"  , MapFunction::map_SearchKey  },
    {"searchValue", MapFunction::map_SearchValue},
    {"keys"       , MapFunction::map_keys       },
    {"values"     , MapFunction::map_values     },
};

JDM_DLL
const std::shared_ptr<HigherObject> MapHigherFunctions::manageFunction(
	MapHigherFunctions::MapFunction mapFuncType,
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (mapFuncType == MapHigherFunctions::MapFunction::map_size)
	{
		if (!objects.empty())
			throw std::runtime_error("Runtime Error: Expecting 0 arguments.");
		return std::make_shared<HigherObject>(static_cast<int64_t>(MapHigherFunctions::size(obj1)));
	}
	else if (mapFuncType == MapHigherFunctions::MapFunction::map_atKey)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 arguments.");
		return MapHigherFunctions::atKey(obj1, objects[0]);
	}
	else if (mapFuncType == MapHigherFunctions::MapFunction::map_atValue)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 arguments.");
		return MapHigherFunctions::atValue(obj1, objects[0]);
	}
	else if (mapFuncType == MapHigherFunctions::MapFunction::map_SearchKey)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 arguments.");
		return std::make_shared<HigherObject>(static_cast<int64_t>(MapHigherFunctions::searchKey(obj1, objects[0])));
	}
	else if (mapFuncType == MapHigherFunctions::MapFunction::map_SearchValue)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 arguments.");
		return std::make_shared<HigherObject>(static_cast<int64_t>(MapHigherFunctions::searchValue(obj1, objects[0])));
	}
	else if (mapFuncType == MapHigherFunctions::MapFunction::map_keys)
	{
		if (!objects.empty())
			throw std::runtime_error("Runtime Error: Expecting 0 arguments.");
		return MapHigherFunctions::keys(obj1);
	}
	else if (mapFuncType == MapHigherFunctions::MapFunction::map_values)
	{
		if (!objects.empty())
			throw std::runtime_error("Runtime Error: Expecting 0 arguments.");
		return MapHigherFunctions::values(obj1);
	}
	throw std::runtime_error("Runtime Error: This function is not a member of class 'jmap'.");
}


JDM_DLL
const int MapHigherFunctions::size(std::shared_ptr<HigherObject> &obj1)
{
	return obj1->mapValue.size();
}

JDM_DLL
const int MapHigherFunctions::searchKey(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2)
{
	int index = 0;
	for (auto it = obj1->mapValue.begin(); it != obj1->mapValue.end(); ++it)
	{
		if (it->first->compareHigherObject(obj2)) return index;
		index++;
	}
	return -1;
}

JDM_DLL
const int MapHigherFunctions::searchValue(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2)
{
	int index = 0;
	for (auto it = obj1->mapValue.begin(); it != obj1->mapValue.end(); ++it)
	{
		if (it->second->compareHigherObject(obj2)) return index;
		index++;
	}
	return -1;
}

JDM_DLL
const std::shared_ptr<HigherObject> MapHigherFunctions::atKey(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2)
{
	for (auto it = obj1->mapValue.begin(); it != obj1->mapValue.end(); ++it)
	{
		if (it->first->compareHigherObject(obj2))
			return it->second;
	}
	return nullptr;
}

JDM_DLL
const std::shared_ptr<HigherObject> MapHigherFunctions::atValue(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2)
{
	for (auto it = obj1->mapValue.begin(); it != obj1->mapValue.end(); ++it)
	{
		if (it->second->compareHigherObject(obj2))
			return it->first;
	}
	return nullptr;
}

JDM_DLL
const std::shared_ptr<HigherObject> MapHigherFunctions::keys(std::shared_ptr<HigherObject> &obj1)
{
	std::vector<std::shared_ptr<HigherObject>> otherObject;
	for (const auto &obj : obj1->mapValue)
	{
		otherObject.push_back(obj.first);
	}
	return std::make_shared<HigherObject>(otherObject);
}

JDM_DLL
const std::shared_ptr<HigherObject> MapHigherFunctions::values(std::shared_ptr<HigherObject> &obj1)
{
	std::vector<std::shared_ptr<HigherObject>> otherObject;
	for (const auto &obj : obj1->mapValue)
	{
		otherObject.push_back(obj.second);
	}
	return std::make_shared<HigherObject>(otherObject);
}
