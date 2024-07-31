#include "library/console/window.hpp"

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
	GetCursorPos(&this->MousePos);
	ScreenToClient(GetConsoleWindow(), &this->MousePos);
	this->ExactMousePos.X = this->MousePos.x / this->FontWidth;
	this->ExactMousePos.Y = this->MousePos.y / this->FontHeight;

	this->keyboard.update();
	if (this->keyboard.Keys[JDMConsole::Keys::ESC].isReleased)
		return false;

	wchar_t updater[255];
	swprintf(updater, 255, L"%s - FPS: %4.2f", this->title.c_str(), 1.f / this->ElapseTime);
	SetConsoleTitleW(updater);
	WriteConsoleOutputW(this->hConsole, this->Screen, {this->GetWidth(), this->GetHeight()}, {0, 0}, &this->screenBufferCorners);
	return true;
}

void Window::Draw(
	const JDMConsole::Pos2F Position,
	const short Character,
	const short Color,
	const bool AlphaR)
{
	if (static_cast<int>(Position.X) >= 0 && static_cast<int>(Position.X) < this->GetWidth()
	 && static_cast<int>(Position.Y) >= 0 && static_cast<int>(Position.Y) < this->GetHeight())
	{
		if (AlphaR && Character == JDMConsole::BLANK) return;
		this->Screen[this->GetWidth() * static_cast<int>(Position.Y) + static_cast<int>(Position.X)].Char.UnicodeChar = Character;
		this->Screen[this->GetWidth() * static_cast<int>(Position.Y) + static_cast<int>(Position.X)].Attributes       = Color;
	}
}

void Window::DrawCycle(
	const JDMConsole::Pos2F Position,
	const short Character,
	const short color,
	const bool AlphaR)
{
	if (AlphaR && Character == JDMConsole::BLANK) return;

	int posX = static_cast<int>(Position.X);
	int posY = static_cast<int>(Position.Y);
	if (posX < 0) posX = this->GetWidth()  - (std::abs(posX) % GetWidth());
	if (posY < 0) posY = this->GetHeight() - (std::abs(posY) % GetHeight());

	this->Screen[this->GetWidth() * (posY % this->GetHeight()) + (posX % this->GetWidth())].Char.UnicodeChar = Character;
	this->Screen[this->GetWidth() * (posY % this->GetHeight()) + (posX % this->GetWidth())].Attributes = color;
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


void Window::DrawString(
	const JDMConsole::Pos2F Position,
	const std::wstring &str,
	const short Color,
	const bool AlphaR,
	const bool cycle)
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
		if (cycle) this->DrawCycle({Position.X + x_adder, Position.Y + y_adder}, str[i], Color, AlphaR);
		else this->Draw({Position.X + x_adder, Position.Y + y_adder}, str[i], Color, AlphaR);
		x_adder++;
	}
}

void Window::DrawStringChar(
	const JDMConsole::Pos2F Position,
	const std::wstring &str,
	const short Character,
	const short Color,
	const bool AlphaR,
	const bool cycle)
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
		if (cycle) this->DrawCycle({Position.X + x_adder, Position.Y + y_adder}, Character, Color);
		else this->Draw({Position.X + x_adder, Position.Y + y_adder}, Character, Color);
		x_adder++;
	}
}

void Window::DrawCString(
	const JDMConsole::Pos2F Position,
	const std::wstring &str,
	const bool AlphaR,
	const bool cycle)
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
		if (cycle) this->DrawCycle({Position.X + x_adder, Position.Y + y_adder}, JDMConsole::PIXEL_SOLID, (this->getColor(str[i]) | JDMConsole::BG_BLACK));
		else this->Draw({Position.X + x_adder, Position.Y + y_adder}, JDMConsole::PIXEL_SOLID, (this->getColor(str[i]) | JDMConsole::BG_BLACK));
		x_adder++;
	}
}

void Window::DrawCStringChar(
	const JDMConsole::Pos2F Position,
	const std::wstring &str,
	const short Character,
	const bool AlphaR,
	const bool cycle)
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
		if (cycle) this->DrawCycle({Position.X + x_adder, Position.Y + y_adder}, Character, (this->getColor(str[i]) | JDMConsole::BG_BLACK));
		else this->Draw({Position.X + x_adder, Position.Y + y_adder}, Character, (this->getColor(str[i]) | JDMConsole::BG_BLACK));
		x_adder++;
	}
}


void Window::DrawHorizontal(
	const JDMConsole::Pos2F Position,
	const int Width,
	const short Character,
	const short color,
	const bool AlphaR,
	const bool cycle)
{
	for (int i = static_cast<int>(Position.X); i < Width; i++)
		if (cycle) this->DrawCycle({Position.X + i, Position.Y}, Character, color, AlphaR);
		else this->Draw({Position.X + i, Position.Y}, Character, color, AlphaR);
}

void Window::DrawVertical(
	const JDMConsole::Pos2F Position,
	const int Height,
	const short Character,
	const short color,
	const bool AlphaR,
	const bool cycle)
{
	for (int i = static_cast<int>(Position.Y); i < Height; i++)
		if (cycle) this->DrawCycle({Position.X, Position.Y + i}, Character, color, AlphaR);
		else this->Draw({Position.X, Position.Y + i}, Character, color, AlphaR);
}

void Window::DrawLine(
	const JDMConsole::Pos4F Position,
	const short Character,
	const short color,
	const bool AlphaR,
	const bool cycle)
{
	float xCurrent = Position.W;
	float yCurrent = Position.X;
	float xDistant = (Position.W > Position.Y) ? (Position.W - Position.Y) : (Position.Y - Position.W);
	float yDistant = (Position.X > Position.Z) ? (Position.X - Position.Z) : (Position.Z - Position.X);
	float distance = std::sqrt(xDistant * xDistant + yDistant * yDistant);

	int numberSamp = std::max(xDistant, yDistant);
	float samplePX = xDistant / numberSamp;
	float samplePY = yDistant / numberSamp;
	for (int i = 0; i < numberSamp; i++)
	{
		if (cycle) this->DrawCycle({xCurrent, yCurrent}, Character, color, AlphaR);
		else this->Draw({xCurrent, yCurrent}, Character, color, AlphaR);
		xCurrent += (Position.W > Position.Y) ? -samplePX : samplePX;
		yCurrent += (Position.X > Position.Z) ? -samplePY : samplePY;
	}
}

void Window::DrawTriangle(
	const JDMConsole::Pos6F Position,
	const short Character,
	const short color,
	const bool AlphaR,
	const bool cycle)
{
	this->DrawLine({Position.U, Position.U, Position.W, Position.X}, Character, color, AlphaR, cycle);
	this->DrawLine({Position.W, Position.X, Position.Y, Position.Z}, Character, color, AlphaR, cycle);
	this->DrawLine({Position.Y, Position.Z, Position.U, Position.U}, Character, color, AlphaR, cycle);
}

void Window::DrawBox(
	const JDMConsole::SizePosDF SizePosition,
	const short Character,
	const short color,
	const bool AlphaR,
	const bool cycle)
{
	for (int i = 0; i < SizePosition.Height; i++)
		for (int j = 0; j < SizePosition.Width; j++)
		{
			if (cycle) this->DrawCycle({SizePosition.X + j, SizePosition.Y + i}, Character, color, AlphaR);
			else this->Draw({SizePosition.X + j, SizePosition.Y + i}, Character, color, AlphaR);
		}
}
