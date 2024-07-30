#include "library/console/window.hpp"
#include "library/console/Drawer.hpp"

void Drawer::DrawBoxHollow(Window *window, const JDMConsole::SizePosDF SizePosition, const int thickness,
			const short Character, const short Color, const bool AlphaR)
{
	for (int px = SizePosition.X; px < SizePosition.X + SizePosition.Width; px++)
		for (int py = SizePosition.Y; py < SizePosition.Y + thickness; py++)
			window->Draw({static_cast<float>(px), static_cast<float>(py)}, Character, Color, AlphaR);
	for (int px = SizePosition.X; px < SizePosition.X + SizePosition.Width; px++)
		for (int py = SizePosition.Y + SizePosition.Height - 1; py > SizePosition.Y + SizePosition.Height - 1 - thickness; py--)
			window->Draw({static_cast<float>(px), static_cast<float>(py)}, Character, Color, AlphaR);
	for (int py = SizePosition.Y; py < SizePosition.Y + SizePosition.Height; py++)
		for (int px = SizePosition.X; px < SizePosition.X + thickness; px++)
			window->Draw({static_cast<float>(px), static_cast<float>(py)}, Character, Color, AlphaR);
	for (int py = SizePosition.Y; py < SizePosition.Y + SizePosition.Height; py++)
		for (int px = SizePosition.X + SizePosition.Width - 1; px > SizePosition.X + SizePosition.Width - 1 - thickness; px--)
			window->Draw({static_cast<float>(px), static_cast<float>(py)}, Character, Color, AlphaR);
}
void Drawer::DrawBoxHollowWH(Window *window, const JDMConsole::SizePosDF SizePosition, const int thicknessW, const int thicknessH,
			const short Character, const short Color, const bool AlphaR)
{
	for (int px = SizePosition.X; px < SizePosition.X + SizePosition.Width; px++)
		for (int py = SizePosition.Y; py < SizePosition.Y + thicknessH; py++)
			window->Draw({static_cast<float>(px), static_cast<float>(py)}, Character, Color, AlphaR);
	for (int px = SizePosition.X; px < SizePosition.X + SizePosition.Width; px++)
		for (int py = SizePosition.Y + SizePosition.Height - 1; py > SizePosition.Y + SizePosition.Height - 1 - thicknessH; py--)
			window->Draw({static_cast<float>(px), static_cast<float>(py)}, Character, Color, AlphaR);
	for (int py = SizePosition.Y; py < SizePosition.Y + SizePosition.Height; py++)
		for (int px = SizePosition.X; px < SizePosition.X + thicknessW; px++)
			window->Draw({static_cast<float>(px), static_cast<float>(py)}, Character, Color, AlphaR);
	for (int py = SizePosition.Y; py < SizePosition.Y + SizePosition.Height; py++)
		for (int px = SizePosition.X + SizePosition.Width - 1; px > SizePosition.X + SizePosition.Width - 1 - thicknessW; px--)
			window->Draw({static_cast<float>(px), static_cast<float>(py)}, Character, Color, AlphaR);
}
void Drawer::DrawBoxHollowHV(Window *window, const JDMConsole::SizePosDF SizePosition, const int LEFT, const int RIGHT, const int UP, const int DOWN, 
			const short Character, const short Color, const bool AlphaR)
{
	for (int px = SizePosition.X; px < SizePosition.X + SizePosition.Width; px++)
		for (int py = SizePosition.Y; py < SizePosition.Y + UP; py++)
			window->Draw({static_cast<float>(px), static_cast<float>(py)}, Character, Color, AlphaR);
	for (int px = SizePosition.X; px < SizePosition.X + SizePosition.Width; px++)
		for (int py = SizePosition.Y + SizePosition.Height - 1; py > SizePosition.Y + SizePosition.Height - 1 - DOWN; py--)
			window->Draw({static_cast<float>(px), static_cast<float>(py)}, Character, Color, AlphaR);
	for (int py = SizePosition.Y; py < SizePosition.Y + SizePosition.Height; py++)
		for (int px = SizePosition.X; px < SizePosition.X + RIGHT; px++)
			window->Draw({static_cast<float>(px), static_cast<float>(py)}, Character, Color, AlphaR);
	for (int py = SizePosition.Y; py < SizePosition.Y + SizePosition.Height; py++)
		for (int px = SizePosition.X + SizePosition.Width - 1; px > SizePosition.X + SizePosition.Width - 1 - LEFT; px--)
			window->Draw({static_cast<float>(px), static_cast<float>(py)}, Character, Color, AlphaR);
}
void Drawer::DrawCircle(Window *window, const JDMConsole::Pos2F Position, const int radius,
	const short Character, const short Color, const bool AlphaR)
{
	for (int h = 0; h < radius * 2; h++)
		for (int w = 0; w < radius * 2; w++)
		{
			int dx = radius - w;
			int dy = radius - h;
			if (!((dx * dx) + (dy * dy) >= (radius * radius)))
				window->Draw({Position.X + radius + dx - 1, Position.Y + radius + dy - 1}, Character, Color, AlphaR);
		}
}
void Drawer::DrawHallowCircle(Window *window, const JDMConsole::Pos2F Position, const int radius, int Thickness,
	const short Character, const short Color, const bool AlphaR)
{
	for (int h = 0; h < radius * 2; h++)
		for (int w = 0; w < radius * 2; w++)
		{
			int dx = radius - w;
			int dy = radius - h;
			if (!((dx * dx) + (dy * dy) >= (radius * radius)) &&
				!((dx * dx) + (dy * dy) <= ((radius - Thickness) * (radius - Thickness))))
				window->Draw({Position.X + radius + dx - 1, Position.Y + radius + dy - 1}, Character, Color, AlphaR);
		}
}
void Drawer::DrawColoredHallowCircle(Window *window, const JDMConsole::Pos2F Position, const int radius, int Thickness,
	const short Character, const short OutColor, const short Color, const bool AlphaR)
{
	for (int h = 0; h < radius * 2; h++)
		for (int w = 0; w < radius * 2; w++)
		{
			int dx = radius - w;
			int dy = radius - h;
			if (!((dx * dx) + (dy * dy) >= (radius * radius)))
				window->Draw({Position.X + radius + dx - 1, Position.Y + radius + dy - 1}, Character, Color, AlphaR);
			if (!((dx * dx) + (dy * dy) >= (radius * radius)) &&
				!((dx * dx) + (dy * dy) <= ((radius - Thickness) * (radius - Thickness))))
				window->Draw({Position.X + radius + dx - 1, Position.Y + radius + dy - 1}, Character, OutColor, AlphaR);
		}
}