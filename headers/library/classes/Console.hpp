#pragma once
#include "baseClasses.hpp"
#include "../console/window.hpp"

class ConsoleClass : public BaseNativeClass {
public:
	enum ConsoleEnum {
		CONSOLE_RUN
	};

	ConsoleClass() {
		this->mapFunctions["run" ] = ConsoleEnum::CONSOLE_RUN;
	}
	std::shared_ptr<HigherObject> constructor(
		std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects)
	{
		obj1->objectValue->members["isRunning" ] = std::make_shared<HigherObject>(false);
		obj1->objectValue->members["Width"     ] = std::make_shared<HigherObject>(static_cast<int64_t>(50));
		obj1->objectValue->members["Height"    ] = std::make_shared<HigherObject>(static_cast<int64_t>(50));
		obj1->objectValue->members["FontWidth" ] = std::make_shared<HigherObject>(static_cast<int64_t>(5 ));
		obj1->objectValue->members["FontHeight"] = std::make_shared<HigherObject>(static_cast<int64_t>(5 ));
		return obj1;
	}

	std::shared_ptr<HigherObject> manageFunction(int funcType, std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects)
	{
		ConsoleEnum newfuncType = static_cast<ConsoleEnum>(funcType);
		std::shared_ptr<HigherObject> newReturn;

		if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_RUN)
		{
			if (obj1->objectValue->fromMainSource)
				throw std::runtime_error("Runtime Error: The 'run' method is not static.");
			if (obj1->objectValue->members["isRunning"]->booleanValue)
				throw std::runtime_error("Runtime Error: The Console is already running.");
			if (objects.empty())
				throw std::runtime_error("Runtime Error: Expecting atleast 1 argument.");
			if (objects.size() > 5)
				throw std::runtime_error("Runtime Error: Expecting only 5 arguments.");
			if (objects[0]->getCurrActive() != ACTIVE_STRING)
				throw std::runtime_error("Runtime Error: Console title must be a 'jstring'");

			for (int i = 1; i < objects.size(); i++)
				if (objects[i]->getCurrActive() != ACTIVE_INTEGER)
					throw std::runtime_error("Runtime Error: Console argument " + std::to_string(i+1) +  "must be a 'jint'");

			obj1->objectValue->members["isRunning" ]->booleanValue = true;
			obj1->objectValue->members["Width"     ]->integerValue = ((objects.size() > 1) ? objects[1]->integerValue : obj1->objectValue->members["Width"     ]->integerValue);
			obj1->objectValue->members["Height"    ]->integerValue = ((objects.size() > 2) ? objects[2]->integerValue : obj1->objectValue->members["Height"    ]->integerValue);
			obj1->objectValue->members["FontWidth" ]->integerValue = ((objects.size() > 3) ? objects[3]->integerValue : obj1->objectValue->members["FontWidth" ]->integerValue);
			obj1->objectValue->members["FontHeight"]->integerValue = ((objects.size() > 4) ? objects[4]->integerValue : obj1->objectValue->members["FontHeight"]->integerValue);
			this->runConsole(objects[0]->stringValue,
				obj1->objectValue->members["Width"     ]->integerValue,
				obj1->objectValue->members["Height"    ]->integerValue,
				obj1->objectValue->members["FontWidth" ]->integerValue,
				obj1->objectValue->members["FontHeight"]->integerValue
			);
		}
		else throw std::runtime_error("Runtime Error: This function is not a member of class FILEEE.");

		return newReturn;
	}

	void runConsole(const std::string& title, short Width = 100, short Height = 50, short fontWidth = 5, short fontHeight = 5) {
		std::make_shared<Window>(title.c_str(), Width, Height, fontWidth, fontHeight)->Start();
	}
};