#pragma once

/*
Controller acts as an interface for the different kind of devices.
*/
class InputDevice
{
public:
	virtual void onFocus(const HWND &windowHandle) = 0;

	//Constructor
	inline InputDevice()
		: connected(false)
	{

	}

	//Destructor
	virtual ~InputDevice()
	{

	}

	//Return whether the device is connected
	inline bool isConnected() const
	{
		return connected;
	}

protected:
	bool connected;
};