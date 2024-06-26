#pragma once
#include "../higherObject.hpp"

class JDM_DLL ListHigherFunctions {
public:
	enum ListFunction {
    	list_size,
    	list_count,
    	list_sort,
    	list_search,
    	list_insert,
    	list_delete,
    	list_at,
    	list_reverse,
    	list_push_back,
    	list_push_front,
    	list_pop_back,
    	list_pop_front,
	};
	JDM_DLL static std::unordered_map<std::string, ListFunction> listFunctions;
	JDM_DLL static const std::shared_ptr<HigherObject> manageFunction(
		ListHigherFunctions::ListFunction listFuncType, std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects);

	JDM_DLL StaticCInt size       (std::shared_ptr<HigherObject> &obj1);
	JDM_DLL StaticCInt count      (std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2);
	JDM_DLL StaticCInt search     (std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2);

	JDM_DLL StaticCVoid sort      (std::shared_ptr<HigherObject> &obj1);
	JDM_DLL StaticCVoid reverse   (std::shared_ptr<HigherObject> &obj1);
	JDM_DLL StaticCVoid del       (std::shared_ptr<HigherObject> &obj1, int index);
	JDM_DLL StaticCVoid push_back (std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2);
	JDM_DLL StaticCVoid insert    (std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2, int index = -1);
	JDM_DLL StaticCVoid push_front(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2);
	JDM_DLL StaticCVoid pop_back  (std::shared_ptr<HigherObject> &obj1);
	JDM_DLL StaticCVoid pop_front (std::shared_ptr<HigherObject> &obj1);
	JDM_DLL StaticCVoid rdup      (std::shared_ptr<HigherObject> &obj1);

	JDM_DLL static const std::shared_ptr<HigherObject> at(std::shared_ptr<HigherObject> &obj1, int index);

};
