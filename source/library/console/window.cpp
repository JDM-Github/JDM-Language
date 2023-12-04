#include "library/console/window.hpp"

// Window::Window(const char *Title, short Width, short Height, short fontWidth, short fontHeight)


Window::~Window() {
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

void Window::SetConsoleWindowSize(HANDLE console, short Width, short Height, short FontWidth, short FontHeight) {
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

void Window::Start() {
    assert(this->onUserCreate());
    this->timePoint1 = std::chrono::system_clock::now();
    this->timePoint2 = std::chrono::system_clock::now();
    while (this->Running) this->Running = this->gameLoop();
}

bool Window::gameLoop() {
    this->timePoint2 = std::chrono::system_clock::now();
    std::chrono::duration<float> elapseTime = this->timePoint2 - this->timePoint1;
    this->timePoint1 = this->timePoint2;
    this->ElapseTime = elapseTime.count();

    if (this->onUserUpdate()) {
        GetCursorPos(&this->MousePos);
        ScreenToClient(GetConsoleWindow(), &this->MousePos);
        this->ExactMousePos.X = this->MousePos.x / this->FontWidth;
        this->ExactMousePos.Y = this->MousePos.y / this->FontHeight;

        this->keyboard.update();
        if (keyboard.Keys[JDMConsole::Keys::J_ESC].isReleased) return false;
        wchar_t updater[255];
        swprintf(updater, 255, L"%s - FPS: %4.2f", this->title.c_str(), 1.f / this->ElapseTime);
        SetConsoleTitleW(updater);
        WriteConsoleOutputW(this->hConsole, this->Screen, {this->GetWidth(), this->GetHeight()}, {0, 0}, &this->screenBufferCorners);
        return true;
    }
    return false;
}

void Window::Clear(short Character, short Color) {
    for (short i = 0; i < this->GetHeight(); i++)
        for (short j = 0; j < this->GetWidth(); j++) {
            this->Screen[i * this->GetWidth() + j].Char.UnicodeChar = Character;
            this->Screen[i * this->GetWidth() + j].Attributes       = Color;
        }
}

void Window::Draw(const JDMConsole::Pos2F Position, short Character, short color, bool AlphaR) {
    if (static_cast<int>(Position.X) >= 0 && static_cast<int>(Position.X) < this->GetWidth()
     && static_cast<int>(Position.Y) >= 0 && static_cast<int>(Position.Y) < this->GetHeight()) {
        if (AlphaR && Character == JDMConsole::BLANK) return;
        this->Screen[this->GetWidth() * static_cast<int>(Position.Y) + static_cast<int>(Position.X)].Char.UnicodeChar = Character;
        this->Screen[this->GetWidth() * static_cast<int>(Position.Y) + static_cast<int>(Position.X)].Attributes       = color;
    }
}
