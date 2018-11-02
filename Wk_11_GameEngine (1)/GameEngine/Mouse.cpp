#include "Mouse.h"

//Constructor
Mouse::Mouse(float snstvty)
	: sensitivity(snstvty),
	raw(nullptr),
	dx(0),
	dy(0),
	dwSize(40),
	locked(false),
	moved(false),
	pressed(false),
	wheelDX(0),
	visible(true),
	registered(false),
	inverted(false)
{
	if (GetSystemMetrics(SM_MOUSEPRESENT))
		connected = true;

	std::fill_n(buttons, 5, false);
	std::fill_n(prevButtons, 5, false);
	std::fill_n(dblButtons, 5, false);

	/*
	//This gets the monitor's DPI settings (dots per inch) to scale things by
	//so our game/app is compatible with fancy high-dpi displays
	//to use it add #include <d2d1_1.h> here and d2d1.lib under Project Properties->Linker->Input->Additional Dependencies
	ID2D1Factory *factory;
	HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory));
	float dpix, dpiy;
	factory->GetDesktopDpi(&dpix, &dpiy);
	float dipScaleX = dpix / 96.0f, dipScaleY = dpiy / 96.0f;
	SafeRelease(factory);
	*/
}

//Register mouse to get high-res input data through WM_INPUT signals
void Mouse::InitMouseCallback(const HWND &windowHandle)
{
	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = windowHandle;
	RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
	registered = true;//When registered this way we get higher resolution mouse data and it fills 'dx' and 'dy' directly.
}

//Update the mouse when it receives an WM_INPUT signal
void Mouse::MouseCallback(LPARAM lParam)
{
	GetRawInputData((HRAWINPUT)lParam, RID_INPUT,
		lpb, &dwSize, RAWHEADSIZE);
	raw = (RAWINPUT*)lpb;
	if (raw->header.dwType == RIM_TYPEMOUSE)
	{	
		moved = true;
		dx = raw->data.mouse.lLastX * sensitivity;
		dy = raw->data.mouse.lLastY * sensitivity; 
		dy = inverted ? -dy : dy;
	}	
}

//http://msdn.microsoft.com/en-us/library/windows/desktop/ms648380(v=vs.85).aspx#_win32_Confining_a_Cursor
//This ensures the mouse is within the play space at all times.
void Mouse::ClipMouse(const HWND &windowHandle)
{
	RECT window;
	GetClientRect(windowHandle, &window);

	// Nifty trick takes advantage of the fact that RECT is 
	// {LONG,LONG,LONG,LONG} and POINT is {LONG,LONG}. 
	ClientToScreen(windowHandle, reinterpret_cast<POINT*>(&window.left));
	ClientToScreen(windowHandle, reinterpret_cast<POINT*>(&window.right));

	// Ensures the mouse is in play space. 
	SetCursorPos((window.top + window.bottom) / 2, (window.left + window.right) / 2); 

	ClipCursor(&window);
}

//Set the new mouse position and that the mouse has been moved
void Mouse::OnMouseMove(LPARAM lParam)
{
	prevPos = pos;

	//lparam stores client-based mouse position (relative to the window, not the screen)
	pos.x = (long)GET_X_LPARAM(lParam);
	pos.y = (long)GET_Y_LPARAM(lParam);

	if (!registered)//Get low res mouse data if not using raw input (if you didn't call mouse->InitMouseCallback in your game)
	{
		dx = (float)(pos.x - prevPos.x) * sensitivity;
		dy = (float)(pos.y - prevPos.y) * sensitivity; 
		dy = inverted ? -dy : dy;
	}

	moved = true;
}

void Mouse::OnResize(int screenWidth, int screenHeight, HWND handle)
{
	//Make this happen once (updated on resize) so this function is much more efficient.
	centerPosScreen.x = screenWidth / 2;
	centerPosScreen.y = screenHeight / 2;

	//SetCursorPos wants the position on screen, not position in the window. This converts to the window position.
	centerPosClient = centerPosScreen;
	ScreenToClient(handle, &centerPosClient);
}

//Lock the mouse to the game window
//If the game is not fullscreen and you are using a cursor in the game like in a menu,
//it is preferrable to be able to leave the game window, "X" out, go to other windows, etc.
void Mouse::LockMouse(bool lock, const HWND &windowHandle)
{
	if (locked != lock)
	{
		locked = lock;
		if (locked)
			ClipMouse(windowHandle);
		else
			ClipCursor(NULL);
	}
}

//Reset at the end of a frame
void Mouse::EndFrame()
{
	std::fill_n(dblButtons, 5, false);
	wheelDX = 0;
	dx = dy = 0;
	moved = pressed = false; 
}