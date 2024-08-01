#pragma once

namespace JDMConsole
{
	struct Vector2F { float X, Y; };
	struct Vector3F { float X, Y, Z; };
	struct Vector2D { int X, Y; };
	struct Vector3D { int X, Y, Z; };

	struct Size2D { int Width, Height; };
	struct Size2F { float Width, Height; };
	struct Pos2D { int X, Y; };
	struct Pos2F { float X, Y; };
	struct Pos4D { int W, X, Y, Z; };
	struct Pos4F { float W, X, Y, Z; };
	struct Pos6D { int U, V, W, X, Y, Z; };
	struct Pos6F { float U, V, W, X, Y, Z; };

	struct SizePosD { int Width, Height, X, Y; };
	struct SizePosF { float Width, Height, X, Y; };
	struct SizePosDF { int Width, Height; float X, Y; };
	struct SizePosFD { float Width, Height; int X, Y; };

	struct PixelRGB { int R, G, B; };

	enum PIXEL
	{
		NEWLINE     = 0xA,
		BLANK       = 0x20,
		PIXEL_SOLID = 0x2588,
		S0 = 0x20,
		S1 = 0x2588,
		S2 = 0x2593,
		S3 = 0x2592,
		S4 = 0x2591,
	};

	enum Color {
		FG_BLACK      = 0x0000, BG_BLACK      = 0x0000,
		FG_BLUE       = 0x0001, BG_BLUE       = 0x0010,
		FG_GREEN      = 0x0002, BG_GREEN      = 0x0020,
		FG_CYAN       = 0x0003, BG_CYAN       = 0x0030,
		FG_RED        = 0x0004, BG_RED        = 0x0040,
		FG_MAGENTA    = 0x0005, BG_MAGENTA    = 0x0050,
		FG_YELLOW     = 0x0006, BG_YELLOW     = 0x0060,
		FG_GRAY       = 0x0007, BG_GRAY       = 0x0070,
		FG_DARK_GRAY  = 0x0008, BG_DARK_GRAY  = 0x0080,
		FG_DARK_BLUE  = 0x0009, BG_DARK_BLUE  = 0x0090,
		FG_DARK_GREEN = 0x000A, BG_DARK_GREEN = 0x00A0,
		FG_DARK_CYAN  = 0x000B, BG_DARK_CYAN  = 0x00B0,
		FG_DARK_RED   = 0x000C, BG_DARK_RED   = 0x00C0,
		FG_SKIN       = 0x000D, BG_SKIN       = 0x00D0,
		FG_DARK_SKIN  = 0x000E, BG_DARK_SKIN  = 0x00E0,
		FG_WHITE      = 0x000F, BG_WHITE      = 0x00F0
	};

	enum Keys {
		MOUSE_LEFT = 1, MOUSE_RIGHT = 2, BACKSPACE = 8, TAB,
		ENTER = 13, SHIFT = 16, CTRL, ALT,
		CAPSLOCK = 20, ESC = 27, SPACEBAR = 32, PAGEUP,
		PAGEDOWN, END, HOME, LEFTKEY,
		UPKEY, RIGHTKEY, DOWNKEY, DELETEKEY = 46,
		N0KEY = 48, N1KEY, N2KEY, N3KEY,
		N4KEY, N5KEY, N6KEY, N7KEY,
		N8KEY, N9KEY, AKEY = 65, BKEY,
		CKEY, DKEY, EKEY, FKEY,
		GKEY, HKEY, IKEY, JKEY,
		KKEY, LKEY, MKEY, NKEY,
		OKEY, PKEY, QKEY, RKEY,
		SKEY, TKEY, UKEY, VKEY,
		WKEY, XKEY, YKEY, ZKEY,
		WIN = 91, NOTE = 93, F1 = 112, F2,
		F3, F4, F5, F6,
		F7, F8, F9, F10,
		F11, F12, LSHIFT = 160, RSHIFT,
		LCTRL, RCTRL, LALT, RALT,
		SIMICOLONKEY = 186, ADD, COMMA, MINUS,
		DOT, BSLASH, DQOUTEKEY, OBRACKET = 219,
		FSLASH, CBRACKET, QOUTEKEY, PRTSC = 255,
	};
};
