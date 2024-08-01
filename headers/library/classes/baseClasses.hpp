#pragma once

class BaseNativeClass {
public:
	BaseNativeClass() {}
	virtual ~BaseNativeClass() {}
	virtual const void init() {}

	std::unordered_map<std::string, int> mapFunctions;

	virtual std::shared_ptr<HigherObject> constructor(
		std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects)
	{ return nullptr; }

	virtual std::shared_ptr<HigherObject> manageFunction(
		int funcType,
		std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects)
	{
		return nullptr;
	}
};
