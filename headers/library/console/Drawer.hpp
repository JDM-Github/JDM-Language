#pragma once

class Drawer
{
public:
	static void DrawBoxHollow(Window *window, const JDMConsole::SizePosDF SizePosition, const int thickness, const short Character = JDMConsole::PIXEL_SOLID, const short Color = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK), const bool AlphaR = false);
	static void DrawBoxHollowWH(Window *window, const JDMConsole::SizePosDF SizePosition, const int thicknessW, const int thicknessH,
				const short Character = JDMConsole::PIXEL_SOLID, const short Color = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK), const bool AlphaR = false);
	static void DrawBoxHollowHV(Window *window, const JDMConsole::SizePosDF SizePosition, const int LEFT, const int RIGHT, const int UP, const int DOWN, 
				const short Character = JDMConsole::PIXEL_SOLID, const short Color = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK), const bool AlphaR = false);
	static void DrawCircle(Window *window, const JDMConsole::Pos2F Position, const int radius,
		const short Character = JDMConsole::PIXEL_SOLID, const short Color = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK), const bool AlphaR = false);
	static void DrawHallowCircle(Window *window, const JDMConsole::Pos2F Position, const int radius, int Thickness,
		const short Character = JDMConsole::PIXEL_SOLID, const short Color = (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK), const bool AlphaR = false);
	static void DrawColoredHallowCircle(Window *window, const JDMConsole::Pos2F Position, const int radius, int Thickness,
		const short Character = JDMConsole::PIXEL_SOLID, const short OutColor =  (JDMConsole::FG_WHITE | JDMConsole::BG_BLACK), const short Color = (JDMConsole::FG_GRAY | JDMConsole::BG_BLACK), const bool AlphaR = false);
};
