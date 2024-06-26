#include "library/console/window.hpp"
    
// Window::Window(const char *Title, short Width, short Height, short fontWidth, short fontHeight)

void Window::Draw(
    const JDMConsole::Pos2F Position,
    short Character, short Color, bool AlphaR) {
    if (static_cast<int>(Position.X) >= 0 && static_cast<int>(Position.X) < this->GetWidth()
     && static_cast<int>(Position.Y) >= 0 && static_cast<int>(Position.Y) < this->GetHeight()) {
        if (AlphaR && Character == JDMConsole::BLANK) return;
        this->Screen[this->GetWidth() * static_cast<int>(Position.Y) + static_cast<int>(Position.X)].Char.UnicodeChar = Character;
        this->Screen[this->GetWidth() * static_cast<int>(Position.Y) + static_cast<int>(Position.X)].Attributes       = Color;
    }
}

