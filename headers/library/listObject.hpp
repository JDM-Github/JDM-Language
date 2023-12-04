#pragma once
#include "../higherObject.hpp"

class JDM_DLL ListHigherFunctions {
public:
	enum ListFunction {
    	list_size,
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
    	list_rdup
	};
	JDM_DLL static std::unordered_map<std::string, ListFunction> listFunctions;
	JDM_DLL static const std::shared_ptr<HigherObject> manageFunction(
		ListHigherFunctions::ListFunction listFuncType,
		std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects);

	JDM_DLL static const int size(std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const int search(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2);

	JDM_DLL static const void sort(std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const void reverse(std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const void del(std::shared_ptr<HigherObject> &obj1, int index);
	JDM_DLL static const void push_back(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2);
	JDM_DLL static const void insert(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2, int index = -1);
	JDM_DLL static const void push_front(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2);
	JDM_DLL static const void pop_back(std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const void pop_front(std::shared_ptr<HigherObject> &obj1);
	JDM_DLL static const void rdup(std::shared_ptr<HigherObject> &obj1);

	JDM_DLL static const std::shared_ptr<HigherObject> at(std::shared_ptr<HigherObject> &obj1, int index);

};
