#pragma once

#include <WindowsX.h>
#include <Windows.h>
#include <string>
#include <d3d11.h>
#include <assert.h>
#include <vector>
#include <sstream>  
 
#include "dxerr.h" 
#include "GameTimer.h"
#include "AlgorithmTimer.h" 
#include "UsageMonitor.h"
#include "Keyboard.h"
#include "Mouse.h"

using namespace DirectX; 

/*
BaseGame describes what any DirectX game would require, as opposed to Game which describes our particular game.
Remember that BaseGame::Run() is the actual game loop and BaseGame::ProcessMessage is where all events like input, resizing, etc. goes
*/
class BaseGame
{
public:
	BaseGame(HINSTANCE instance);
	virtual ~BaseGame();
	int Run();
	virtual bool Init();
	virtual void OnResize(); 
	virtual void GetInput(float dt) = 0;
	virtual void Update(float dt) = 0;
	virtual void Draw() = 0;
	virtual void Clear() = 0;	
	virtual LRESULT ProcessMessage(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam);

	//Returns the AspectRatio of the screen
	inline float AspectRatio() const
	{
		return (float)windowWidth / windowHeight;
	}

	//When the window becomes active i.e. if alt tab away and click on it again
	inline virtual void BaseGame::OnFocus()
	{

	}

protected: 
	HWND gameWindow;
	HINSTANCE gameInstance;
	ID3D11Device *device;
	ID3D11DeviceContext *deviceContext;
	IDXGISwapChain *swapChain;
	ID3D11Texture2D *depthStencilBuffer;
	ID3D11RenderTargetView *renderTargetView;
	ID3D11DepthStencilView *depthStencilView;
	D3D11_VIEWPORT viewport;
	D3D_DRIVER_TYPE driverType;
	D3D_FEATURE_LEVEL featureLevel;
	GameTimer timer;
	Keyboard *keyboard;
	Mouse *mouse; 
	std::wstring windowCaption;
	bool paused, minimized, maximized, resizing;
	int windowWidth, windowHeight;
	float frameRate;
	float targetFrameRate;
	float targetDeltaTime;
	FILE *consoleFile = nullptr, *logFile = nullptr; 

	//Things that should be in a settings container that can be read from config file and eventually be configured by a user.
	UINT aliasingQuality, vsync;
	bool aliasingEnabled;
	 
	//Dev stuff for studying CPU and RAM usage, performance, etc.
	#if defined (_DEBUG)
		UsageMonitor usageMonitor;
		std::vector<AlgorithmTimer*> algorithmTimers;
		AlgorithmTimer drawTimer, updateTimer;
	#endif	
	
	bool InitMainWindow();
	bool InitD3D();
	void GenerateFrameStats();	
};