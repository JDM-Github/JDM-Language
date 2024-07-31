#include "library/console/Keyboard.hpp"

const void KeyBoard::update()
{
	for (int i = 0; i < 256; i++)
	{
		KeyNewState[i]     = GetAsyncKeyState(i);
		Keys[i].isHeld     = this->isKeyHeld(KeyOldState[i], KeyNewState[i]);
		Keys[i].isPressed  = this->isKeyPressed(KeyOldState[i], KeyNewState[i]);
		Keys[i].isReleased = this->isKeyReleased(KeyOldState[i], KeyNewState[i]);
		KeyOldState[i] = KeyNewState[i];
	}
}

KeyState KeyBoard::getState(short index)
{
	if (index < 0 || index > 255) return KeyState();
	return Keys[index];
}