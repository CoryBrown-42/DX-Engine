#pragma region Includes and Namespaces
#include <string>

#include "ExampleGame.h"
#include "VertexDefinitions.h"
#include "Geometry.h"
#include "ModelImporter.h"
#include "Mesh.h"

using namespace std;
using namespace DirectX;
#pragma endregion

#pragma region Setup
//The Game() constructor
ExampleGame::ExampleGame(HINSTANCE instance)
	: BaseGame(instance)
{
	windowCaption = L"ExampleGame ";
}

//Init() is for initializing the game Objects. 
bool ExampleGame::Init()
{
	if (!BaseGame::Init())
		return false;	 

	srand((UINT)time(0));//Seed Random so its different every run.
	 
	//background color
	clearcolor[0] = 0;//Red
	clearcolor[1] = 0;//Green
	clearcolor[2] = 0;//Blue
	clearcolor[3] = 1;//Alpha

	//Create Font and SpriteBatch for drawing text on the screen
	//Spritebatch/Font comes from the DirectX Tool Kit, which is Microsoft provided code
	spriteBatch = new SpriteBatch(deviceContext);
	spriteFont = new SpriteFont(device, L"..\\Assets\\Fonts\\segoe12.spritefont"); 

	RenderManager::CreateRasterizerState(rasterizerState, device, D3D11_FILL_SOLID, D3D11_CULL_BACK);
	RenderManager::CreateRasterizerState(wireframeRS, device, D3D11_FILL_WIREFRAME, D3D11_CULL_BACK);
	RenderManager::CreateDepthStencilState(depthState, device);
	RenderManager::CreateConstantBuffer<SimpleConstantBuffer>(gpuSimpleCBuffer, device);

	textureMap["testImage"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\mipmaps.dds", device, deviceContext);
	textureMap["kirbyDiffuse"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\KirbyDiffuse1.jpg", device, deviceContext);
	textureMap["wood"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\wood1.jpg", device, deviceContext);
	textureMap["kirbySpec"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\KirbySpec2.jpg", device, deviceContext);
	textureMap["kirbyMask"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\KirbyEffectMask.jpg", device, deviceContext);
	
	samplerMap["default"] = RenderManager::CreateSamplerState(device);
	D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerMap["point"] = RenderManager::CreateSamplerState(device, D3D11_FILTER_MIN_MAG_MIP_POINT, addressMode);
	samplerMap["bilinear"] = RenderManager::CreateSamplerState(device, D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, addressMode);
	samplerMap["trilinear"] = RenderManager::CreateSamplerState(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, addressMode);
	samplerMap["anisotropic"] = RenderManager::CreateSamplerState(device, D3D11_FILTER_ANISOTROPIC, addressMode);

	//Load flat-color material (shaders/input layout)
	RenderManager::CreateVertexShaderAndInputLayout<VertexPosColNormal>(L"VS_color.cso", "color", vShaderMap, inputLayoutMap, device);
	pShaderMap["color"] = RenderManager::CreatePixelShader(L"PS_color.cso", device);

	//Load lighting material
	RenderManager::CreateVertexShaderAndInputLayout<VertexPosColNormal>(L"VS_light.cso", "lit", vShaderMap, inputLayoutMap, device);
	pShaderMap["lit"] = RenderManager::CreatePixelShader(L"PS_light.cso", device);
	
	//Load texturing material
	RenderManager::CreateVertexShaderAndInputLayout<VertexPosNormTexture>(L"VS_litTextured.cso", "litTextured", vShaderMap, inputLayoutMap, device);
	pShaderMap["litTextured"] = RenderManager::CreatePixelShader(L"PS_litTextured.cso", device);

	//Setup Globals so we can conveniently get these commonly used objects
	Globals::Get().SetDevice(device);
	Globals::Get().SetDeviceContext(deviceContext);
	Globals::Get().SetGameCBuffer(&gameSimpleCBuffer);
	Globals::Get().SetGPUCBuffer(gpuSimpleCBuffer);

	//Generate models
	meshMap["Square"] = new Mesh<VertexPositionColor>();
	Geometry::CreateEfficientSquare((Mesh<VertexPositionColor>*)meshMap["Square"], device);
	meshMap["TexturedSquare"] = new Mesh<VertexPosNormTexture>();
	Geometry::CreateTexturedSquare((Mesh<VertexPosNormTexture>*)meshMap["TexturedSquare"], device);

	//Import models
	meshMap["Kirby"] = new Mesh<VertexPosNormTexture>();
	ModelImporter::LoadFromFile("../Assets/Models/kirby.obj", (Mesh<VertexPosNormTexture>*)meshMap["Kirby"], device);
	meshMap["Vase"] = new Mesh<VertexPosColNormal>();
	ModelImporter::LoadFromFile("../Assets/Models/vase.obj", (Mesh<VertexPosColNormal>*)meshMap["Vase"], device);
	meshMap["Barrel"] = new Mesh<VertexPosNormTexture>();
	ModelImporter::LoadFromFile("../Assets/Models/barrel.obj", (Mesh<VertexPosNormTexture>*)meshMap["Barrel"], device);
	meshMap["Diamond"] = new Mesh<VertexPosColNormal>();
	ModelImporter::LoadFromFile("../Assets/Models/diamond.obj", (Mesh<VertexPosColNormal>*)meshMap["Diamond"], device);
	meshMap["Potion"] = new Mesh<VertexPosColNormal>();
	ModelImporter::LoadFromFile("../Assets/Models/potion.obj", (Mesh<VertexPosColNormal>*)meshMap["Potion"], device);
	meshMap["Apple"] = new Mesh<VertexPosColNormal>();
	ModelImporter::LoadFromFile("../Assets/Models/apple.obj", (Mesh<VertexPosColNormal>*)meshMap["Apple"], device);
	meshMap["Sword"] = new Mesh<VertexPosColNormal>();
	ModelImporter::LoadFromFile("../Assets/Models/sword.obj", (Mesh<VertexPosColNormal>*)meshMap["Sword"], device);
	UINT numModels = 7;
	
	//Setup other game settings as needed
	camera.useSpaceCamera = false;
	//mouse->InitMouseCallback(gameWindow);//Enable hi-res mouse input
	sun.direction = XMFLOAT3(0, -5, 1);

	//Setup game objects (ground, buildings, characters, etc.)
	gameObjects.push_back(GameObject());
	GameObject* g = &gameObjects[gameObjects.size() - 1];
	g->mesh = meshMap["TexturedSquare"];
	g->scale = XMFLOAT3(5, 5, 5);
	g->rotation.y = XMConvertToRadians(90);
	g->specIntensity = 128;

	kirbies.push_back(GameObject());
	kirbies[0].mesh = meshMap["Kirby"];

	barrels.push_back(GameObject());
	barrels[0].mesh = meshMap["Barrel"];
	barrels[0].position.x = 5;

	int maxDist = 100;
	for (int i = 0; i < 100; ++i)
	{
		gameObjects.push_back(GameObject());

		//Pick a model	
		gameObjects[gameObjects.size() - 1].mesh = meshMap["Diamond"];
		gameObjects[gameObjects.size() - 1].specIntensity = 8;

		//Set position, scale, rotation, color
		gameObjects[gameObjects.size() - 1].position =
				XMFLOAT3(rand() % maxDist - maxDist / 2.0f, //Make gameobjects randomly in a cube
						 rand() % maxDist - maxDist / 2.0f, 
						 rand() % maxDist - maxDist / 2.0f);
		float scale = rand() / (float)RAND_MAX * 5 + 1;//scale as a FLOAT between 1 and 6
		gameObjects[gameObjects.size() - 1].scale = XMFLOAT3(scale, scale, scale);//Uniform Scale
		gameObjects[gameObjects.size() - 1].color = Utility::RandomColor();
		gameObjects[gameObjects.size() - 1].rotation.x = rand()/(float)RAND_MAX * 360;//Yaw
	}

	//We must resize once at the beginning of the game to build the initial screen buffers, camera aspect ratio, etc.
	//After that it only gets called if the game window changes size/maximizes/fullscreens, etc.
	OnResize(); 

	return true;
}
#pragma endregion

#pragma region Game Loop
//GetInput is called in BaseGame::Run before Update() and Draw().
void ExampleGame::GetInput(float dt)//dt = deltaTime
{
	if (keyboard->ButtonDown(VK_ESCAPE))
		PostQuitMessage(0); 

	float speed = 3;
	if (keyboard->ButtonDown(VK_SHIFT))
		speed = 10;

	//Framerate Independent Gameplay
	//Delta Time - The time it took to complete the last frame.
	if (keyboard->ButtonDown('D'))
		camera.Strafe(speed * dt);//By adding * dt, now its moving in units per second, instead of per frame.
	if (keyboard->ButtonDown('A'))
		camera.Strafe(-speed * dt);
	if (keyboard->ButtonDown('W'))
		camera.MoveForward(speed * dt);
	if (keyboard->ButtonDown('S'))
		camera.MoveForward(-speed * dt);

	if (keyboard->ButtonDown('1'))
		gameSimpleCBuffer.uOffset -= dt;
	if (keyboard->ButtonDown('2'))
		gameSimpleCBuffer.uOffset += dt;
	if (keyboard->ButtonDown('3'))
		gameSimpleCBuffer.vOffset -= dt;
	if (keyboard->ButtonDown('4'))
		gameSimpleCBuffer.vOffset += dt;

	if (keyboard->ButtonDown('5'))
		gameSimpleCBuffer.uScale -= dt;
	if (keyboard->ButtonDown('6'))
		gameSimpleCBuffer.uScale += dt;
	if (keyboard->ButtonDown('7'))
		gameSimpleCBuffer.vScale -= dt;
	if (keyboard->ButtonDown('8'))
		gameSimpleCBuffer.vScale += dt;

	if (keyboard->ButtonDown('Q'))
		camera.rotation.z += 5 * dt;
	if (keyboard->ButtonDown('E'))
		camera.rotation.z -= 5 * dt;

	if (keyboard->ButtonDown(VK_UP))
		ambient += dt;
	if (keyboard->ButtonDown(VK_DOWN))
		ambient -= dt;

	if (keyboard->ButtonDown(VK_LEFT))
		showNormals -= dt;
	if (keyboard->ButtonDown(VK_RIGHT))
		showNormals += dt;

	if (keyboard->ButtonDown('L'))
	{
		XMStoreFloat3(&sun.direction, //And finally store the result back in sun.direction
			XMVector3Rotate(XMLoadFloat3(&sun.direction),//Use it to rotate the sun direction (converted to an XMVECTOR)
				XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(90) * dt)));//Create a quaternion (a way of storing rotation) - 90 degrees/sec
	}
	if (keyboard->ButtonDown('K'))
	{
		XMStoreFloat3(&sun.direction, //And finally store the result back in sun.direction
			XMVector3Rotate(XMLoadFloat3(&sun.direction),//Use it to rotate the sun direction (converted to an XMVECTOR)
				XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(90) * dt)));//Create a quaternion (a way of storing rotation) - 90 degrees/sec
	}

	if (keyboard->ButtonPressed('B'))
	{
		whichSampler++;
		if (whichSampler > 3)
			whichSampler = 0;
	}

	if (keyboard->ButtonPressed('R'))
		useWireframe = !useWireframe;
	
	//Disable mouse controls if mouse is visible
	if (!mouse->IsVisible())
	{
		//Mouse input doesn't need dt (delta time) since the amount you can physically move it each frame is naturally scaled by the framerate.
		camera.rotation.x = XMConvertToRadians(mouse->DX());
		camera.rotation.y = XMConvertToRadians(mouse->DY());//I added an 'inverted' bool to the mouse class so you don't need to negate this here anymore.
	}

	//toggle mouse visibility. 
	if (keyboard->ButtonPressed(VK_TAB))
	{
		if (mouse->IsVisible())
			mouse->HideAndLockMouse(gameWindow);
		else
			mouse->ShowAndUnlockMouse(gameWindow);
	}
} 

//Update is for movement, AI, physics, most gameplay things.
void ExampleGame::Update(float dt)
{
	if (dt > 50)
		cout << "Framerate: " << 1000.0f/dt << "! If your framerate is this low you are doing something wrong!!!" << endl;

	if (!mouse->IsVisible())
		mouse->CenterCursor();//Only do this after using the mouse's current position/dx/dy, etc. this frame.

	gameSimpleCBuffer.ambientLightIntensity = ambient;
	gameSimpleCBuffer.showNormals = showNormals;

	sun.Update();
	 
	camera.Update();
}

//Clear() resets the backbuffer texture so that we are drawing to a blank screen
void ExampleGame::Clear()
{
	deviceContext->ClearRenderTargetView(renderTargetView, clearcolor);
	deviceContext->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
}

//Draw is where we actually render to the screen
void ExampleGame::Draw()
{
	//Clear the buffer (otherwise we'll see what was drawn on it before)
	Clear();

	deviceContext->OMSetDepthStencilState(depthState, 0);
	if (useWireframe)
		deviceContext->RSSetState(wireframeRS);
	else
		deviceContext->RSSetState(rasterizerState);

	//View our vertex buffer as just a list of triangles, every 3 vertices = 1 triangle.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->VSSetConstantBuffers(0, 1, &gpuSimpleCBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &gpuSimpleCBuffer);

	//Tell the gpu what 'material' to use to draw
	deviceContext->VSSetShader(vShaderMap["lit"], NULL, 0);
	deviceContext->PSSetShader(pShaderMap["lit"], NULL, 0);
	deviceContext->IASetInputLayout(inputLayoutMap["lit"]);

	//Draw only the objects that will use the above material (VertexPosColNormal)
	for (UINT i = 1; i < gameObjects.size(); ++i)
		gameObjects[i].Draw();

	//Tell the gpu what 'material' to use to draw
	deviceContext->VSSetShader(vShaderMap["color"], NULL, 0);
	deviceContext->PSSetShader(pShaderMap["color"], NULL, 0);
	deviceContext->IASetInputLayout(inputLayoutMap["color"]);

	//Draw gameobjects that use VertexPositionColor here

	deviceContext->VSSetShader(vShaderMap["litTextured"], NULL, 0);
	deviceContext->PSSetShader(pShaderMap["litTextured"], NULL, 0);
	deviceContext->IASetInputLayout(inputLayoutMap["litTextured"]);

	//Draw gameobjects that use VertexPositionColor here
	deviceContext->PSSetShaderResources(0, 1, &textureMap["testImage"]);

	switch (whichSampler)
	{
	case 0:
		deviceContext->PSSetSamplers(0, 1, &samplerMap["point"]);
		break;
	case 1:
		deviceContext->PSSetSamplers(0, 1, &samplerMap["bilinear"]);
		break;
	case 2:
		deviceContext->PSSetSamplers(0, 1, &samplerMap["trilinear"]);
		break;
	case 3:
		deviceContext->PSSetSamplers(0, 1, &samplerMap["anisotropic"]);
		break;
	}
	gameObjects[0].Draw();  

	deviceContext->PSSetShaderResources(0, 1, &textureMap["kirbyDiffuse"]);
	deviceContext->PSSetShaderResources(1, 1, &textureMap["kirbySpec"]);
	deviceContext->PSSetShaderResources(2, 1, &textureMap["kirbyMask"]);
	kirbies[0].color = Utility::RandomColor();
	kirbies[0].Draw();
	
	//Draw text to the screen
	spriteBatch->Begin(); 
	{
		spriteFont->SetLineSpacing(0);
		float offset = spriteFont->MeasureString(L"ABC").m128_f32[1];
		XMFLOAT4 col = Utility::RandomColor();
		XMVECTOR textColor = XMLoadFloat4(&col);
		spriteFont->DrawString(spriteBatch, std::wstring(L"FPS: " + std::to_wstring((int)frameRate)).c_str(), XMFLOAT2(0, 0), textColor);
		spriteFont->DrawString(spriteBatch, L"Toggle Mouse: Tab", XMFLOAT2(0, offset * 1), textColor);
		spriteFont->DrawString(spriteBatch, L"Rotate Light: L", XMFLOAT2(0, offset * 2), textColor);
		spriteFont->DrawString(spriteBatch, L"Movement: WASD", XMFLOAT2(0, offset * 3), textColor);
		spriteFont->DrawString(spriteBatch, L"Increase Ambience: Up/Down Arrows", XMFLOAT2(0, offset * 4), textColor);
		spriteFont->DrawString(spriteBatch, L"Blend between Color and Normals: Left/Right Arrows", XMFLOAT2(0, offset * 5), textColor);
	}
	spriteBatch->End();	

	//Finally present new image to the screen
	HR(swapChain->Present(vsync, 0));
}	
#pragma endregion

#pragma region Events
//Reacquire devices when the window loses and regains focus
void ExampleGame::OnFocus()
{
	if(mouse != nullptr)
		mouse->onFocus(gameWindow);

	if(keyboard != nullptr)
		keyboard->onFocus(gameWindow);
}

//OnResize is called whenever the user changes the size of the window - update anything that depends on the window size here.
void ExampleGame::OnResize()
{
	BaseGame::OnResize();

	Globals::Get().SetAspectRatio(AspectRatio());
}

//The ~Game() destructor is for releasing and deleting all our objects
//Remember Release is for DirectX objects, and delete is for pointers that use the 'new' keyword
ExampleGame::~ExampleGame()
{
	//Delete our things
	SafeDelete(spriteFont);
	SafeDelete(spriteBatch);
	for (auto it = meshMap.begin(); it != meshMap.end(); ++it)
		delete it->second;

	//Release DirectX Things
	//Reminder: the goal is to see a refcount of 2 when
		//you run in debug mode and look at the output log
		//the last 2 objects are the device and debug object, 
		//they'll release in ~BaseGame
	SafeRelease(rasterizerState);
	SafeRelease(wireframeRS);
	SafeRelease(depthState);
	SafeRelease(gpuSimpleCBuffer);
	for (auto it = inputLayoutMap.begin(); it != inputLayoutMap.end(); ++it)
		it->second->Release();
	for (auto it = vShaderMap.begin(); it != vShaderMap.end(); ++it)
		it->second->Release();
	for (auto it = pShaderMap.begin(); it != pShaderMap.end(); ++it)
		it->second->Release();
}
#pragma endregion