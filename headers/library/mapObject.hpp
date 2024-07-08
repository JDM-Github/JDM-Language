#pragma once

class JDM_DLL MapHigherFunctions
{
public:
	enum MapFunction
	{
    	map_size,
    	map_atKey,
    	map_atValue,
    	map_SearchKey,
    	map_SearchValue,
    	map_keys,
    	map_values
	};

	JDM_DLL static std::unordered_map<std::string, MapFunction> mapFunctions;
	JDM_DLL static const std::shared_ptr<HigherObject> manageFunction(
		MapHigherFunctions::MapFunction mapFuncType,
		std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects);

	JDM_DLL static const int size(std::shared_ptr<HigherObject> &obj1);

	JDM_DLL static const int searchKey(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2);
	JDM_DLL static const int searchValue(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2);

	JDM_DLL static const std::shared_ptr<HigherObject> atKey(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2);

	JDM_DLL static const std::shared_ptr<HigherObject> atValue(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2);

	JDM_DLL static const std::shared_ptr<HigherObject> keys(std::shared_ptr<HigherObject> &obj1);

	JDM_DLL static const std::shared_ptr<HigherObject> values(std::shared_ptr<HigherObject> &obj1);
};
