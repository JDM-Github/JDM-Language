#include "Compiler.hpp"

JDM_DLL
const std::shared_ptr<HigherObject> Compiler::handleNativeFunction(
	NativeFunction::NativeFunctionEnum nativeType,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_FILTER)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments. Target ITERABLE and Function.");

		if (objects[1]->getCurrActive() != ACTIVE_FUNCTION)
			throw std::runtime_error("Runtime Error: Invalid Function arguments on 'filter'.");

		if (objects[0]->getCurrActive() != ACTIVE_LIST
		 && objects[0]->getCurrActive() != ACTIVE_MAP)
			return objects[0];
		else
		{
			auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();
			newList->listValue.erase(std::remove_if(newList->listValue.begin(), newList->listValue.end(),
				[&](const std::shared_ptr<HigherObject> &obj1)
				{
					std::shared_ptr<HigherObject> filterResult = this->runFunction(objects[1]->funcValue, { obj1 });
					filterResult->castToBoolean(); return !filterResult->booleanValue;
				}),
			newList->listValue.end());
			return newList;
		}
	}
	else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_UNIQUE)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: Expecting 1 argument. Target ITERABLE");

		if (objects[0]->getCurrActive() != ACTIVE_LIST && objects[0]->getCurrActive() != ACTIVE_MAP)
			return objects[0];
		else
		{
			auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();
			auto resultList = std::make_shared<HigherObject>(static_cast<int64_t>(0)); resultList->castToList();
			resultList->listValue.clear();
			for (int i = 0; i < newList->listValue.size(); i++)
			{
				if (!resultList->isInList(newList->listValue[i]))
					resultList->listValue.push_back(std::make_shared<HigherObject>(newList->listValue[i]));
			}
			return resultList;
		}
	}
	else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_REDUCE)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments. Target ITERABLE and Function.");

		if (objects[1]->getCurrActive() != ACTIVE_FUNCTION)
			throw std::runtime_error("Runtime Error: Invalid Function arguments on 'reduce'.");

		if (objects[0]->getCurrActive() != ACTIVE_LIST && objects[0]->getCurrActive() != ACTIVE_MAP)
			return objects[0];
		else
		{
			auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();
			if (newList->listValue.empty())
				throw std::runtime_error("Runtime Error: Using 'reduce' on empty ITERABLE.");

			auto result = newList->listValue[0];
			for (int i = 1; i < newList->listValue.size(); i++)
				result = this->runFunction(objects[1]->funcValue, { result, newList->listValue[i] });
			return result;
		}
	}
	else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_MAP)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments. Target ITERABLE and Function.");

		if (objects[1]->getCurrActive() != ACTIVE_FUNCTION)
			throw std::runtime_error("Runtime Error: Invalid Function arguments on 'reduce'.");

		if (objects[0]->getCurrActive() != ACTIVE_LIST
		 && objects[0]->getCurrActive() != ACTIVE_MAP)
			return objects[0];
		else
		{
			auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();
			for (int i = 0; i < newList->listValue.size(); i++)
				newList->listValue[i] = this->runFunction(objects[1]->funcValue, { newList->listValue[i] });
			return newList;
		}
	}
	else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_SORT_IF)
	{
		if (objects.size() != 2)
			throw std::runtime_error("Runtime Error: Expecting 2 arguments. Target ITERABLE and Function.");

		if (objects[1]->getCurrActive() != ACTIVE_FUNCTION)
			throw std::runtime_error("Runtime Error: Invalid Function arguments on 'reduce'.");

		if (objects[0]->getCurrActive() != ACTIVE_LIST
		 && objects[0]->getCurrActive() != ACTIVE_MAP)
			return objects[0];
		else
		{
			auto newList = std::make_shared<HigherObject>(objects[0]); newList->castToList();
			std::sort(newList->listValue.begin(), newList->listValue.end(),
				[&](const std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2)
				{
					std::shared_ptr<HigherObject> conditionResult = this->runFunction(objects[1]->funcValue, { obj1, obj2 });
					conditionResult->castToBoolean(); return conditionResult->booleanValue;
				});
			return newList;
		}
	}
	else if (nativeType == NativeFunction::NativeFunctionEnum::NATIVE_PARTIAL)
	{
		if (objects.size() < 1)
			throw std::runtime_error("Runtime Error: Expecting atleast 1 argument. Target Function.");

		if (objects[0]->getCurrActive() != ACTIVE_FUNCTION)
			throw std::runtime_error("Runtime Error: Invalid Function arguments on 'partial'.");

		auto newFunction              = std::make_shared<FunctionCall>();
		newFunction->funcName         = objects[0]->funcValue->funcName;
		newFunction->varNameAccesible = objects[0]->funcValue->varNameAccesible;
		newFunction->blockWillRun     = objects[0]->funcValue->blockWillRun;
		newFunction->parameters       = objects[0]->funcValue->parameters;
		newFunction->preArgs          = { objects.begin()+1, objects.end() };
		return std::make_shared<HigherObject>(newFunction);
	}
	else
		return NativeFunction::manageFunction( nativeType, objects );

	return nullptr;
}
