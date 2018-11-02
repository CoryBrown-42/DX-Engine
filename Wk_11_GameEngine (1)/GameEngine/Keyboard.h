#pragma once

#include <Windows.h>

#include "InputDevice.h"

#define VK_OEM_SEMICOLON VK_OEM_1//For US Standard Keyboards
#define VK_ALT VK_MENU//Who calls alt the menu key...?

/*
A Keyboard object represents a physical keyboard attached to the computer
*/
class Keyboard : public InputDevice
{
public:
	Keyboard();
	void onFocus(const HWND &windowHandle) override;

	//Sets a key as being down
	inline void OnKeyDown(WPARAM key)
	{
		curKeys[key] = true;
		pressed = true;
	}

	//Sets a key as not being down when it is released
	inline void OnKeyUp(WPARAM key) 
	{
		curKeys[key] = false;
	}

	//Return whether a button has just been pressed
	inline bool ButtonPressed(WORD button) const
	{
		return curKeys[button] && !prevKeys[button];
	}

	//Return whether a button has just been released
	inline bool ButtonReleased(WORD button) const
	{
		return !curKeys[button] && prevKeys[button];
	}

	//Return whether a button is currently down
	inline bool ButtonDown(WORD button) const
	{
		return curKeys[button];
	} 

	//Reset at the end of the frame
	inline void EndFrame()
	{
		std::copy(std::begin(curKeys), std::end(curKeys), prevKeys);
		pressed = false;
	}

	//Return whether any key has been pressed at all
	//This could be to optimize input code or in "press any key to continue" situations...
	inline bool Pressed() const
	{
		return pressed;
	}

protected:
	bool prevKeys[256], curKeys[256], pressed;
};