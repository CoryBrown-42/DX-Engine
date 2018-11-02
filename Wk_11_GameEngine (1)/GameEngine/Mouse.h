#pragma once

#include <algorithm>
#include <Windows.h>
#include <WindowsX.h> 

#include "InputDevice.h" 
#include "Utility.h" 

//For using WM_INPUT high-res mouse data
#ifndef HID_USAGE_PAGE_GENERIC
	#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
	#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif
#define RAWHEADSIZE sizeof(RAWINPUTHEADER)

//For easily specifying a particular button
enum class MouseButton
{
	LBUTTON,
	RBUTTON,
	MBUTTON,
	BUTTON4,
	BUTTON5 
};
 
/*
A Mouse object represents the current state of the mouse and cursor
*/
class Mouse : public InputDevice
{
public:
	Mouse(float snstvty = .1f);
	void InitMouseCallback(const HWND &windowHandle);
	void MouseCallback(LPARAM lParam);
	void ClipMouse(const HWND &windowHandle);
	void LockMouse(bool lock, const HWND &windowHandle);
	void EndFrame();
	void OnResize(int screenWidth, int screenHeight, HWND handle);
	void OnMouseMove(LPARAM lParam);

	//Re-lock the mouse if the window goes out of focus and back into focus
	void onFocus(const HWND &windowHandle) override
	{
		if (connected && locked)
			ClipMouse(windowHandle);
	}

	//return whether a button has just been clicked
	inline bool ButtonPressed(MouseButton button) const
	{
		return buttons[(int)button] && !prevButtons[(int)button];
	}

	//return whether a button is currently down
	inline bool ButtonDown(MouseButton button) const
	{ 
		return buttons[(int)button];
	}

	//Return whether a button has just been released
	inline bool ButtonReleased(MouseButton button) const
	{
		return !buttons[(int)button] && prevButtons[(int)button];
	}

	//Return whether a button has been double clicked
	inline bool ButtonDblClicked(MouseButton button) const
	{
		return dblButtons[(int)button];
	}

	inline void SetSensitivity(float newSensitivity)
	{
		newSensitivity = max(.01f, newSensitivity);
		sensitivity = newSensitivity;
	}
	
	//Set whether vertical movement of the mouse is treated as direct movement or inverse movement.
	inline void SetInverted(bool newInverted)
	{
		inverted = newInverted;
	}

	//Return how much the mouse moved on the x axis since the last frame
	inline float DX() const
	{
		return dx;
	}

	//Return how much the mouse moved on the y axis since the last frame
	inline float DY() const
	{
		return dy;
	}

	//Return how much the wheel has been scrolled since the last frame
	inline int WheelDX() const
	{
		return wheelDX;
	} 

	//Get the current mouse position relative to the game window
	inline POINT Pos() const
	{
		return pos;
	}

	//Set that a button is down
	inline void OnMouseDown(MouseButton button)
	{ 
		buttons[(int)button] = true;
		pressed = true;
	}

	//Set that a button has been released
	inline void OnMouseUp(MouseButton button)
	{
		buttons[(int)button] = false;
	}

	//Set that a button has been double clicked
	inline void OnMouseDBLClick(MouseButton button)
	{
		dblButtons[(int)button] = true;
		buttons[(int)button] = true;
	}

	//Set that the mouse wheel has been scrolled
	inline void OnMouseWheel(int wheeldx)
	{
		wheelDX = wheeldx/120;//Goes in increments of 120 so I'm just scaling down to increments of 1
	}

	//Set whether the cursor is currently visible
	inline void ShowMouse(bool show)
	{
		visible = show;
		ShowCursor(visible); 
	}

	inline void ShowAndUnlockMouse(const HWND window)
	{
		ShowMouse(true);
		LockMouse(false, window);
	}

	inline void HideAndLockMouse(const HWND window)
	{
		ShowMouse(false);
		LockMouse(true, window);
	}

	//Return whether the cursor is currently bound to the game window or not
	inline bool IsLocked() const
	{
		return locked;
	}

	//Return whether the mouse has moved since the last frame
	inline bool HasMoved() const
	{
		return moved;
	}

	//Return whether the mouse has been clicked since the last frame
	inline bool IsPressed() const
	{
		return pressed;
	}

	inline bool IsVisible() const
	{
		return visible;
	}

	inline void CenterCursor()
	{  
		SetCursorPos(centerPosScreen.x, centerPosScreen.y);

		//Set the position now so prevPos will get the correct value (center of screen) on next mouse_move event.
		pos = centerPosClient;
	}
	 
protected:
	RAWINPUT* raw;
	UINT dwSize;
	BYTE lpb[40];
	POINT pos, prevPos, centerPosScreen, centerPosClient;
	float dx, dy, sensitivity;
	int wheelDX;
	bool registered, pressed, visible, moved, locked, inverted;
	bool buttons[5], dblButtons[5], prevButtons[5];
}; 