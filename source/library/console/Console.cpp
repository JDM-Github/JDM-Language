#include "utils/HigherObject.hpp"
#include "library/classes/BaseClasses.hpp"
#include "library/console/Console.hpp"
#include "library/console/window.hpp"
#include "library/console/Drawer.hpp"

JDM_DLL
ConsoleClass::ConsoleClass()
{
	this->initializeVariable();
	this->initializeFunction();
}

JDM_DLL
const void ConsoleClass::initializeFunction()
{
	this->mapFunctions["run"       ] = ConsoleEnum::CONSOLE_RUN;
	this->mapFunctions["setUpdate" ] = ConsoleEnum::CONSOLE_SET_UPDATE;
	this->mapFunctions["setCreate" ] = ConsoleEnum::CONSOLE_SET_CREATE;

	this->mapFunctions["clear"        ] = ConsoleEnum::CONSOLE_CLEAR;
	this->mapFunctions["setAutoClear" ] = ConsoleEnum::CONSOLE_AUTO_CLEAR;

	this->mapFunctions["getMouseX"    ] = ConsoleEnum::CONSOLE_GET_MOUSE_X;
	this->mapFunctions["getMouseY"    ] = ConsoleEnum::CONSOLE_GET_MOUSE_Y;
	this->mapFunctions["getMouse"     ] = ConsoleEnum::CONSOLE_GET_MOUSE;
	this->mapFunctions["getElapseTime"] = ConsoleEnum::CONSOLE_GET_ELAPSETIME;

	this->mapFunctions["draw"          ] = ConsoleEnum::CONSOLE_DRAW;
	this->mapFunctions["drawString"    ] = ConsoleEnum::CONSOLE_DRAW_STRING;
	this->mapFunctions["drawStringChar"] = ConsoleEnum::CONSOLE_DRAW_STRING_CHAR;
	this->mapFunctions["drawBox"       ] = ConsoleEnum::CONSOLE_DRAW_BOX;

	this->mapFunctions["drawCycle"          ] = ConsoleEnum::CONSOLE_DRAW_CYCLE;
	this->mapFunctions["drawStringCycle"    ] = ConsoleEnum::CONSOLE_DRAW_STRING_CYCLE;
	this->mapFunctions["drawStringCharCycle"] = ConsoleEnum::CONSOLE_DRAW_STRING_CHAR_CYCLE;
	this->mapFunctions["drawBoxCycle"       ] = ConsoleEnum::CONSOLE_DRAW_BOX_CYCLE;

	this->mapFunctions["drawColoredString"         ] = ConsoleEnum::CONSOLE_DRAW_COLORED_STRING;
	this->mapFunctions["drawColoredStringCycle"    ] = ConsoleEnum::CONSOLE_DRAW_COLORED_STRING_CYCLE;
	this->mapFunctions["drawColoredStringChar"     ] = ConsoleEnum::CONSOLE_DRAW_COLORED_STRING_CHAR;
	this->mapFunctions["drawColoredStringCharCycle"] = ConsoleEnum::CONSOLE_DRAW_COLORED_STRING_CHAR_CYCLE;

	this->mapFunctions["isHeld"    ] = ConsoleEnum::CONSOLE_IS_HELD;
	this->mapFunctions["isReleased"] = ConsoleEnum::CONSOLE_IS_RELEASED;
	this->mapFunctions["isPressed" ] = ConsoleEnum::CONSOLE_IS_PRESSED;
}

JDM_DLL
const void ConsoleClass::initializeVariable()
{

}

JDM_DLL
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

// JDM_DLL
// std::shared_ptr<HigherObject> ConsoleClass::manageVariable(
// 	int varType,
// 	std::shared_ptr<HigherObject> &obj1)
// {
// 	ConsoleEnumVariable newVarType = static_cast<ConsoleEnumVariable>(varType);
// 	switch (newVarType)
// 	{
// 	case ConsoleEnumVariable::FG_BLACK:
// 		return 
// 	default:
// 		throw std::runtime_error("Runtime Error: This classfield is not a member of class 'Console'.");
// 	}
// }

JDM_DLL
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

	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_GET_ELAPSETIME)
		return std::make_shared<HigherObject>(this->getElapseTime(obj1, objects));
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_GET_MOUSE_X)
		return std::make_shared<HigherObject>(this->getMouseX(obj1, objects));
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_GET_MOUSE_Y)
		return std::make_shared<HigherObject>(this->getMouseY(obj1, objects));
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_GET_MOUSE)
		return std::make_shared<HigherObject>(this->getMouse(obj1, objects));

	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_DRAW)
		this->drawConsole(obj1, objects);
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_DRAW_CYCLE)
		this->drawCycle(obj1, objects);

	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_DRAW_STRING)
		this->drawString(obj1, objects);
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_DRAW_STRING_CYCLE)
		this->drawString(obj1, objects, true);

	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_DRAW_STRING_CHAR)
		this->drawStringChar(obj1, objects);
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_DRAW_STRING_CHAR_CYCLE)
		this->drawStringChar(obj1, objects, true);

	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_DRAW_COLORED_STRING)
		this->drawCString(obj1, objects);
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_DRAW_COLORED_STRING_CYCLE)
		this->drawCString(obj1, objects, true);
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_DRAW_COLORED_STRING_CHAR)
		this->drawCStringChar(obj1, objects);
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_DRAW_COLORED_STRING_CHAR_CYCLE)
		this->drawCStringChar(obj1, objects, true);

	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_DRAW_BOX)
		this->drawBox(obj1, objects);
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_DRAW_BOX_CYCLE)
		this->drawBox(obj1, objects, true);

	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_IS_HELD)
		return std::make_shared<HigherObject>(this->isHeld(obj1, objects));
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_IS_RELEASED)
		return std::make_shared<HigherObject>(this->isReleased(obj1, objects));
	else if (newfuncType == ConsoleClass::ConsoleEnum::CONSOLE_IS_PRESSED)
		return std::make_shared<HigherObject>(this->isPressed(obj1, objects));
	else
		throw std::runtime_error("Runtime Error: This method is not a member of class 'Console'.");

	return nullptr;
}

JDM_DLL
int64_t ConsoleClass::getCurrent(const std::shared_ptr<HigherObject> &obj1)
{
	return obj1->objectValue->members["current"]->integerValue;
}

JDM_DLL
const void ConsoleClass::runConsole(
	int64_t current, const std::string& title, short Width,
	short Height, short fontWidth, short fontHeight)
{
	if (this->windows[current] == nullptr)
		this->windows[current] = std::make_shared<Window>(title.c_str(), Width, Height, fontWidth, fontHeight);

	this->windows[current]->Start();
}

JDM_DLL
const void ConsoleClass::startUpdateConsole(int64_t current, bool clear)
{
	this->windows[current]->startUpdate(clear);
}

JDM_DLL
const bool ConsoleClass::updateConsole(int64_t current)
{
	return this->windows[current]->updateGame();
}

JDM_DLL
const void ConsoleClass::clearConsole(
	const std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (obj1->objectValue->fromMainSource)
		throw std::runtime_error("Runtime Error: The 'Console.clear' method is not static.");

	if (objects.size() > 0)
		throw std::runtime_error("Runtime Error: 'Console.clear' does not take any argument.");

	this->windows[this->getCurrent(obj1)]->Clear();
}

JDM_DLL
const long double ConsoleClass::getElapseTime(
	const std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (obj1->objectValue->fromMainSource)
		throw std::runtime_error("Runtime Error: The 'Console.getElapseTime' method is not static.");

	if (objects.size() > 0)
		throw std::runtime_error("Runtime Error: 'Console.getElapseTime' does not take any argument.");

	return this->windows[this->getCurrent(obj1)]->GetElapseTime();
}

JDM_DLL
const int64_t ConsoleClass::getMouseX(
	const std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (obj1->objectValue->fromMainSource)
		throw std::runtime_error("Runtime Error: The 'Console.getMouseX' method is not static.");

	if (objects.size() > 0)
		throw std::runtime_error("Runtime Error: 'Console.getMouseX' does not take any argument.");

	return this->windows[this->getCurrent(obj1)]->GetMouseX();
}

JDM_DLL
const int64_t ConsoleClass::getMouseY(
	const std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (obj1->objectValue->fromMainSource)
		throw std::runtime_error("Runtime Error: The 'Console.getMouseY' method is not static.");

	if (objects.size() > 0)
		throw std::runtime_error("Runtime Error: 'Console.getMouseY' does not take any argument.");

	return this->windows[this->getCurrent(obj1)]->GetMouseY();
}

JDM_DLL
const std::vector<std::shared_ptr<HigherObject>> ConsoleClass::getMouse(
	const std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (obj1->objectValue->fromMainSource)
		throw std::runtime_error("Runtime Error: The 'Console.getMouse' method is not static.");

	if (objects.size() > 0)
		throw std::runtime_error("Runtime Error: 'Console.getMouse' does not take any argument.");

	return {
		std::make_shared<HigherObject>(this->windows[this->getCurrent(obj1)]->GetMouseX()),
		std::make_shared<HigherObject>(this->windows[this->getCurrent(obj1)]->GetMouseY())
	};
}

JDM_DLL
const void ConsoleClass::setFirstArgs(
	const std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects,
	int first, int last, const std::string &text,
	float *u, float *v, float *w, float *x, float *y, float *z)
{
	if (obj1->objectValue->fromMainSource)
		throw std::runtime_error("Runtime Error: The '" + text + "' method is not static.");

	std::string firstStr = std::to_string(first);
	if (objects.size() < first || objects.size() > last)
		throw std::runtime_error("Runtime Error: '" + text + "' expects " + firstStr + " to " + std::to_string(last) + " arguments.");

	for (int i = 0; i < std::min(first, 6); i++)
	{
		if (!objects[i]->isNumber())
			throw std::runtime_error("Runtime Error: '" + text + "' expects the first " + firstStr + " to be a number.");

		objects[i]->castToInteger();
		int64_t value = objects[i]->integerValue;
		if (i == 0) *u = value;
		if (i == 1) *v = value;
		if (i == 2) *w = value;
		if (i == 3) *x = value;
		if (i == 4) *y = value;
		if (i == 5) *z = value;
	}
}

JDM_DLL
const void ConsoleClass::checkLastArguments(
	const std::vector<std::shared_ptr<HigherObject>> &objects, int start,
	short &character, short &color, bool &alpha, const std::string &text)
{
	if (objects.size() > 0)
	{
		auto active = objects[0]->getCurrActive();
		auto errorMess = "Runtime Error: '" + text + "' expects the " + std::to_string(start)
					+ " argument to be a 'jstring' with 1 character or 'jint' (character).";
		if (active == ACTIVE_STRING)
		{
			if (objects[0]->stringValue.size() > 1)
				throw std::runtime_error(errorMess);
			character = static_cast<short>(objects[0]->stringValue[0]);
		}
		else if (active == ACTIVE_INTEGER)
			character = static_cast<short>(objects[0]->integerValue);
		else
			throw std::runtime_error(errorMess);
	}
	if (objects.size() > 1)
	{
		if (objects[1]->getCurrActive() != ACTIVE_INTEGER)
			throw std::runtime_error("Runtime Error: '" + text + "' expects the " + std::to_string(start+1) + " argument to be a 'jint' (color).");
		color = objects[1]->integerValue;
	}
	if (objects.size() > 2)
	{
		if (objects[2]->getCurrActive() != ACTIVE_BOOLEAN)
			throw std::runtime_error("Runtime Error: '" + text + "' expects the " + std::to_string(start+2) + " argument to be a 'jboolean' (alpha).");
		alpha = objects[2]->booleanValue;
	}
}

JDM_DLL
const void ConsoleClass::drawConsole(
	const std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	float x, y;
	this->setFirstArgs(obj1, objects, 2, 5, "Console.draw", &x, &y);

	short character = JDMConsole::PIXEL::PIXEL_SOLID;
	short color     = JDMConsole::FG_WHITE | JDMConsole::BG_BLACK;
	bool  alpha     = false;

	this->checkLastArguments({objects.begin() + 2, objects.end()}, 3, character, color, alpha, "Console.draw");
	this->windows[this->getCurrent(obj1)]->Draw({x, y}, character, color, alpha);
}

JDM_DLL
const void ConsoleClass::drawCycle(
	const std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	float x, y;
	this->setFirstArgs(obj1, objects, 2, 5, "Console.drawCycle", &x, &y);

	short character = JDMConsole::PIXEL::PIXEL_SOLID;
	short color     = JDMConsole::FG_WHITE | JDMConsole::BG_BLACK;
	bool  alpha     = false;

	this->checkLastArguments({objects.begin() + 2, objects.end()}, 3, character, color, alpha, "Console.drawCycle");
	this->windows[this->getCurrent(obj1)]->DrawCycle({x, y}, character, color, alpha);
}

JDM_DLL
const void ConsoleClass::drawString(
	const std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects,
	const bool cycle)
{
	std::string strFunc = "Console.drawString";
	if (cycle) strFunc += "Cycle";

	if (objects.size() < 3 || objects.size() > 5)
		throw std::runtime_error("Runtime Error: '" + strFunc + "' expects 3 to 5 arguments.");

	float x, y;
	this->setFirstArgs(obj1, objects, 2, 5, strFunc, &x, &y);

	if (objects[2]->getCurrActive() != ACTIVE_STRING)
		throw std::runtime_error("Runtime Error: '" + strFunc + "' expects the 3 argument to be a 'jstring' (Text to display).");
	std::wstring textToDisplay = {objects[2]->stringValue.begin(), objects[2]->stringValue.end()};

	short color     = JDMConsole::FG_WHITE | JDMConsole::BG_BLACK;
	bool  alpha     = false;
	if (objects.size() > 3)
	{
		if (objects[3]->getCurrActive() != ACTIVE_INTEGER)
			throw std::runtime_error("Runtime Error: '" + strFunc + "' expects the 4 argument to be a 'jint' (color).");
		color = objects[3]->integerValue;
	}
	if (objects.size() > 4)
	{
		if (objects[4]->getCurrActive() != ACTIVE_BOOLEAN)
			throw std::runtime_error("Runtime Error: '" + strFunc + "' expects the 5 argument to be a 'jboolean' (alpha).");
		alpha = objects[4]->booleanValue;
	}
	this->windows[this->getCurrent(obj1)]->DrawString({x, y}, textToDisplay, color, alpha, cycle);
}

JDM_DLL
const void ConsoleClass::drawStringChar(
	const std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects,
	const bool cycle)
{
	std::string strFunc = "Console.drawStringChar";
	if (cycle) strFunc += "Cycle";

	if (objects.size() < 3 || objects.size() > 6)
		throw std::runtime_error("Runtime Error: '" + strFunc + "' expects 3 to 6 arguments.");

	float x, y;
	this->setFirstArgs(obj1, objects, 2, 6, strFunc, &x, &y);

	if (objects[2]->getCurrActive() != ACTIVE_STRING)
		throw std::runtime_error("Runtime Error: '" + strFunc + "' expects the 3 argument to be a 'jstring' (Text to display).");
	std::wstring textToDisplay = {objects[2]->stringValue.begin(), objects[2]->stringValue.end()};

	short character = JDMConsole::PIXEL::PIXEL_SOLID;
	short color     = JDMConsole::FG_WHITE | JDMConsole::BG_BLACK;
	bool  alpha     = false;
	this->checkLastArguments({objects.begin() + 3, objects.end()}, 6, character, color, alpha, strFunc);
	this->windows[this->getCurrent(obj1)]->DrawStringChar({x, y}, textToDisplay, character, color, alpha, cycle);
}

JDM_DLL
const void ConsoleClass::drawCString(
	const std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects,
	const bool cycle)
{
	std::string strFunc = "Console.drawColoredString";
	if (cycle) strFunc += "Cycle";

	if (objects.size() < 3 || objects.size() > 4)
		throw std::runtime_error("Runtime Error: '" + strFunc + "' expects 3 to 4 arguments.");

	float x, y;
	this->setFirstArgs(obj1, objects, 2, 4, strFunc, &x, &y);

	if (objects[2]->getCurrActive() != ACTIVE_STRING)
		throw std::runtime_error("Runtime Error: '" + strFunc + "' expects the 3 argument to be a 'jstring' (Text to display).");
	std::wstring textToDisplay = {objects[2]->stringValue.begin(), objects[2]->stringValue.end()};

	bool alpha = false;
	if (objects.size() > 3)
	{
		if (objects[3]->getCurrActive() != ACTIVE_BOOLEAN)
			throw std::runtime_error("Runtime Error: '" + strFunc + "' expects the 4 argument to be a 'jboolean' (alpha).");
		alpha = objects[3]->booleanValue;
	}
	this->windows[this->getCurrent(obj1)]->DrawCString({x, y}, textToDisplay, alpha, cycle);
}

JDM_DLL
const void ConsoleClass::drawCStringChar(
	const std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects,
	const bool cycle)
{
	std::string strFunc = "Console.drawColoredStringChar";
	if (cycle) strFunc += "Cycle";

	if (objects.size() < 3 || objects.size() > 5)
		throw std::runtime_error("Runtime Error: '" + strFunc + "' expects 3 to 4 arguments.");

	float x, y;
	this->setFirstArgs(obj1, objects, 2, 5, strFunc, &x, &y);

	if (objects[2]->getCurrActive() != ACTIVE_STRING)
		throw std::runtime_error("Runtime Error: '" + strFunc + "' expects the 3 argument to be a 'jstring' (Text to display).");
	std::wstring textToDisplay = {objects[2]->stringValue.begin(), objects[2]->stringValue.end()};

	short character = JDMConsole::PIXEL::PIXEL_SOLID;
	bool  alpha     = false;
	if (objects.size() > 3)
	{
		auto active = objects[3]->getCurrActive();
		auto errorMess = "Runtime Error: '" + strFunc + "' expects the 4 argument to be a 'jstring' with 1 character or 'jint' (character).";
		if (active == ACTIVE_STRING)
		{
			if (objects[3]->stringValue.size() > 1)
				throw std::runtime_error(errorMess);
			character = static_cast<short>(objects[3]->stringValue[0]);
		}
		else if (active == ACTIVE_INTEGER)
			character = static_cast<short>(objects[3]->integerValue);
		else
			throw std::runtime_error(errorMess);
	}
	if (objects.size() > 4)
	{
		if (objects[4]->getCurrActive() != ACTIVE_BOOLEAN)
			throw std::runtime_error("Runtime Error: '" + strFunc + "' expects the 5 argument to be a 'jboolean' (alpha).");
		alpha = objects[4]->booleanValue;
	}
	this->windows[this->getCurrent(obj1)]->DrawCStringChar({x, y}, textToDisplay, character, alpha, cycle);
}

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

JDM_DLL
const void ConsoleClass::drawBox(
	const std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects,
	const bool cycle)
{
	std::string strFunc = "Console.drawBox";
	if (cycle) strFunc += "Cycle";

	float x, y, w, h;
	this->setFirstArgs(obj1, objects, 4, 7, strFunc, &x, &y, &w, &h);

	short character = JDMConsole::PIXEL::PIXEL_SOLID;
	short color     = JDMConsole::FG_WHITE | JDMConsole::BG_BLACK;
	bool  alpha     = false;

	this->checkLastArguments({objects.begin() + 4, objects.end()}, 5, character, color, alpha, strFunc);
	this->windows[this->getCurrent(obj1)]->DrawBox({static_cast<int>(w), static_cast<int>(h), x, y}, character, color, alpha, cycle);
}



JDM_DLL
const bool ConsoleClass::isHeld(
	const std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (obj1->objectValue->fromMainSource)
		throw std::runtime_error("Runtime Error: The 'Console.isHeld' method is not static.");

	if (objects.size() != 1)
		throw std::runtime_error("Runtime Error: 'Console.isHeld' Expecting one argument.");

	if (objects[0]->getCurrActive() != ACTIVE_INTEGER || objects[0]->integerValue > 255 || objects[0]->integerValue < 0)
		throw std::runtime_error("Runtime Error: 'Console.isHeld' argument must be a 'jint' and must be within 0-255");

	return this->windows[this->getCurrent(obj1)]->GetIsHeld(objects[0]->integerValue);
}

JDM_DLL
const bool ConsoleClass::isReleased(
	const std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (obj1->objectValue->fromMainSource)
		throw std::runtime_error("Runtime Error: The 'Console.isReleased' method is not static.");

	if (objects.size() != 1)
		throw std::runtime_error("Runtime Error: 'Console.isReleased' Expecting one argument.");

	if (objects[0]->getCurrActive() != ACTIVE_INTEGER || objects[0]->integerValue > 255 || objects[0]->integerValue < 0)
		throw std::runtime_error("Runtime Error: 'Console.isReleased' argument must be a 'jint' and must be within 0-255");

	return this->windows[this->getCurrent(obj1)]->GetIsReleased(objects[0]->integerValue);
}

JDM_DLL
const bool ConsoleClass::isPressed(
	const std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (obj1->objectValue->fromMainSource)
		throw std::runtime_error("Runtime Error: The 'Console.isPressed' method is not static.");

	if (objects.size() != 1)
		throw std::runtime_error("Runtime Error: 'Console.isPressed' Expecting one argument.");

	if (objects[0]->getCurrActive() != ACTIVE_INTEGER || objects[0]->integerValue > 255 || objects[0]->integerValue < 0)
		throw std::runtime_error("Runtime Error: 'Console.isPressed' argument must be a 'jint' and must be within 0-255");

	return this->windows[this->getCurrent(obj1)]->GetIsPressed(objects[0]->integerValue);
}