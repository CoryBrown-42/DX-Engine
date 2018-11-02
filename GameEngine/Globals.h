#pragma once

#include <iostream>

#include "ConstantBufferDefinitions.h"

using namespace std;

//Singleton - Convenience of global variables, protection of a class
class Globals final
{
private:
	Globals() {}//private constructor makes this a singleton

public:
	static Globals& Get()
	{
		//static here means instance will only be instantiated the first time 
		//this code is reached, and deallocated when the whole program closes
		static Globals instance;
		return instance;
	}

	//Prevent any method of making copies of this object
	Globals(Globals& copy) = delete;//= delete means 'don't auto-generate this function' since 2011
	void operator=(Globals& copy) = delete;
		
private://Variables we want to store in Globals
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	SimpleConstantBuffer* gameCBuffer = nullptr;
	ID3D11Buffer* gpuCBuffer = nullptr;
	float aspectRatio = 0;

public://Get/Set functions for variables
	void SetGPUCBuffer(ID3D11Buffer* cBuffer) { gpuCBuffer = cBuffer; }
	ID3D11Buffer* GetGPUCBuffer()
	{
		if (!gpuCBuffer)
			cout << "Error - gpuCBuffer not set in Globals." << endl;
		return gpuCBuffer;
	}
	
	void SetGameCBuffer(SimpleConstantBuffer* simpleCBuffer) { gameCBuffer = simpleCBuffer; }
	SimpleConstantBuffer* GetGameCBuffer()
	{
		if (!gameCBuffer)
			cout << "Error - gameCBuffer not set in Globals." << endl;
		return gameCBuffer;
	}
	
	void SetDeviceContext(ID3D11DeviceContext* dC){ deviceContext = dC; }
	ID3D11DeviceContext* GetDeviceContext()
	{
		if (!deviceContext)
			cout << "Error - deviceContext not set in Globals." << endl;
		return deviceContext;
	}

	void SetDevice(ID3D11Device* d) { device = d; }
	ID3D11Device* GetDevice()
	{
		if (!device)
			cout << "Error - device not set in Globals." << endl;
		return device;
	}

	void SetAspectRatio(float aR) { aspectRatio = aR; }
	float GetAspectRatio()
	{
		if (aspectRatio == 0)
			cout << "Error - aspect ratio not set in Globals." << endl;
		return aspectRatio;
	}
}; 