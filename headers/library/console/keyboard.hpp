#pragma once
#include <Windows.h>

struct KeyState {
	bool isPressed  = false;
	bool isHeld     = false;
	bool isReleased = false;
};
constexpr bool isKeyHeld    (bool OldState, bool NewState) { return( OldState && NewState); }
constexpr bool isKeyPressed (bool OldState, bool NewState) { return(!OldState && NewState); }
constexpr bool isKeyReleased(bool OldState, bool NewState) { return(OldState && !NewState); }

namespace JDMConsole {

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
        J_MOUSE_LEFT = 1, J_MOUSE_RIGHT = 2, J_BACKSPACE = 8, J_TAB,
        J_ENTER = 13, J_SHIFT = 16, J_CTRL, J_ALT,
        J_CAPSLOCK = 20, J_ESC = 27, J_SPACEBAR = 32, J_PAGEUP,
        J_PAGEDOWN, J_END, J_HOME, J_LEFTKEY,
        J_UPKEY, J_RIGHTKEY, J_DOWNKEY, J_DELETE = 46,
        J_0KEY = 48, J_1KEY, J_2KEY, J_3KEY,
        J_4KEY, J_5KEY, J_6KEY, J_7KEY,
        J_8KEY, J_9KEY, J_AKEY = 65, J_BKEY,
        J_CKEY, J_DKEY, J_EKEY, J_FKEY,
        J_GKEY, J_HKEY, J_IKEY, J_JKEY,
        J_KKEY, J_LKEY, J_MKEY, J_NKEY,
        J_OKEY, J_PKEY, J_QKEY, J_RKEY,
        J_SKEY, J_TKEY, J_UKEY, J_VKEY,
        J_WKEY, J_XKEY, J_YKEY, J_ZKEY,
        J_WIN = 91, J_NOTE = 93, J_F1 = 112, J_F2,
        J_F3, J_F4, J_F5, J_F6,
        J_F7, J_F8, J_F9, J_F10,
        J_F11, J_F12, J_LSHIFT = 160, J_RSHIFT,
        J_LCTRL, J_RCTRL, J_LALT, J_RALT,
        J_SIMICOLON = 186, J_ADD, J_COMMA, J_MINUS,
        J_DOT, J_BSLASH, J_DQOUTE, J_OBRACKET = 219,
        J_FSLASH, J_CBRACKET, J_QOUTE, J_PRTSC = 255,
    };
};

class KeyBoard {
public:
	KeyState Keys      [256];
	bool KeyOldState   [256] = {false};
	bool KeyNewState   [256] = {false};

public:
	KeyBoard() {}
	void update() {
		for (int i = 0; i < 256; i++) {
			KeyNewState[i]     = GetAsyncKeyState(i);
			Keys[i].isHeld     = isKeyHeld(KeyOldState[i], KeyNewState[i]);
			Keys[i].isPressed  = isKeyPressed(KeyOldState[i], KeyNewState[i]);
			Keys[i].isReleased = isKeyReleased(KeyOldState[i], KeyNewState[i]);
			KeyOldState[i] = KeyNewState[i];
		}
	}
	KeyState getState(short index) {
		if (index < 0 || index > 255) return KeyState();
		return Keys[index];
	}
};

