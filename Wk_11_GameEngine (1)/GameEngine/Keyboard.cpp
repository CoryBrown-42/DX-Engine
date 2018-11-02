#include <algorithm>

#include "Keyboard.h"
#include "Utility.h"

//Constructor
Keyboard::Keyboard()
	: pressed(false)
{
	std::fill_n(prevKeys, 256, false);
	std::fill_n(curKeys, 256, false);

	byte testKeyboard[256] = { 0 };
	if (GetKeyboardState(testKeyboard))
		connected = true;
} 

//Reset after the window has been lost and refocused
#pragma warning (disable: 4100) //Unused Parameter windowHandle we might need later
void Keyboard::onFocus(const HWND &windowHandle)
{ 
	if (connected)
	{
		std::fill_n(prevKeys, 256, false);
		std::fill_n(curKeys, 256, false);
	}
}
#pragma warning (default: 4100)