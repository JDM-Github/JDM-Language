#pragma once
#include "Keyboard.hpp"
#include <string>
#include <chrono>
#include <cassert>

#define HeightMax 680
#define WidthMax 1340

class Window {
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
	void Draw(
		const JDMConsole::Pos2F Position,
		short Character = JDMConsole::PIXEL_SOLID,
		short Color     = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK),
		bool  AlphaR    = false);

	constexpr int GetMouseX() { return this->MousePos.x / this->FontWidth; }
	constexpr int GetMouseY() { return this->MousePos.y / this->FontHeight; }
	constexpr JDMConsole::Color getColor(short Index) const {
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

// protected:
	Window(const char *Title, short Width, short Height, short fontWidth = 5, short fontHeight = 5)
	    :
    title(Title),
    ScreenWidth (std::min(Width , static_cast<short>(WidthMax  / fontWidth ))),
    ScreenHeight(std::min(Height, static_cast<short>(HeightMax / fontHeight))), 
    FontWidth(fontWidth), FontHeight(fontHeight)
{
    this->originalConsole   = GetStdHandle(STD_OUTPUT_HANDLE);
    this->hConsole          = GetStdHandle(STD_OUTPUT_HANDLE);
    this->hConsoleI         = GetStdHandle(STD_INPUT_HANDLE );
    this->bufferInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);

    assert(SetConsoleActiveScreenBuffer(this->hConsole));
    MoveWindow(GetConsoleWindow(), 0, 0, 0, 0, true);
    GetConsoleMode      (this->hConsoleI, &this->PrevMode);
    SetConsoleMode      (this->hConsoleI, ENABLE_EXTENDED_FLAGS | (this->PrevMode & ~ENABLE_QUICK_EDIT_MODE));
    SetConsoleWindowSize(this->hConsole, ScreenWidth, ScreenHeight, fontWidth, fontHeight);

    ShowScrollBar(GetConsoleWindow(), SB_VERT, false);
    GetConsoleCursorInfo(this->hConsole, &this->CursorInfo);
    this->CursorInfo.dwSize = 100;
    this->CursorInfo.bVisible = false;
    SetConsoleCursorInfo(this->hConsole, &this->CursorInfo);
    GetConsoleScreenBufferInfoEx(this->hConsole, &bufferInfo);
    this->Screen = new CHAR_INFO[ScreenWidth * ScreenHeight];
}
	~Window();
	virtual bool onUserUpdate() { return true; }
	virtual bool onUserCreate() { return true; }
	constexpr short GetWidth () const { return this->ScreenWidth ; }
	constexpr short GetHeight() const { return this->ScreenHeight; }
	constexpr int GetMouseIndex(int BlockWidth, int Pitch) {
		return (GetMouseY() / BlockWidth) * Pitch + (GetMouseX() / BlockWidth);
	}

private:
	void SetConsoleWindowSize(HANDLE console, short Width, short Height, short FontWidth, short FontHeight);
	bool gameLoop();

};
