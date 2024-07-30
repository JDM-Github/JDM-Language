#include "utils/HigherObject.hpp"
#include "library/classes/Console.hpp"
#include "library/console/window.hpp"
#include "library/console/Drawer.hpp"

ConsoleClass::ConsoleClass()
{
	this->mapFunctions["run"       ] = ConsoleEnum::CONSOLE_RUN;
	this->mapFunctions["setUpdate" ] = ConsoleEnum::CONSOLE_SET_UPDATE;
	this->mapFunctions["setCreate" ] = ConsoleEnum::CONSOLE_SET_CREATE;

	this->mapFunctions["clear"        ] = ConsoleEnum::CONSOLE_CLEAR;
	this->mapFunctions["setAutoClear" ] = ConsoleEnum::CONSOLE_AUTO_CLEAR;

	this->mapFunctions["draw"    ] = ConsoleEnum::CONSOLE_DRAW;
	this->mapFunctions["drawBox" ] = ConsoleEnum::CONSOLE_DRAW_BOX;
}

std::shared_ptr<HigherObject> ConsoleClass::constructor(
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	obj1->objectValue->members["class"     ] = std::make_shared<HigherObject>("Console");
	obj1->objectValue->members["isRunning" ] = std::make_shared<HigherObject>(false);
	obj1->objectValue->members["Width"     ] = std::make_shared<HigherObject>(static_cast<int64_t>(50));
	obj1->objectValue->members["Height"    ] = std::make_shared<HigherObject>(static_cast<int64_t>(50));
	obj1->objectValue->members["FontWidth" ] = std::make_shared<HigherObject>(static_cast<int64_t>(5 ));
	obj1->objectValue->members["FontHeight"] = std::make_shared<HigherObject>(static_cast<int64_t>(5 ));

	obj1->objectValue->members["userUpdate"] = nullptr;
	obj1->objectValue->members["userCreate"] = nullptr;
	obj1->objectValue->members["autoClear" ] = std::make_shared<HigherObject>(false);

	obj1->objectValue->members["current"] = std::make_shared<HigherObject>(this->index++);
	windows.push_back(nullptr);
	return obj1;
}

std::shared_ptr<HigherObject> ConsoleClass::manageFunction(
	int funcType, std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	ConsoleEnum newfuncType = static_cast<ConsoleEnum>(funcType);

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
		{
			if (objects[i]->getCurrActive() != ACTIVE_INTEGER)
				throw std::runtime_error("Runtime Error: Console argument "
					+ std::to_string(i+1) +  "must be a 'jint'");
		}

		obj1->objectValue->members["isRunning" ]->booleanValue = true;
		obj1->objectValue->members["Width"     ]->integerValue = ((objects.size() > 1) ? objects[1]->integerValue : obj1->objectValue->members["Width"     ]->integerValue);
		obj1->objectValue->members["Height"    ]->integerValue = ((objects.size() > 2) ? objects[2]->integerValue : obj1->objectValue->members["Height"    ]->integerValue);
		obj1->objectValue->members["FontWidth" ]->integerValue = ((objects.size() > 3) ? objects[3]->integerValue : obj1->objectValue->members["FontWidth" ]->integerValue);
		obj1->objectValue->members["FontHeight"]->integerValue = ((objects.size() > 4) ? objects[4]->integerValue : obj1->objectValue->members["FontHeight"]->integerValue);
		this->runConsole(
			this->getCurrent(obj1),
			objects[0]->stringValue,
			obj1->objectValue->members["Width"     ]->integerValue,
			obj1->objectValue->members["Height"    ]->integerValue,
			obj1->objectValue->members["FontWidth" ]->integerValue,
			obj1->objectValue->members["FontHeight"]->integerValue
		);
	}
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_SET_UPDATE)
	{
		if (obj1->objectValue->fromMainSource)
			throw std::runtime_error("Runtime Error: The 'Console.setUpdate' method is not static.");

		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: 'Console.setUpdate' Expecting one argument.");

		if (objects[0]->getCurrActive() != ACTIVE_FUNCTION)
			throw std::runtime_error("Runtime Error: 'Console.setUpdate' argument must be a 'jfunc' or 'jlambda'");

		obj1->objectValue->members["userUpdate"] = std::make_shared<HigherObject>(objects[0]->funcValue);
	}
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_SET_CREATE)
	{
		if (obj1->objectValue->fromMainSource)
			throw std::runtime_error("Runtime Error: The 'Console.setCreate' method is not static.");

		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: 'Console.setCreate' Expecting one argument.");

		if (objects[0]->getCurrActive() != ACTIVE_FUNCTION)
			throw std::runtime_error("Runtime Error: 'Console.setCreate' argument must be a 'jfunc' or 'jlambda'");

		obj1->objectValue->members["userCreate"] = std::make_shared<HigherObject>(objects[0]->funcValue);
	}
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_START_UPDATE)
	{
		if (obj1->objectValue->fromMainSource)
			throw std::runtime_error("Runtime Error: The 'run' method is not static.");

		this->startUpdateConsole(this->getCurrent(obj1),
			obj1->objectValue->members["autoClear"]->booleanValue);
	}
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_UPDATE)
	{
		if (obj1->objectValue->fromMainSource)
			throw std::runtime_error("Runtime Error: The 'run' method is not static.");

		return std::make_shared<HigherObject>(this->updateConsole(this->getCurrent(obj1)));
	}

	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_SET_UPDATE)
	{
		if (obj1->objectValue->fromMainSource)
			throw std::runtime_error("Runtime Error: The 'Console.setUpdate' method is not static.");

		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: 'Console.setUpdate' Expecting one argument.");

		if (objects[0]->getCurrActive() != ACTIVE_FUNCTION)
			throw std::runtime_error("Runtime Error: 'Console.setUpdate' argument must be a 'jfunc' or 'jlambda'");

		obj1->objectValue->members["userUpdate"] = std::make_shared<HigherObject>(objects[0]->funcValue);
	}

	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_CLEAR)
	{
		this->clearConsole(obj1, objects);
	}
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_AUTO_CLEAR)
	{
		if (obj1->objectValue->fromMainSource)
			throw std::runtime_error("Runtime Error: The 'Console.setUpdate' method is not static.");

		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: 'Console.setUpdate' Expecting one argument.");

		obj1->objectValue->members["autoClear"]->booleanValue = true;
	}

	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_DRAW)
		this->drawConsole(obj1, objects);
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_DRAW_BOX)
		this->drawBox(obj1, objects);
	else
		throw std::runtime_error("Runtime Error: This function is not a member of class FILEEE.");

	return nullptr;
}

JDM_DLL
int64_t ConsoleClass::getCurrent(const std::shared_ptr<HigherObject> &obj1)
{
	return obj1->objectValue->members["current"]->integerValue;
}

const void ConsoleClass::runConsole(
	int64_t current, const std::string& title, short Width,
	short Height, short fontWidth, short fontHeight)
{
	if (this->windows[current] == nullptr)
		this->windows[current] = std::make_shared<Window>(title.c_str(), Width, Height, fontWidth, fontHeight);

	this->windows[current]->Start();
}

const void ConsoleClass::startUpdateConsole(int64_t current, bool clear)
{
	this->windows[current]->startUpdate(clear);
}

const bool ConsoleClass::updateConsole(int64_t current)
{
	return this->windows[current]->updateGame();
}

const void ConsoleClass::clearConsole(
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (obj1->objectValue->fromMainSource)
			throw std::runtime_error("Runtime Error: The 'Console.clear' method is not static.");

	if (objects.size() > 0)
		throw std::runtime_error("Runtime Error: 'Console.clear' does not take any argument.");

	this->windows[this->getCurrent(obj1)]->Clear();
}

const void ConsoleClass::drawConsole(
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (objects.size() < 2 || objects.size() > 5)
		throw std::runtime_error("Runtime Error: 'Console.draw' expects 2 to 5 arguments.");

	if (!objects[0]->isNumber() || !objects[1]->isNumber())
		throw std::runtime_error("Runtime Error: 'Console.draw' expects the first and the second arguments to be number.");

	objects[0]->castToInteger();
	objects[1]->castToInteger();
	float x = objects[0]->integerValue;
	float y = objects[1]->integerValue;
	short character = JDMConsole::PIXEL::PIXEL_SOLID;
	short color     = JDMConsole::FG_WHITE | JDMConsole::BG_BLACK;
	bool alpha      = false;

	if (objects.size() > 2)
	{
		if (objects[2]->getCurrActive() != ACTIVE_INTEGER)
			throw std::runtime_error("Runtime Error: 'Console.draw' expects the third argument to be a 'jint' (character).");
		character = objects[2]->integerValue;
	}

	if (objects.size() > 3)
	{
		if (objects[3]->getCurrActive() != ACTIVE_INTEGER)
			throw std::runtime_error("Runtime Error: 'Console.draw' expects the fourth argument to be a 'jint' (color).");
		character = objects[3]->integerValue;
	}

	if (objects.size() > 4)
	{
		if (objects[4]->getCurrActive() != ACTIVE_BOOLEAN)
			throw std::runtime_error("Runtime Error: 'Console.draw' expects the fifth argument to be a 'jboolean' (alpha).");
		alpha = objects[4]->booleanValue;
	}
	this->windows[this->getCurrent(obj1)]->Draw({x, y}, character, color, alpha);
}

// const void drawCycle       (int64_t current, float x, float y, short character, short color, bool alpha)
// {

// }

// const void drawCString     (int64_t current, float x, float y, const std::string &character, bool alpha)
// {

// }
// const void drawCStringCycle(int64_t current, float x, float y, const std::string &character, bool alpha)
// {

// }

// const void drawString      (int64_t current, float x, float y, const std::string &character, short color, bool alpha)
// {

// }
// const void drawACString    (int64_t current, float x, float y, const std::string &character, short character, short color, bool alpha)
// {

// }

// const void drawHorizontal  (int64_t current, float x, float y, short character, short color, bool alpha)
// {

// }
// const void drawVertical    (int64_t current, float x, float y, short character, short color, bool alpha)
// {

// }
// const void drawLine        (int64_t current, float w, float x, float y, float z, short character, short color, bool alpha)
// {

// }
// const void drawTriangle    (int64_t current, float u, float v, float w, float x, float y, float z, short character, short color, bool alpha)
// {

// }

const void ConsoleClass::drawBox(
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (objects.size() < 4 || objects.size() > 7)
		throw std::runtime_error("Runtime Error: 'Console.drawBox' expects 4 to 7 arguments.");

	if (!objects[0]->isNumber() || !objects[1]->isNumber() || !objects[2]->isNumber() || !objects[3]->isNumber())
		throw std::runtime_error("Runtime Error: 'Console.drawBox' expects the first four arguments to be numbers.");

	objects[0]->castToInteger();
	objects[1]->castToInteger();
	objects[2]->castToInteger();
	objects[3]->castToInteger();
	float xBox = objects[0]->integerValue;
	float yBox = objects[1]->integerValue;
	int wBox = objects[2]->integerValue;
	int hBox = objects[3]->integerValue;
	short character = JDMConsole::PIXEL::PIXEL_SOLID;
	short color     = JDMConsole::FG_WHITE | JDMConsole::BG_BLACK;
	bool  alpha     = false;
	if (objects.size() > 4)
	{
		if (objects[4]->getCurrActive() != ACTIVE_INTEGER)
			throw std::runtime_error("Runtime Error: 'Console.drawBox' expects the fifth argument to be a 'jint' (character).");
		character = objects[4]->integerValue;
	}
	if (objects.size() > 5)
	{
		if (objects[5]->getCurrActive() != ACTIVE_INTEGER)
			throw std::runtime_error("Runtime Error: 'Console.drawBox' expects the sixth argument to be a 'jint' (color).");
		color = objects[5]->integerValue;
	}
	if (objects.size() > 6)
	{
		if (objects[6]->getCurrActive() != ACTIVE_BOOLEAN)
			throw std::runtime_error("Runtime Error: 'Console.drawBox' expects the seventh argument to be a 'jboolean' (alpha).");
		alpha = objects[6]->booleanValue;
	}
	this->windows[this->getCurrent(obj1)]->DrawBox({wBox, hBox, xBox, yBox}, character, color, alpha);
}
