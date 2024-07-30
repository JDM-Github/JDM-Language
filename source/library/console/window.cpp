#include "library/console/window.hpp"
#include <cmath>

Window::Window(const char *Title, short Width, short Height, short fontWidth, short fontHeight)
	:
	title(Title),
	ScreenWidth (std::min(Width , static_cast<short>(WidthMax  / fontWidth ))),
	ScreenHeight(std::min(Height, static_cast<short>(HeightMax / fontHeight))), 
	FontWidth(fontWidth), FontHeight(fontHeight)
{
	HWND consoleWindow = GetConsoleWindow();
	SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX);

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

Window::~Window()
{
	Clear();
	WriteConsoleOutputW(this->hConsole, this->Screen, {this->GetWidth(), this->GetHeight()}, {0, 0}, &this->screenBufferCorners);
	SetConsoleWindowSize(this->originalConsole, 120, 50, 6, 10);
	SetConsoleActiveScreenBuffer(this->originalConsole);
	ShowScrollBar(GetConsoleWindow(), SB_VERT, true);
	SetConsoleCursorPosition(this->originalConsole, {0, 0});
	SetConsoleMode(this->hConsoleI, this->PrevMode);

	this->CursorInfo.dwSize = 100;
	this->CursorInfo.bVisible = true;
	SetConsoleCursorInfo(this->hConsole, &this->CursorInfo);
	SetConsoleTextAttribute(this->originalConsole, JDMConsole::FG_WHITE | JDMConsole::BG_BLACK);
	delete[] this->Screen;
}

void Window::SetConsoleWindowSize(HANDLE console, short Width, short Height, short FontWidth, short FontHeight)
{
	this->screenBufferCorners = {0, 0, 1, 1};
	SetConsoleWindowInfo(console, true, &this->screenBufferCorners);
	assert(SetConsoleScreenBufferSize(console, {Width, Height}));
	assert(SetConsoleActiveScreenBuffer(console));

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = FontWidth;
	cfi.dwFontSize.Y = FontHeight;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	std::wcscpy(cfi.FaceName, L"Lucida Console");
	assert(SetCurrentConsoleFontEx(console, false, &cfi));
	this->screenBufferCorners = {0, 0, static_cast<short>(Width - 1), static_cast<short>(Height - 1)};
	assert(SetConsoleWindowInfo(console, true, &this->screenBufferCorners));
}

void Window::Draw(
	const JDMConsole::Pos2F Position,
	short Character, short Color, bool AlphaR)
{
	if (static_cast<int>(Position.X) >= 0 && static_cast<int>(Position.X) < this->GetWidth()
	 && static_cast<int>(Position.Y) >= 0 && static_cast<int>(Position.Y) < this->GetHeight())
	{
		if (AlphaR && Character == JDMConsole::BLANK) return;
		this->Screen[this->GetWidth() * static_cast<int>(Position.Y) + static_cast<int>(Position.X)].Char.UnicodeChar = Character;
		this->Screen[this->GetWidth() * static_cast<int>(Position.Y) + static_cast<int>(Position.X)].Attributes       = Color;
	}
}

void Window::Clear(short Character, short Color)
{
	for (short i = 0; i < this->GetHeight(); i++)
		for (short j = 0; j < this->GetWidth(); j++)
		{
			this->Screen[i * this->GetWidth() + j].Char.UnicodeChar = Character;
			this->Screen[i * this->GetWidth() + j].Attributes       = Color;
		}
}

void Window::Start()
{
	this->timePoint1 = std::chrono::system_clock::now();
	this->timePoint2 = std::chrono::system_clock::now();
}

const void Window::startUpdate(bool autoDelete)
{
	this->timePoint2 = std::chrono::system_clock::now();
	std::chrono::duration<float> elapseTime = this->timePoint2 - this->timePoint1;
	this->timePoint1 = this->timePoint2;
	this->ElapseTime = elapseTime.count();
	if (autoDelete) this->Clear();
}

const bool Window::updateGame()
{
	this->DrawBox({20, 20, static_cast<float>(this->GetMouseX() - 10), static_cast<float>(this->GetMouseY() - 10)},
		JDMConsole::PIXEL_SOLID, JDMConsole::FG_RED, true);
	GetCursorPos(&this->MousePos);
	ScreenToClient(GetConsoleWindow(), &this->MousePos);
	this->ExactMousePos.X = this->MousePos.x / this->FontWidth;
	this->ExactMousePos.Y = this->MousePos.y / this->FontHeight;

	this->keyboard.update();
	if (keyboard.Keys[JDMConsole::Keys::J_ESC].isReleased)
		return false;

	wchar_t updater[255];
	swprintf(updater, 255, L"%s - FPS: %4.2f", this->title.c_str(), 1.f / this->ElapseTime);
	SetConsoleTitleW(updater);
	WriteConsoleOutputW(this->hConsole, this->Screen, {this->GetWidth(), this->GetHeight()}, {0, 0}, &this->screenBufferCorners);
	return true;
}


void Window::DrawString(const JDMConsole::Pos2F Position, const std::wstring &str, const short Color, const bool AlphaR)
{
	short x_adder = 0, y_adder = 0;
	for (short i = 0; i < str.size(); i++)
	{
		if (str[i] == JDMConsole::NEWLINE)
		{
			x_adder = 0;
			y_adder++;
			continue;
		}
		if (AlphaR && str[i] == JDMConsole::BLANK) continue;
		this->Draw({Position.X + x_adder, Position.Y + y_adder}, str[i], Color, AlphaR);
		x_adder++;
	}
}

void Window::DrawACString(const JDMConsole::Pos2F Position, const std::wstring &str, const short Character, const short Color, const bool AlphaR)
{
	short x_adder = 0, y_adder = 0;
	for (short i = 0; i < str.size(); i++)
	{
		if (str[i] == JDMConsole::NEWLINE)
		{
			x_adder = 0;
			y_adder++;
			continue;
		}
		if (AlphaR && str[i] == JDMConsole::BLANK) continue;
		this->Draw({Position.X + x_adder, Position.Y + y_adder}, Character, Color);
		x_adder++;
	}
}

void Window::DrawCString(const JDMConsole::Pos2F Position, const std::wstring &str, const bool AlphaR)
{
	short x_adder = 0, y_adder = 0;
	for (short i = 0; i < str.size(); i++)
	{
		if (str[i] == JDMConsole::NEWLINE)
		{
			x_adder = 0;
			y_adder++;
			continue;
		}
		x_adder++;
		if (AlphaR && str[i] == JDMConsole::BLANK) continue;
		this->Draw({Position.X + x_adder - 1, Position.Y + y_adder}, JDMConsole::PIXEL_SOLID, (this->getColor(str[i]) | JDMConsole::BG_BLACK));
	}
}

void Window::DrawCStringCycle(const JDMConsole::Pos2F Position, const std::wstring &str, const bool AlphaR)
{
	short x_adder = 0, y_adder = 0;
	for (short i = 0; i < str.size(); i++)
	{
		if (str[i] == JDMConsole::NEWLINE)
		{
			x_adder = 0;
			y_adder++;
			continue;
		}
		x_adder++;
		if (AlphaR && str[i] == JDMConsole::BLANK) continue;
		this->DrawCycle({((Position.X < 0) ? GetWidth() - std::abs(Position.X) : Position.X) + x_adder - 1,
						  ((Position.Y < 0) ? GetHeight() - std::abs(Position.Y) : Position.Y) + y_adder},
						  JDMConsole::PIXEL_SOLID, (this->getColor(str[i]) | JDMConsole::BG_BLACK));
	}
}

// void Window::DrawString(const JDMConsole::Pos2F Position, const wchar_t str[], const short Color, const bool AlphaR)
// {
// 	short x_adder = 0, y_adder = 0;
// 	for (short i = 0; str[i]; i++) {
// 		if (str[i] == JDMConsole::NEWLINE) {
// 			x_adder = 0;
// 			y_adder++;
// 			continue;
// 		}
// 		this->Draw({Position.X + x_adder, Position.Y + y_adder}, str[i], Color, AlphaR);
// 		x_adder++;
// 	}
// }

void Window::DrawHorizontal(const JDMConsole::Pos2F Position, const int Width, const short Character, const short color, const bool AlphaR)
{
	for (int i = static_cast<int>(Position.X); i < Width; i++)
		this->Draw({Position.X + i, Position.Y}, Character, color, AlphaR);
}

void Window::DrawVertical(const JDMConsole::Pos2F Position, const int Height, const short Character, const short color, const bool AlphaR)
{
	for (int i = static_cast<int>(Position.Y); i < Height; i++)
		this->Draw({Position.X, Position.Y + i}, Character, color, AlphaR);
}

void Window::DrawLine(const JDMConsole::Pos4F Position, const short Character, const short color, const bool AlphaR)
{
	float xCurrent = Position.W;
	float yCurrent = Position.X;
	float xDistant = (Position.W > Position.Y) ? (Position.W - Position.Y) : (Position.Y - Position.W);
	float yDistant = (Position.X > Position.Z) ? (Position.X - Position.Z) : (Position.Z - Position.X);
	float distance = std::sqrt(xDistant * xDistant + yDistant * yDistant);
	int numberSample = std::max(xDistant, yDistant);

	float samplePX = xDistant / numberSample;
	float samplePY = yDistant / numberSample;
	for (int i = 0; i < numberSample; i++) {
		this->Draw({xCurrent, yCurrent}, Character, color, AlphaR);
		xCurrent += (Position.W > Position.Y) ? -samplePX : samplePX;
		yCurrent += (Position.X > Position.Z) ? -samplePY : samplePY;
	}
}

void Window::DrawTriangle(const JDMConsole::Pos6F Position, const short Character, const short color, const bool AlphaR)
{
	this->DrawLine({Position.U, Position.U, Position.W, Position.X}, Character, color, AlphaR);
	this->DrawLine({Position.W, Position.X, Position.Y, Position.Z}, Character, color, AlphaR);
	this->DrawLine({Position.Y, Position.Z, Position.U, Position.U}, Character, color, AlphaR);
}

void Window::DrawBox(const JDMConsole::SizePosDF SizePosition, const short Character, const short color, const bool AlphaR)
{
	for (int i = 0; i < SizePosition.Height; i++)
		for (int j = 0; j < SizePosition.Width; j++)
			this->Draw({SizePosition.X + j, SizePosition.Y + i}, Character, color, AlphaR);
}

void Window::DrawCycle(const JDMConsole::Pos2F Position, const short Character, const short color, const bool AlphaR)
{
	if (static_cast<int>(Position.X) >= 0 && static_cast<int>(Position.Y) >= 0)
	{
		if (AlphaR && Character == JDMConsole::BLANK) return;
		this->Screen[this->GetWidth() * (static_cast<int>(Position.Y) % this->GetHeight()) + (static_cast<int>(Position.X) % this->GetWidth())].Char.UnicodeChar = Character;
		this->Screen[this->GetWidth() * (static_cast<int>(Position.Y) % this->GetHeight()) + (static_cast<int>(Position.X) % this->GetWidth())].Attributes = color;
	}
}
