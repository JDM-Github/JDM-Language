#pragma once

class Window;

class JDM_DLL ConsoleClass : public BaseNativeClass
{
public:
	enum ConsoleEnum {
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

		CONSOLE_DRAW_CYCLE,
		CONSOLE_DRAW_STRING_CYCLE,
		CONSOLE_DRAW_STRING_CHAR_CYCLE,
		CONSOLE_DRAW_COLORED_STRING_CYCLE,
		CONSOLE_DRAW_COLORED_STRING_CHAR_CYCLE,
		CONSOLE_DRAW_BOX_CYCLE,

		CONSOLE_IS_HELD,
		CONSOLE_IS_RELEASED,
		CONSOLE_IS_PRESSED
	};

	enum ConsoleEnumVariable {
		FG_RED
	};

	int64_t index = 0;
	std::vector<std::shared_ptr<Window>> windows;

	JDM_DLL ConsoleClass();
	JDM_DLL const void initializeFunction();
	JDM_DLL const void initializeVariable();

	JDM_DLL std::shared_ptr<HigherObject> constructor(
		std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects);

	JDM_DLL std::shared_ptr<HigherObject> manageFunction(
		int funcType, std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects);

	// JDM_DLL std::shared_ptr<HigherObject> manageFunction(
	// 	int varType, std::shared_ptr<HigherObject> &obj1);

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

	JDM_DLL const void runConsole(int64_t current, const std::string& title, short Width = 100,
		short Height = 50, short fontWidth = 5, short fontHeight = 5);

	JDM_DLL const void startUpdateConsole(int64_t current, bool clear);
	JDM_DLL const bool updateConsole   (int64_t current);
	JDM_DLL const void clearConsole    (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);

	JDM_DLL const void drawConsole     (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL const void drawCycle       (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL const void drawString      (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects, const bool cycle = false);
	JDM_DLL const void drawStringChar  (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects, const bool cycle = false);
	JDM_DLL const void drawCString     (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects, const bool cycle = false);
	JDM_DLL const void drawCStringChar (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects, const bool cycle = false);
	JDM_DLL const void drawBox         (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects, const bool cycle = false);

	// JDM_DLL const void drawCStringCycle(const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	// JDM_DLL const void drawString      (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	// JDM_DLL const void drawACString    (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	// JDM_DLL const void drawHorizontal  (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	// JDM_DLL const void drawVertical    (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	// JDM_DLL const void drawLine        (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	// JDM_DLL const void drawTriangle    (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);


	JDM_DLL const long double getElapseTime(const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL const int64_t getMouseX(const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL const int64_t getMouseY(const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);

	JDM_DLL const bool isHeld    (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL const bool isReleased(const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL const bool isPressed (const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);

	JDM_DLL const std::vector<std::shared_ptr<HigherObject>> getMouse(const std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);

	// const void drawConsole     (int64_t current, float x, float y, short character, short color, bool alpha);
	// const void drawCycle       (int64_t current, float x, float y, short character, short color, bool alpha);

	// const void drawCString     (int64_t current, float x, float y, const std::string &character, bool alpha);
	// const void drawCStringCycle(int64_t current, float x, float y, const std::string &character, bool alpha);

	// const void drawString      (int64_t current, float x, float y, const std::string &character, short color, bool alpha);
	// const void drawACString    (int64_t current, float x, float y, const std::string &character, short character, short color, bool alpha);

	// const void drawHorizontal  (int64_t current, float x, float y, short character, short color, bool alpha);
	// const void drawVertical    (int64_t current, float x, float y, short character, short color, bool alpha);
	// const void drawLine        (int64_t current, float w, float x, float y, float z, short character, short color, bool alpha);
	// const void drawTriangle    (int64_t current, float u, float v, float w, float x, float y, float z, short character, short color, bool alpha);
	// const void drawBox         (int64_t current, float x, float y, float w, float h, short character, short color, bool alpha);

	// void Draw(
	// 	const JDMConsole::Pos2F Position,
	// 	short Character = JDMConsole::PIXEL_SOLID,
	// 	short Color     = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK),
	// 	bool  AlphaR    = false);
	// void DrawCycle       (const JDMConsole::Pos2F Position, const short Character = JDMConsole::PIXEL_SOLID, const short Color = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK), const bool AlphaR = false);
	// void DrawCStringCycle(const JDMConsole::Pos2F Position, const std::wstring &str, const bool AlphaR = false);
	// void DrawString      (const JDMConsole::Pos2F Position, const std::wstring &str, const short Color, const bool AlphaR = false);
	// void DrawACString    (const JDMConsole::Pos2F Position, const std::wstring &str, const short Character, const short Color);
	// void DrawCString     (const JDMConsole::Pos2F Position, const std::wstring &str, const bool AlphaR = false);
	// void DrawHorizontal  (const JDMConsole::Pos2F Position, const int Width, const short Character = JDMConsole::PIXEL_SOLID, const short Color = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK), const bool AlphaR = false);
	// void DrawVertical    (const JDMConsole::Pos2F Position, const int Height, const short Character = JDMConsole::PIXEL_SOLID, const short Color = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK), const bool AlphaR = false);
	// void DrawLine        (const JDMConsole::Pos4F Position, const short Character = JDMConsole::PIXEL_SOLID, const short Color = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK), const bool AlphaR = false);
	// void DrawTriangle    (const JDMConsole::Pos6F Position, const short Character = JDMConsole::PIXEL_SOLID, const short Color = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK), const bool AlphaR = false);
	// void DrawBox         (const JDMConsole::SizePosDF SizePosition, const short Character = JDMConsole::PIXEL_SOLID, const short Color = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK), const bool AlphaR = false);
};
