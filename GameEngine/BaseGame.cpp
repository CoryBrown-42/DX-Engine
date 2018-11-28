#include <io.h>
#include <fcntl.h>

#include "BaseGame.h"

//The namespace and GlobalProcessMessage are necessary to assign the 
//ProcessMessage function to the Window application itself
namespace//unnamed namespace hides this global from other files.
{
	BaseGame *baseGame = 0;
}

//This tells the window to call our ProcessMessage function when it gets a signal
LRESULT CALLBACK GlobalProcessMessage(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return baseGame->ProcessMessage(handle, msg, wParam, lParam);
}

//BaseGame Constructor
BaseGame::BaseGame(HINSTANCE instance)
	: gameInstance(instance),
	driverType(D3D_DRIVER_TYPE_HARDWARE),
	windowWidth(800),
	windowHeight(450),
	aliasingEnabled(false),//!!!IMPORTANT!!! Post Processing (using render targets) is incompatible with hardware anti-aliasing.
	gameWindow(0),
	paused(false),
	minimized(false),
	maximized(false),
	resizing(false),
	vsync(0),
	aliasingQuality(1),
	device(0),
	deviceContext(0),
	swapChain(0),
	depthStencilBuffer(0),
	renderTargetView(0),
	depthStencilView(0),
	frameRate(0),
	targetFrameRate(60),
	targetDeltaTime(1000 / targetFrameRate)
{
	#if defined(_DEBUG)		
		//Create Console
		AllocConsole();
		SetConsoleTitle(L"Debug Console");
		
		freopen_s(&consoleFile, "CONOUT$", "w", stdout);
		freopen_s(&consoleFile, "CONOUT$", "w", stderr); 

		//Move Console
		RECT R = { 0, 0, windowWidth, windowHeight };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		int width = R.right - R.left;
		HWND consoleHandle = GetConsoleWindow();
		HMENU hmenu = GetSystemMenu(consoleHandle, FALSE);
		DeleteMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
		MoveWindow(consoleHandle, width, 0, 800, 400, true);
	#else
		//Create Log File
		freopen_s(&logFile, "log.txt", "w", stdout); 
	#endif
		 
	printf("Output for printf, and cout statements.\n\n");
	std::cout << "Use this to help you understand what's going on in the engine!\n\n";

	#if defined(_DEBUG)
		updateTimer = AlgorithmTimer("Update");
		drawTimer = AlgorithmTimer("Draw"); 
		algorithmTimers.push_back(&updateTimer);
		algorithmTimers.push_back(&drawTimer);
	#endif

	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	baseGame = this;

	keyboard = new Keyboard();
	if (!keyboard->isConnected())
		SafeDelete(keyboard);

	mouse = new Mouse();
	if (!mouse->isConnected())
		SafeDelete(mouse);
}

//Init creates the window and DX devices (swapchain, device, and deviceContext)
bool BaseGame::Init()
{			
	if(!InitMainWindow())
		return false;

	if(!InitD3D())
		return false;

	return true;
}

//InitMainWindow creates the actual Game Window. This is where 
//we assign the ProcessMessage Function so it can listen to events
bool BaseGame::InitMainWindow()
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc   = GlobalProcessMessage;  // Can't be a member function!  Hence our global version
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = gameInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"WindowClass";

	if(!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, windowWidth, windowHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	gameWindow = CreateWindow(L"WindowClass", windowCaption.c_str(), 
		WS_OVERLAPPEDWINDOW, 0, 0, width, height, 0, 0, gameInstance, 0); 
	
	if(!gameWindow)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(gameWindow, SW_SHOW);
	UpdateWindow(gameWindow);

	return true;
}

//InitD3D creates the device, deviceContext, and SwapChain objects
bool BaseGame::InitD3D()
{
	UINT createDeviceFlags = 0;
	 
	#if defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	//These are all the levels it is recommended that we support.
	//To force your card to act like an older one for testing compatability,
	//just comment out levels higher than the one you want to test.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	}; 
	
	//handle is a pointer to our program's process
	HRESULT hResult = D3D11CreateDevice(
	0,
	driverType,
	0,
	createDeviceFlags,
	featureLevels,
	ARRAYSIZE(featureLevels),
	D3D11_SDK_VERSION,
	&device,
	&featureLevel,
	&deviceContext);

	//HR - HRESULT - just stores error messages from directx functions
	if(FAILED(hResult))
	{
		MessageBox(0, L"D3D11CreateDevice Failed", 0, 0);
		return false;
	}

	//This HR macro is a shortcut to declare an hresult and check for an error message.
	HR(device->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		4,
		&aliasingQuality));

	if(aliasingQuality < 0)
		aliasingQuality = 1;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width = windowWidth;
	swapChainDesc.BufferDesc.Height = windowHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format	= DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = gameWindow;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags	= 0;

	if(aliasingEnabled)
	{
		swapChainDesc.SampleDesc.Count   = 4;
		swapChainDesc.SampleDesc.Quality = aliasingQuality - 1;
	}
	else
	{
		swapChainDesc.SampleDesc.Count   = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}
	 
	IDXGIDevice	*dxgiDevice	= 0;
	IDXGIAdapter *dxgiAdapter = 0;
	IDXGIFactory *dxgiFactory = 0;
	HR(device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));		//Need an IDXGIDevice to get an adapter...
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));	//... Need an IDXGIAdapter to get a factory...
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));	//... Need an IDXGIFactory to make the swap chain

	HR(dxgiFactory->CreateSwapChain(device, &swapChainDesc, &swapChain));

	SafeRelease(dxgiDevice);
	SafeRelease(dxgiAdapter);
	SafeRelease(dxgiFactory);
	
	return true;
}

//Run is the actual game loop that runs every frame until its closed somehow
int BaseGame::Run()
{
	MSG msg = {0};//default initialization
	timer.Reset();
	
	//Game Loop
	while(msg.message != WM_QUIT)
	{
		#if defined (_DEBUG)
			updateTimer.Start(timer.CurrTics());
		#endif

		//GetMessage - Wait for an event to occur
		//PeekMessage - checks if an event occured since the last peek message
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{ 
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			timer.Tick();
			//Allows for framerate independent gameplay
			//We want 1 unit/second to be the same at 10fps as it is at 1000fps

			if(paused)
			{
				Sleep(100);
			}
			else
			{ 
				#if defined (_DEBUG)
					GetInput(timer.DeltaTime());
					mouse->EndFrame();
					keyboard->EndFrame();

					Update(timer.DeltaTime());
					updateTimer.Stop(timer.CurrTics());

					drawTimer.Start(timer.CurrTics()); 
					Draw();
					drawTimer.Stop(timer.CurrTics());
				#else					
					GetInput(timer.DeltaTime());
					mouse->EndFrame();
					keyboard->EndFrame();

					Update(timer.DeltaTime()); 
					Draw();
				#endif

				GenerateFrameStats();

				#if defined (_RELEASE)
					//Limit to ~70FPS to reduce CPU usage.
					if (timer.DeltaTime() < targetDeltaTime)
						Sleep((DWORD)(targetDeltaTime - timer.DeltaTime()));
				#endif
			}
		}
	}

	return (int)msg.wParam;
}

//ProcessMessage is what the window uses to listen to events like keyboard input, mouse, resizing, etc.
//Keep it in order of most likely first, for optimization
LRESULT BaseGame::ProcessMessage(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (mouse != nullptr)
	{
		switch (msg)
		{
		case WM_MOUSEMOVE:
			mouse->OnMouseMove(lParam);
			return 0;

		case WM_INPUT:
			mouse->MouseCallback(lParam);
			return 0;

		case WM_MOUSEWHEEL:
			mouse->OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
			return 0;

		case WM_LBUTTONDOWN:
			mouse->OnMouseDown(MouseButton::LBUTTON);
			return 0;
		case WM_MBUTTONDOWN:
			mouse->OnMouseDown(MouseButton::MBUTTON);
			return 0;
		case WM_RBUTTONDOWN:
			mouse->OnMouseDown(MouseButton::RBUTTON);
			return 0;
		case WM_XBUTTONDOWN:
			if (wParam & 0x0020)
				mouse->OnMouseDown(MouseButton::BUTTON4);
			else if (wParam & 0x0040)
				mouse->OnMouseDown(MouseButton::BUTTON5);
			return 0;

		case WM_LBUTTONUP:
			mouse->OnMouseUp(MouseButton::LBUTTON);
			return 0;
		case WM_MBUTTONUP:
			mouse->OnMouseUp(MouseButton::MBUTTON);
			return 0;
		case WM_RBUTTONUP:
			mouse->OnMouseUp(MouseButton::RBUTTON);
			return 0;
		case WM_XBUTTONUP:
			if (HIWORD(wParam) == 1)//Not sure why this is different from XButtonDown or XButtonDblClk
				mouse->OnMouseUp(MouseButton::BUTTON4);
			else if (HIWORD(wParam) == 2)
				mouse->OnMouseUp(MouseButton::BUTTON5);
			return 0;

		case WM_LBUTTONDBLCLK:
			mouse->OnMouseDBLClick(MouseButton::LBUTTON);
			return 0;
		case WM_MBUTTONDBLCLK:
			mouse->OnMouseDBLClick(MouseButton::MBUTTON);
			return 0;
		case WM_RBUTTONDBLCLK:
			mouse->OnMouseDBLClick(MouseButton::RBUTTON);
			return 0;
		case WM_XBUTTONDBLCLK:
			if (wParam & 0x0020)
				mouse->OnMouseDBLClick(MouseButton::BUTTON4);
			else if (wParam & 0x0040)
				mouse->OnMouseDBLClick(MouseButton::BUTTON5);
			return 0;
		}
	}

	if (keyboard != nullptr)
	{
		switch (msg)
		{  
		case WM_CHAR:
			//Get text input here (usernames, textboxes, chat)
			return 0;

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			keyboard->OnKeyDown(wParam); 
			return 0;

		case WM_KEYUP:
		case WM_SYSKEYUP:
			keyboard->OnKeyUp(wParam); 
			return 0;
		}
	}

	switch (msg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			paused = true;
			timer.Stop();
		}
		else
		{
			paused = false;
			timer.Start();
			OnFocus();//Invoked for input devices...			
		}
		return 0;

	case WM_SIZE:
		windowWidth = LOWORD(lParam);
		windowHeight = HIWORD(lParam); 
		if (device)
		{
			switch (wParam)
			{
			case SIZE_MINIMIZED:
				paused = true;
				minimized = true;
				maximized = false;
				return 0;
			case SIZE_MAXIMIZED:
				paused = false;
				minimized = false;
				maximized = true;
				OnResize();
				return 0;
			case SIZE_RESTORED:
				if (minimized)
				{
					paused = false;
					minimized = false;
					OnResize();
				}
				else if (maximized)
				{
					paused = false;
					maximized = false;
					OnResize();
				}
				else if (!resizing)//API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
				return 0;
			}
		}
		return 0;

	case WM_ENTERSIZEMOVE:
		paused = true;
		resizing = true;
		timer.Stop();
		return 0;

	case WM_EXITSIZEMOVE:
		paused = false;
		resizing = false;
		timer.Start();
		OnResize();
		return 0;

	case WM_GETMINMAXINFO://Catch this message so to prevent the window from becoming too small.
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;	

	case WM_MENUCHAR://sent when a menu is active and the user presses a key that does not correspond to any mnemonic or accelerator key. 		
		return MAKELRESULT(0, MNC_CLOSE);//Don't beep when we alt-enter.	

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(handle, msg, wParam, lParam);
}

//GenerateFrameStats calculates the frame rate and other metrics and displays them in the titlebar
void BaseGame::GenerateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;	 

	++frameCnt;
	
	if ((timer.TotalTime() - timeElapsed) >= 1.0f)
	{
		frameRate = (float)frameCnt;

		#if defined (_DEBUG)
			std::wostringstream outs;
			outs.precision(6);

			outs << windowCaption << L" "
				<< L"FPS: " << frameRate << " ";
		
			for (unsigned int i = 0; i < algorithmTimers.size(); ++i)
				outs << algorithmTimers[i]->Name().c_str() << L": " << algorithmTimers[i]->AverageAndClear() << L" ";
			outs << L"(tics) ";

			usageMonitor.Update();
			outs << L"CPU: " << usageMonitor.CPUPercent() << L"% "
				<< L"RAM: " << usageMonitor.WorkingSetSizeMB()
				<< L" VRAM: " << usageMonitor.PagefileUsageMB() << " (MB)";

			SetWindowText(gameWindow, outs.str().c_str());
		#endif

		frameCnt = 0;
		timeElapsed += 1.0f;
	}	
}

//OnResize is called whenever ProcessMessage detects that the user changed the size of the window
void BaseGame::OnResize()
{
	SafeRelease(renderTargetView);
	SafeRelease(depthStencilView);
	SafeRelease(depthStencilBuffer);

	HR(swapChain->ResizeBuffers(
		1, 
		windowWidth, 
		windowHeight, 
		DXGI_FORMAT_R8G8B8A8_UNORM,
		0));
	ID3D11Texture2D* backBuffer;
	HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(device->CreateRenderTargetView(backBuffer, 0, &renderTargetView));

	BOOL fullscreen;
	swapChain->GetFullscreenState(&fullscreen,NULL);
	if (fullscreen)
		vsync = 1;
	else
		vsync = 0;

	SafeRelease(backBuffer);

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width	= windowWidth;
	depthStencilDesc.Height	= windowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format	= DXGI_FORMAT_R32_TYPELESS;/*For reading depth buffer*///DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags = 0;

	if(aliasingEnabled)
	{
		depthStencilDesc.SampleDesc.Count= 4;
		depthStencilDesc.SampleDesc.Quality = aliasingQuality - 1;
	}
	else
	{ 
		depthStencilDesc.SampleDesc.Count   = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}
	 
    D3D11_DEPTH_STENCIL_VIEW_DESC ddesc;
    ZeroMemory(&ddesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	ddesc.Format = DXGI_FORMAT_D32_FLOAT;
	ddesc.ViewDimension = aliasingEnabled ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	ddesc.Texture2D.MipSlice = 0;

	HR(device->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer));
	HR(device->CreateDepthStencilView(depthStencilBuffer, &ddesc, &depthStencilView));
	 
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	 
	viewport.TopLeftX	= 0;
	viewport.TopLeftY	= 0;
	viewport.Width		= (float)windowWidth;
	viewport.Height		= (float)windowHeight;
	viewport.MinDepth	= 0.0f;
	viewport.MaxDepth	= 1.0f;
	deviceContext->RSSetViewports(1, &viewport);
	
	if (mouse)
		mouse->OnResize(windowWidth, windowHeight, gameWindow);
}

//BaseGame Destructor - Release any DX objects here
BaseGame::~BaseGame()
{
	SafeDelete(keyboard); 
	SafeDelete(mouse);
	SafeRelease(depthStencilBuffer);
	SafeRelease(renderTargetView);
	SafeRelease(depthStencilView);
	SafeRelease(swapChain);

	deviceContext->Flush();
	deviceContext->ClearState();
	SafeRelease(deviceContext);

#if defined(_DEBUG)
	//If the ref count reported is 2, they are this debug object and the device itself, so you're good to go.
	ID3D11Debug *debug;
	device->QueryInterface(__uuidof(ID3D11Debug), (LPVOID*)&debug);
#endif

	SafeRelease(device);

#if defined(_DEBUG)
	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	SafeRelease(debug);

	if (consoleFile)
		fclose(consoleFile);
	if (logFile)
		fclose(logFile);
#endif
}