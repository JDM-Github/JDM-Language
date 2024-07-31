#pragma once
#include <Windows.h>

struct KeyState
{
	bool isPressed  = false;
	bool isHeld     = false;
	bool isReleased = false;
};

class KeyBoard
{
public:
	KeyState Keys      [256];
	bool KeyOldState   [256] = {false};
	bool KeyNewState   [256] = {false};

public:
	KeyBoard() {}
	const void update();
	KeyState getState(short index);

private:
    constexpr bool isKeyHeld    (bool OldState, bool NewState) { return( OldState && NewState); }
    constexpr bool isKeyPressed (bool OldState, bool NewState) { return(!OldState && NewState); }
    constexpr bool isKeyReleased(bool OldState, bool NewState) { return(OldState && !NewState); }
};

