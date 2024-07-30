#pragma once
#include "BaseClasses.hpp"

class Window;

class ConsoleClass : public BaseNativeClass
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

		CONSOLE_DRAW,
		CONSOLE_DRAW_BOX
	};
	int64_t index = 0;
	std::vector<std::shared_ptr<Window>> windows;

	ConsoleClass();
	std::shared_ptr<HigherObject> constructor(
		std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects);

	std::shared_ptr<HigherObject> manageFunction(
		int funcType, std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects);

	JDM_DLL int64_t getCurrent(const std::shared_ptr<HigherObject> &obj1);

	const void runConsole(int64_t current, const std::string& title, short Width = 100,
		short Height = 50, short fontWidth = 5, short fontHeight = 5);

	const void startUpdateConsole(int64_t current, bool clear);
	const bool updateConsole(int64_t current);
	const void clearConsole(std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	const void drawConsole (std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	const void drawBox     (std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);

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
