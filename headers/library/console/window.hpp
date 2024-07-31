#pragma once
#include "JDMConsole.hpp"
#include "KeyBoard.hpp"
#include <cmath>
#include <string>
#include <chrono>
#include <cassert>

#define HeightMax 680
#define WidthMax 1340

class Window
{
private:
	short ScreenWidth, ScreenHeight;
	bool Running = true;
	std::string title;

	HANDLE originalConsole;
	HANDLE hConsole;
	HANDLE hConsoleI;
	SMALL_RECT screenBufferCorners;
	CONSOLE_SCREEN_BUFFER_INFOEX bufferInfo;
	short FontWidth, FontHeight;
	DWORD PrevMode;
	CONSOLE_CURSOR_INFO CursorInfo;

	std::chrono::_V2::system_clock::time_point timePoint1;
	std::chrono::_V2::system_clock::time_point timePoint2;

public:
	POINT MousePos;
	KeyBoard keyboard = KeyBoard();
	JDMConsole::Pos2D ExactMousePos = { 0, 0 };
	CHAR_INFO *Screen;
	float ElapseTime;

public:
	void Start();
	const void startUpdate(bool autoDelete = false);
	const bool updateGame();

	void Draw(
		const JDMConsole::Pos2F Position,
		const short Character = JDMConsole::PIXEL_SOLID,
		const short Color     = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK),
		const bool  AlphaR    = false);

	void DrawCycle(
		const JDMConsole::Pos2F Position,
		const short Character = JDMConsole::PIXEL_SOLID,
		const short Color     = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK),
		const bool  AlphaR    = false);

	void DrawString( 
		const JDMConsole::Pos2F Position,
		const std::wstring &str,
		const short Color,
		const bool  AlphaR = false,
		const bool  cycle  = false);

	void DrawStringChar(
		const JDMConsole::Pos2F Position,
		const std::wstring &str,
		const short Character = JDMConsole::PIXEL_SOLID,
		const short Color     = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK),
		const bool  AlphaR    = false,
		const bool  cycle     = false);

	void DrawCString(
		const JDMConsole::Pos2F Position,
		const std::wstring &str,
		const bool AlphaR = false,
		const bool cycle  = false);

	void DrawCStringChar(
		const JDMConsole::Pos2F Position,
		const std::wstring &str,
		const short Character = JDMConsole::PIXEL_SOLID,
		const bool  AlphaR    = false,
		const bool  cycle     = false);

	void DrawHorizontal(
		const JDMConsole::Pos2F Position,
		const int Width,
		const short Character = JDMConsole::PIXEL_SOLID,
		const short Color     = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK),
		const bool  AlphaR    = false,
		const bool  cycle     = false);

	void DrawVertical(
		const JDMConsole::Pos2F Position,
		const int Height,
		const short Character = JDMConsole::PIXEL_SOLID,
		const short Color     = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK),
		const bool  AlphaR    = false,
		const bool  cycle     = false);

	void DrawLine(
		const JDMConsole::Pos4F Position,
		const short Character = JDMConsole::PIXEL_SOLID,
		const short Color     = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK),
		const bool  AlphaR    = false,
		const bool  cycle     = false);

	void DrawTriangle(
		const JDMConsole::Pos6F Position,
		const short Character = JDMConsole::PIXEL_SOLID,
		const short Color     = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK),
		const bool  AlphaR    = false,
		const bool  cycle     = false);

	void DrawBox(const JDMConsole::SizePosDF SizePosition,
		const short Character = JDMConsole::PIXEL_SOLID,
		const short Color     = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK),
		const bool  AlphaR    = false,
		const bool  cycle     = false);

	constexpr int64_t GetMouseX() { return this->MousePos.x / this->FontWidth; }
	constexpr int64_t GetMouseY() { return this->MousePos.y / this->FontHeight; }
	constexpr long double GetElapseTime() { return static_cast<long double>(this->ElapseTime); }

	constexpr bool GetIsHeld    (int64_t keys) { return this->keyboard.Keys[keys].isHeld; }
	constexpr bool GetIsReleased(int64_t keys) { return this->keyboard.Keys[keys].isReleased; }
	constexpr bool GetIsPressed (int64_t keys) { return this->keyboard.Keys[keys].isPressed; }

	constexpr JDMConsole::Color getColor(short Index) const
	{
		switch (Index) {
			case L'0': return JDMConsole::FG_BLACK;
			case L'1': return JDMConsole::FG_BLUE;
			case L'2': return JDMConsole::FG_GREEN;
			case L'3': return JDMConsole::FG_CYAN;
			case L'4': return JDMConsole::FG_RED;
			case L'5': return JDMConsole::FG_MAGENTA;
			case L'6': return JDMConsole::FG_YELLOW;
			case L'7': return JDMConsole::FG_GRAY;
			case L'8': return JDMConsole::FG_DARK_GRAY;
			case L'9': return JDMConsole::FG_DARK_BLUE;
			case L'A': return JDMConsole::FG_DARK_GREEN;
			case L'B': return JDMConsole::FG_DARK_CYAN;
			case L'C': return JDMConsole::FG_DARK_RED;
			case L'D': return JDMConsole::FG_SKIN;
			case L'E': return JDMConsole::FG_DARK_SKIN;
			default  : return JDMConsole::FG_WHITE;
		}
	}
	void Clear(short Character = JDMConsole::BLANK, short Color = (JDMConsole::FG_BLACK | JDMConsole::BG_BLACK));

	Window(const char *Title, short Width, short Height, short fontWidth = 5, short fontHeight = 5);
	~Window();

	virtual bool onUserUpdate() { this->Clear(); return true; }
	virtual bool onUserCreate() { return true; }
	constexpr short GetWidth () const { return this->ScreenWidth ; }
	constexpr short GetHeight() const { return this->ScreenHeight; }
	constexpr int GetMouseIndex(int BlockWidth, int Pitch)
	{
		return (GetMouseY() / BlockWidth) * Pitch + (GetMouseX() / BlockWidth);
	}

private:
	void SetConsoleWindowSize(HANDLE console, short Width, short Height, short FontWidth, short FontHeight);
};
