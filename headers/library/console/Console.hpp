#pragma once

class Window;

class JDM_DLL ConsoleClass : public BaseNativeClass
{
public:
	enum ConsoleEnum
	{
		CONSOLE_RUN,
		CONSOLE_UPDATE,
		CONSOLE_START_UPDATE,
		CONSOLE_SET_UPDATE,
		CONSOLE_SET_CREATE,
		CONSOLE_AUTO_CLEAR,
		CONSOLE_CLEAR,

		CONSOLE_GET_MOUSE_X,
		CONSOLE_GET_MOUSE_Y,
		CONSOLE_GET_MOUSE,
		CONSOLE_GET_ELAPSETIME,

		CONSOLE_DRAW,
		CONSOLE_DRAW_STRING,
		CONSOLE_DRAW_STRING_CHAR,
		CONSOLE_DRAW_COLORED_STRING,
		CONSOLE_DRAW_COLORED_STRING_CHAR,
		CONSOLE_DRAW_BOX,
		CONSOLE_DRAW_HORIZONTAL,
		CONSOLE_DRAW_VERTICAL,
		CONSOLE_DRAW_LINE,
		CONSOLE_DRAW_TRIANGLE,

		CONSOLE_DRAW_CYCLE,
		CONSOLE_DRAW_STRING_CYCLE,
		CONSOLE_DRAW_STRING_CHAR_CYCLE,
		CONSOLE_DRAW_COLORED_STRING_CYCLE,
		CONSOLE_DRAW_COLORED_STRING_CHAR_CYCLE,
		CONSOLE_DRAW_BOX_CYCLE,
		CONSOLE_DRAW_HORIZONTAL_CYCLE,
		CONSOLE_DRAW_VERTICAL_CYCLE,
		CONSOLE_DRAW_LINE_CYCLE,
		CONSOLE_DRAW_TRIANGLE_CYCLE,

		CONSOLE_IS_HELD,
		CONSOLE_IS_RELEASED,
		CONSOLE_IS_PRESSED
	};

public:
	JDM_DLL ConsoleClass();
	JDM_DLL ~ConsoleClass();

	JDM_DLL const void init();

	JDM_DLL const void initializeFunction();
	JDM_DLL const void initializeVariable();

	JDM_DLL std::shared_ptr<HigherObject> constructor(std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL std::shared_ptr<HigherObject> manageFunction(int funcType, std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);

private:
	int64_t index = 0;
	std::vector<std::shared_ptr<Window>> windows;
	HigherObject *pixelClass   = nullptr;
	HigherObject *colorClass   = nullptr;
	HigherObject *keyCodeClass = nullptr;

private:
	JDM_DLL int64_t getCurrent(const std::shared_ptr<HigherObject> &obj1);
	JDM_DLL const void setFirstArgs(
		const std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects,
		int first, int last, const std::string &text,
		float *u = nullptr, float *v = nullptr,
		float *w = nullptr, float *x = nullptr,
		float *y = nullptr, float *z = nullptr);

	JDM_DLL const void checkLastArguments(
		const std::vector<std::shared_ptr<HigherObject>> &objects, int start,
		short &character, short &color, bool &alpha, const std::string &text);


	JDM_DLL const void startUpdateConsole(int64_t current, bool clear);
	JDM_DLL const bool updateConsole   (int64_t current);

	JDM_DLL const void runConsole      (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL const void clearConsole    (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);

	JDM_DLL const void drawConsole     (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL const void drawCycle       (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL const void drawString      (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects, const bool cycle = false);
	JDM_DLL const void drawStringChar  (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects, const bool cycle = false);
	JDM_DLL const void drawCString     (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects, const bool cycle = false);
	JDM_DLL const void drawCStringChar (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects, const bool cycle = false);
	JDM_DLL const void drawBox         (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects, const bool cycle = false);

	JDM_DLL const void drawHorizontal  (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects, const bool cycle = false);
	JDM_DLL const void drawVertical    (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects, const bool cycle = false);
	JDM_DLL const void drawLine        (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects, const bool cycle = false);
	JDM_DLL const void drawTriangle    (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects, const bool cycle = false);

	JDM_DLL const long double getElapseTime(const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL const int64_t getMouseX(const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL const int64_t getMouseY(const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);

	JDM_DLL const bool isHeld    (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL const bool isReleased(const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL const bool isPressed (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);

	JDM_DLL const std::vector<std::shared_ptr<HigherObject>> getMouse(const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
};
