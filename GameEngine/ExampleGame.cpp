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
	spriteFont->SetLineSpacing(0);

	//Setup constant buffers
	RenderManager::CreateConstantBuffer<SimpleConstantBuffer>(gpuSimpleCBuffer, device);
	
	//Setup Globals so we can conveniently get these commonly used objects
	Globals::Get().SetDevice(device);
	Globals::Get().SetDeviceContext(deviceContext);
	Globals::Get().SetGameCBuffer(&gameSimpleCBuffer);
	Globals::Get().SetGPUCBuffer(gpuSimpleCBuffer);

	//Create states and buffers.
	RenderManager::CreateRasterizerState(rasterizerState, device, D3D11_FILL_SOLID, D3D11_CULL_BACK);
	RenderManager::CreateRasterizerState(wireframeRS, device, D3D11_FILL_WIREFRAME, D3D11_CULL_BACK);
	RenderManager::CreateRasterizerState(twoSidedRS, device, D3D11_FILL_SOLID, D3D11_CULL_NONE);
	RenderManager::CreateDepthStencilState(standardDepthState, device);
	RenderManager::CreateDepthStencilState(skyDepthState, device, true, D3D11_COMPARISON_LESS_EQUAL);
	RenderManager::CreateDepthStencilState(noDepthState, device, false);
	RenderManager::CreateDepthStencilState(transparentDepthState, device, true, D3D11_COMPARISON_LESS, D3D11_DEPTH_WRITE_MASK_ZERO);
	RenderManager::CreateBlendState(noBlend, device, false);
	RenderManager::CreateBlendState(standardBlend, device);
	RenderManager::CreateBlendState(additiveBlend, device, true, false, D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD);
	RenderManager::CreateBlendState(testBlend, device, true, false, D3D11_BLEND_INV_DEST_ALPHA, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_INV_DEST_ALPHA);

	//Load Textures
	ZeroMemory(&textureMap["blank"], sizeof(ID3D11ShaderResourceView));
	textureMap["testImage"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\wood2.jpg", device, deviceContext);
	textureMap["sky"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\skybox.dds", device, deviceContext);
	textureMap["leaves"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\Leaves.png", device, deviceContext);
	textureMap["groundDiffuse"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\GroundTex.jpg", device, deviceContext);
	textureMap["groundSpec"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\GroundSpecMap.jpg", device, deviceContext);
	textureMap["groundNormal"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\GroundNormalMap.jpg", device, deviceContext);
	textureMap["barrelDiffuse"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\BarrelTex.jpg", device, deviceContext);
	textureMap["barrelSpec"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\BarrelSpecMap.jpg", device, deviceContext);
	textureMap["barrelNormal"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\BarrelNormalMap.jpg", device, deviceContext);
	textureMap["kirbyDiffuse"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\KirbyDiffuse1.jpg", device, deviceContext);
	textureMap["kirbySpec"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\KirbySpec2.jpg", device, deviceContext);
	textureMap["kirbyMask"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\KirbyEffectMask.jpg", device, deviceContext);
	textureMap["wellDiffuse"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\WellTex.jpg", device, deviceContext);
	textureMap["wellSpec"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\WellSpecMap.jpg", device, deviceContext);
	textureMap["wellNormal"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\WellNormalMap.jpg", device, deviceContext);
	textureMap["testNormal"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\testNormalMap.png", device, deviceContext);

	//Create Samplers
	samplerMap["default"] = RenderManager::CreateSamplerState(device);
	samplerMap["point"] = RenderManager::CreateSamplerState(device, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
	samplerMap["bilinear"] = RenderManager::CreateSamplerState(device, D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
	samplerMap["trilinear"] = RenderManager::CreateSamplerState(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
	samplerMap["anisotropic"] = RenderManager::CreateSamplerState(device, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);

	//Load Shaders
	RenderManager::CreateVertexShaderAndInputLayout<VertexPosColNormal>(L"VS_color.cso", "color", vShaderMap, inputLayoutMap, device);
	pShaderMap["color"] = RenderManager::CreatePixelShader(L"PS_color.cso", device);
	RenderManager::CreateVertexShaderAndInputLayout<VertexPosColNormal>(L"VS_light.cso", "lit", vShaderMap, inputLayoutMap, device);
	pShaderMap["lit"] = RenderManager::CreatePixelShader(L"PS_light.cso", device);
	RenderManager::CreateVertexShaderAndInputLayout<VertexPosNormTexture>(L"VS_litTextured.cso", "litTextured", vShaderMap, inputLayoutMap, device);
	RenderManager::CreateVertexShaderAndInputLayout<VertexSky>(L"VS_skybox.cso", "skybox", vShaderMap, inputLayoutMap, device);
	pShaderMap["litTextured"] = RenderManager::CreatePixelShader(L"PS_litTextured.cso", device);
	pShaderMap["skybox"] = RenderManager::CreatePixelShader(L"PS_skybox.cso", device);
	pShaderMap["masked"] = RenderManager::CreatePixelShader(L"PS_masked.cso", device);
	RenderManager::CreateVertexShaderAndInputLayout<VertexStandard>(L"VS_normalmapped.cso", "normalmapping", vShaderMap, inputLayoutMap, device);
	pShaderMap["normalmapping"] = RenderManager::CreatePixelShader(L"PS_normalmapped.cso", device);
	RenderManager::CreateVertexShaderAndInputLayout<VertexSprite>(L"VS_sprite.cso", "sprite", vShaderMap, inputLayoutMap, device);
	pShaderMap["sprite"] = RenderManager::CreatePixelShader(L"PS_sprite.cso", device);

	//Setup materials
	Material::Init();//Just allocates memory for textures and samplers. Mostly for optimization; you could do it each time you apply 1 more than before instead

	materialMap["sky"] = Material();
	materialMap["sky"].rasterizerState = rasterizerState;
	materialMap["sky"].depthState = skyDepthState;
	materialMap["sky"].vShader = vShaderMap["skybox"];
	materialMap["sky"].pShader = pShaderMap["skybox"];
	materialMap["sky"].inputLayout = inputLayoutMap["lit"];
	materialMap["sky"].textures.push_back(textureMap["sky"]);
	materialMap["sky"].samplers.push_back(samplerMap["bilinear"]);
	
	materialMap["lit"] = Material();
	materialMap["lit"].rasterizerState = rasterizerState;
	materialMap["lit"].depthState = standardDepthState;
	materialMap["lit"].blendState = noBlend;
	materialMap["lit"].vShader = vShaderMap["lit"];
	materialMap["lit"].inputLayout = inputLayoutMap["lit"];
	materialMap["lit"].pShader = pShaderMap["lit"];

	//Blending efficiency (best to worst)
		//Masked Transparency (discards some pixels)(efficient except when too many objects overlap on screen, causing overdraw)(can be drawn with opaque things)
		//Opaque objects
		//Additive transparency (doesn't require sorting, but still has to be drawn after opaque things)
		//Alpha Blending (requires sorting)
		//Multiplicative Blending (Alpha blending + 1 extra multplication operation, just Alpha blending but it looks darker)

	//Additive Blending just means add the new color onto what was there before. No 'blending' between them. 
		//Used for things like Fire and Magic, often for particle systems.
	//IF we only had opaque and additive objects, we would not have to sort these by depth to get the correct value
	//HOWEVER, in this example game we have to depth sort these too since we have a mixture of additive and alpha-blended objects.
	materialMap["additiveBlending"] = materialMap["lit"]; 
	materialMap["additiveBlending"].depthState = transparentDepthState;
	materialMap["additiveBlending"].blendState = additiveBlend;

	//Alpha blending means standard 'see-through' stuff. Windows, ghosts, etc.
	//Alpha blending uses depth buffers like normal, but you need to depth sort the objects by distance from the camera.
	materialMap["alphaBlending"] = materialMap["lit"];
	materialMap["alphaBlending"].depthState = standardDepthState;
	//materialMap["alphaBlending"].depthState = transparentDepthState;
	materialMap["alphaBlending"].blendState = standardBlend;
	materialMap["alphaBlending"].needsSorting = true;

	//Just for demoing different effects
	materialMap["testBlending"] = materialMap["lit"];
	materialMap["testBlending"].depthState = transparentDepthState;
	materialMap["testBlending"].blendState = testBlend;
	materialMap["testBlending"].needsSorting = true;

	materialMap["textured"] = Material();
	materialMap["textured"].rasterizerState = rasterizerState;
	materialMap["textured"].depthState = standardDepthState;
	materialMap["textured"].blendState = noBlend;
	materialMap["textured"].vShader = vShaderMap["litTextured"];
	materialMap["textured"].inputLayout = inputLayoutMap["litTextured"];
	materialMap["textured"].pShader = pShaderMap["litTextured"];
	materialMap["textured"].samplers.push_back(samplerMap["anisotropic"]);

	materialMap["leaves"] = materialMap["textured"];
	materialMap["leaves"].pShader = pShaderMap["masked"];
	materialMap["leaves"].rasterizerState = twoSidedRS;
	materialMap["leaves"].textures.push_back(textureMap["leaves"]);
	materialMap["leaves"].textures.push_back(textureMap["blank"]);
	materialMap["leaves"].textures.push_back(textureMap["blank"]);
	materialMap["leaves"].textures.push_back(textureMap["blank"]);

	materialMap["kirby"] = materialMap["textured"];
	materialMap["kirby"].textures.push_back(textureMap["kirbyDiffuse"]);
	materialMap["kirby"].textures.push_back(textureMap["kirbySpec"]);
	materialMap["kirby"].textures.push_back(textureMap["kirbyMask"]);
	materialMap["kirby"].textures.push_back(textureMap["sky"]);

	materialMap["barrel"] = materialMap["textured"];
	materialMap["barrel"].textures.push_back(textureMap["barrelDiffuse"]);
	materialMap["barrel"].textures.push_back(textureMap["barrelSpec"]);
	materialMap["barrel"].textures.push_back(textureMap["blank"]);
	materialMap["barrel"].textures.push_back(textureMap["sky"]);

	materialMap["ground"] = materialMap["textured"];
	materialMap["ground"].textures.push_back(textureMap["groundDiffuse"]);
	materialMap["ground"].textures.push_back(textureMap["groundSpec"]);
	materialMap["ground"].textures.push_back(textureMap["blank"]);
	materialMap["ground"].textures.push_back(textureMap["blank"]);
	//materialMap["ground"].textures.push_back(textureMap["sky"]);

	materialMap["well"] = Material();
	materialMap["well"].rasterizerState = rasterizerState;
	materialMap["well"].depthState = standardDepthState;
	materialMap["well"].blendState = noBlend;
	materialMap["well"].vShader = vShaderMap["normalmapping"];
	materialMap["well"].inputLayout = inputLayoutMap["normalmapping"];
	materialMap["well"].pShader = pShaderMap["normalmapping"];
	materialMap["well"].samplers.push_back(samplerMap["anisotropic"]);
	materialMap["well"].textures.push_back(textureMap["wellDiffuse"]);
	materialMap["well"].textures.push_back(textureMap["wellSpec"]);
	materialMap["well"].textures.push_back(textureMap["blank"]);
	materialMap["well"].textures.push_back(textureMap["wellNormal"]);
	materialMap["well"].textures.push_back(textureMap["blank"]);

	materialMap["testNormalMap"] = materialMap["well"];
	materialMap["testNormalMap"].textures[0] = textureMap["testImage"];
	materialMap["testNormalMap"].textures[1] = textureMap["blank"];
	materialMap["testNormalMap"].textures[3] = textureMap["testNormal"];

	materialMap["groundNormalMapped"] = materialMap["well"];
	materialMap["groundNormalMapped"].textures[0] = textureMap["groundDiffuse"];
	materialMap["groundNormalMapped"].textures[1] = textureMap["groundSpec"];
	materialMap["groundNormalMapped"].textures[3] = textureMap["groundNormal"];
	materialMap["groundNormalMapped"].textures[4] = textureMap["sky"];

	//Generate models
	meshMap["Square"] = new Mesh<VertexPositionColor>();
	Geometry::CreateEfficientSquare((Mesh<VertexPositionColor>*)meshMap["Square"], device);
	meshMap["TexturedSquare"] = new Mesh<VertexPosNormTexture>();
	Geometry::CreateTexturedSquare((Mesh<VertexPosNormTexture>*)meshMap["TexturedSquare"], device);
	meshMap["NormalMappedSquare"] = new Mesh<VertexPosTexNormTan>();
	Geometry::CreateNormalMappedSquare((Mesh<VertexPosTexNormTan>*)meshMap["NormalMappedSquare"], device);

	//Import models
	meshMap["Skybox"] = new Mesh<VertexSky>();
	ModelImporter::LoadFromFile("../Assets/Models/invertedcube.obj", (Mesh<VertexSky>*)meshMap["Skybox"], device);
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
	meshMap["Well"] = new Mesh<VertexPosTexNormTan>();
	ModelImporter::LoadFromFileWithTangents("../Assets/Models/well.obj", (Mesh<VertexPosTexNormTan>*)meshMap["Well"], device);
	UINT numModels = 7;

#pragma region New Stuff For After Thanksgiving Break
	//Setup for post processing functionality
	//We will draw to these
	//This creates a texture as opposed to importing one from a .jpg/.png/.dds/etc.
	textureObjects["PostTarget1"] = RenderManager::CreateBlankTexture(device, windowWidth, windowHeight,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS);
	//A texture that will be drawn to and then used to draw with later has to made with these extra 'bind flags'

	//This controls what we are drawing to
	renderTargets["PostTarget1"] = RenderManager::CreateRenderTarget(textureObjects["PostTarget1"], device);

	//We draw FROM these (we sample ShaderResourceViews for values in our shaders)
	textureMap["PostTarget1"] = RenderManager::CreateTexture(textureObjects["PostTarget1"], device);

	PostProcess::screenDepthView = &depthStencilView;//this is the depth buffer we've been drawing to since day 1
	PostProcess::screenView = &renderTargetView;//this is where we've been rendering everything to each frame since day 1
	post1.Add(renderTargets["PostTarget1"]);
	post1.depthStencilView = &depthStencilView;
	gameSimpleCBuffer.postEffect = 1;//Just for cycling through effects for fun.

	//Setup a 2D sprite mesh
	vector<VertexSprite> spriteVerts =
	{
		{ XMFLOAT3(0, 0, 0), 0, 0 },//BL
		{ XMFLOAT3(1, 0, 0), 1, 0 },//BR
		{ XMFLOAT3(1, 1, 0), 1, 1 },//TR
		{ XMFLOAT3(0, 1, 0), 0, 1 }//TL
	};
	vector<UINT> spriteIndices = { 0, 1, 2,//Triangle 1 
		2, 3, 0 };//Triangle 2	 
	meshMap["Sprite"] = RenderManager::CreateMesh<VertexSprite>(spriteVerts, spriteIndices, device);
	Sprite::mesh = meshMap["Sprite"];//static in sprite because all sprites should use this exact same mesh.
	sprite1.scale.x = (float)windowWidth;
	sprite1.scale.y = (float)windowHeight;
	//Sprite Order: 0 = closest to screen, 1 means furthest away. 
		//.2f drawn in front of .3f. 
		//depths < 0 and > 1 are not drawn. float '1' may be stored as 1.0000001 and not draw, so be careful.
		//You can add some code in Sprite.cpp to convert this to whatever ranges you think make more sense, like to match Unity's sprite order.
	sprite1.position.z = .999f;
	materialMap["sprite"] = Material();
	materialMap["sprite"].rasterizerState = rasterizerState;
	materialMap["sprite"].depthState = standardDepthState;
	materialMap["sprite"].blendState = noBlend;//Could use masking or alpha blending, depends on your sprites.
	materialMap["sprite"].vShader = vShaderMap["sprite"];
	materialMap["sprite"].pShader = pShaderMap["sprite"];
	materialMap["sprite"].inputLayout = inputLayoutMap["sprite"];
	//We take the texture we drew to in part of the draw call, mess with it in the sprite shader, 
		//and draw the final adjusted image on this sprite on the screen.
	materialMap["sprite"].textures.push_back(textureMap["PostTarget1"]);
	materialMap["sprite"].samplers.push_back(samplerMap["bilinear"]);
	sprite1.material = &materialMap["sprite"];
#pragma endregion

	//Setup other game settings as needed
	camera.useSpaceCamera = false;
	camera.position = XMFLOAT3(0, 10, -15);
	camera.forward = XMFLOAT3(0, -2, 1);
	sun.direction = XMFLOAT3(0, -1, 1);
	mouse->InitMouseCallback(gameWindow);//Enable hi-res mouse input
	gameSimpleCBuffer.ambientLightIntensity = ambient;

	//Setup game objects (ground, buildings, characters, etc.)
	sky.mesh = meshMap["Skybox"];
	sky.material = &materialMap["sky"];

	//More efficient to allocate ahead of time rather than growing to size slowly.
	opaqueObjects.reserve(100);
	transparentObjects.reserve(100);

	GameObject ground;
	ground.material = &materialMap["groundNormalMapped"];
	ground.mesh = meshMap["NormalMappedSquare"]; 
	//ground.material = &materialMap["ground"];
	//ground.mesh = meshMap["TexturedSquare"];
	ground.rotation.y = XMConvertToRadians(90);
	ground.scale = XMFLOAT3(150, 150, 150);
	ground.uvScale = XMFLOAT2(25, 25);
	opaqueObjects.emplace_back(ground);

	GameObject leaves;
	leaves.material = &materialMap["leaves"];
	leaves.mesh = meshMap["TexturedSquare"]; 
	leaves.position.y += .5f; 
	opaqueObjects.emplace_back(leaves); 

	GameObject well;
	well.material = &materialMap["well"];
	well.mesh = meshMap["Well"];
	well.position.x -= 5;
	opaqueObjects.emplace_back(well);

	GameObject normalMappedSquare;
	normalMappedSquare.material = &materialMap["testNormalMap"];
	normalMappedSquare.mesh = meshMap["NormalMappedSquare"];
	normalMappedSquare.position.x -= 10;
	normalMappedSquare.position.y += .5f;
	opaqueObjects.emplace_back(normalMappedSquare);

	int maxDist = 25;
	for (int i = 0; i < 100; ++i)
	{
		GameObject obj;
		obj.material = &materialMap["lit"];

		//Pick a model
		switch (rand() % numModels)
		{
		case 0:
			obj.mesh = meshMap["Kirby"];
			obj.material = &materialMap["kirby"];
			break;
		case 1:
			obj.mesh = meshMap["Vase"];
			break;
		case 2:
			obj.mesh = meshMap["Barrel"];
			obj.material = &materialMap["barrel"];
			break; 
		case 3:
			obj.mesh = meshMap["Diamond"];
			obj.specIntensity = 2;
			break;
		case 4:
			obj.mesh = meshMap["Potion"];
			break;
		case 5:
			obj.mesh = meshMap["Apple"];
			obj.specIntensity = 4;
			break;
		case 6:
			obj.mesh = meshMap["Sword"];
			break;
		}

		//Set position, scale, rotation, color
		obj.position = Utility::RandomPosition((float)maxDist);
		obj.position.y += maxDist / 2.0f;
		float scale = rand() / (float)RAND_MAX * 3 + 1;//scale as a FLOAT between 1 and 4
		obj.scale = XMFLOAT3(scale, scale, scale);//Uniform Scale
		obj.scale = XMFLOAT3(rand() / (float)RAND_MAX * 3 + 1, rand() / (float)RAND_MAX * 3 + 1, rand() / (float)RAND_MAX * 3 + 1);//Non-Uniform Scale
		obj.SetColor(Utility::RandomColor());
		obj.rotation.x = rand()/(float)RAND_MAX * 360;//Yaw
		opaqueObjects.emplace_back(obj);
	}

	//Make clumps of meshes to demo transparency
	//For optimizing sorting, I changed this to a vector of pointers instead of objects. Notice the differences (new, ->)
		//Using new often is optimized in many game engines by using 'Memory Pooling'
	maxDist = 5;
	for (int i = 0; i < 25; ++i)
	{
		transparentObjects.emplace_back(new GameObject());
		transparentObjects.back()->SetColor(Utility::RandomColor(true));
		transparentObjects.back()->scale = XMFLOAT3(5, 5, 5);
		transparentObjects.back()->mesh = meshMap["Potion"];
		transparentObjects.back()->material = &materialMap["additiveBlending"];
		transparentObjects.back()->position = Utility::RandomPosition((float)maxDist);
		transparentObjects.back()->position.x += 10;
		transparentObjects.back()->position.y += maxDist / 2.0f;
	}
	for (int i = 0; i < 25; ++i)
	{
		transparentObjects.emplace_back(new GameObject());
		transparentObjects.back()->mesh = meshMap["Potion"];
		transparentObjects.back()->SetColor(Utility::RandomColor(true));
		transparentObjects.back()->scale = XMFLOAT3(5, 5, 5);
		transparentObjects.back()->material = &materialMap["alphaBlending"];
		transparentObjects.back()->position = Utility::RandomPosition((float)maxDist);
		transparentObjects.back()->position.x += 20;
		transparentObjects.back()->position.y += maxDist / 2.0f;
	}
	for (int i = 0; i < 25; ++i)
	{
		transparentObjects.emplace_back(new GameObject());
		transparentObjects.back()->mesh = meshMap["Potion"];
		transparentObjects.back()->SetColor(Utility::RandomColor(true), true);
		transparentObjects.back()->scale = XMFLOAT3(5, 5, 5);
		transparentObjects.back()->material = &materialMap["alphaBlending"];
		transparentObjects.back()->position = Utility::RandomPosition((float)maxDist);
		transparentObjects.back()->position.x += 30;
		transparentObjects.back()->position.y += maxDist/2.0f;
	}
	for (int i = 0; i < 25; ++i)
	{
		transparentObjects.emplace_back(new GameObject());
		transparentObjects.back()->mesh = meshMap["Potion"];
		transparentObjects.back()->SetColor(Utility::RandomColor(true));
		transparentObjects.back()->scale = XMFLOAT3(5, 5, 5);
		transparentObjects.back()->material = &materialMap["testBlending"];
		transparentObjects.back()->position = Utility::RandomPosition((float)maxDist);
		transparentObjects.back()->position.x += 40;
		transparentObjects.back()->position.y += maxDist / 2.0f;
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
		speed = 100;

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

	if (keyboard->ButtonDown('Q'))
		camera.rotation.z += 5 * dt;
	if (keyboard->ButtonDown('E'))
		camera.rotation.z -= 5 * dt;  

	if (keyboard->ButtonDown('1'))
		sprite1.position.x += speed * dt;
	if (keyboard->ButtonDown('2'))
		sprite1.position.x -= speed * dt;
	if (keyboard->ButtonDown('3'))
		sprite1.position.y -= speed * dt;
	if (keyboard->ButtonDown('4'))
		sprite1.position.y += speed * dt;
	if (keyboard->ButtonDown('5'))
		sprite1.rotation.z += speed * dt;
	if (keyboard->ButtonDown('6'))
		sprite1.rotation.z -= speed * dt;
	if (keyboard->ButtonDown('7'))
		sprite1.scale.x -= speed * dt;
	if (keyboard->ButtonDown('8'))
		sprite1.scale.x += speed * dt;
	if (keyboard->ButtonDown('9'))
		sprite1.scale.y -= speed * dt;
	if (keyboard->ButtonDown('0'))
		sprite1.scale.y += speed * dt;

	if (keyboard->ButtonPressed(VK_SPACE))
	{
		gameSimpleCBuffer.postEffect++;
		if (gameSimpleCBuffer.postEffect > 8)
			gameSimpleCBuffer.postEffect = 1;
	}

	if (keyboard->ButtonPressed('N'))
		for (GameObject &g : opaqueObjects)
		{
			if (g.material == &materialMap["groundNormalMapped"])//very lazy way to find which is the ground. Could add tags, IDs, etc.
				continue;
			g.acceleration.y = -1;
		}

	if (keyboard->ButtonDown('G'))
	{
		opaqueObjects.emplace_back(GameObject());
		opaqueObjects.back().mesh = meshMap["Kirby"];
		opaqueObjects.back().material = &materialMap["kirby"];
		opaqueObjects.back().position = Utility::RandomPosition(5);
		opaqueObjects.back().position.y += 5;
		opaqueObjects.back().velocity = Utility::RandomPosition(5);//Just random XYZ between -5 and 5
	}
	if (keyboard->ButtonDown('H'))
	{
		transparentObjects.emplace_back(new GameObject());
		transparentObjects.back()->mesh = meshMap["Potion"];
		transparentObjects.back()->material = &materialMap["additiveBlending"];
		transparentObjects.back()->SetColor(Utility::RandomColor(true));
		transparentObjects.back()->position = Utility::RandomPosition(5);
		transparentObjects.back()->position.y += 5;
		transparentObjects.back()->velocity = Utility::RandomPosition(5);//Just random XYZ between -5 and 5
	}
	if (keyboard->ButtonDown('J'))
	{
		sprites.emplace_back(Sprite());
		sprites.back().mesh = meshMap["Sprite"];
		sprites.back().material = &materialMap["sprite"];
		sprites.back().position = XMFLOAT3((float)(rand() % windowWidth), (float)(rand() % windowHeight), Utility::RandomFloat());
		float randScale = (float)rand() / RAND_MAX * .25f;
		sprites.back().scale = XMFLOAT3(randScale * windowWidth, randScale * windowHeight, 1);
	}

	if (keyboard->ButtonDown('L'))
		XMStoreFloat3(&sun.direction, //And finally store the result back in sun.direction
			XMVector3Rotate(XMLoadFloat3(&sun.direction),//Use it to rotate the sun direction (converted to an XMVECTOR)
				XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(90) * dt)));//Create a quaternion (a way of storing rotation) - 90 degrees/sec
	if (keyboard->ButtonDown('K'))
		XMStoreFloat3(&sun.direction, //And finally store the result back in sun.direction
			XMVector3Rotate(XMLoadFloat3(&sun.direction),//Use it to rotate the sun direction (converted to an XMVECTOR)
				XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(90) * dt)));//Create a quaternion (a way of storing rotation) - 90 degrees/sec
	
	//Disable mouse controls if mouse is visible
	if (!mouse->IsVisible())
	{
		//Mouse input doesn't need dt (delta time) since the amount you can physically move it each frame is naturally scaled by the framerate.
		camera.rotation.x += XMConvertToRadians(mouse->DX());
		camera.rotation.y += XMConvertToRadians(mouse->DY());//I added an 'inverted' bool to the mouse class so you don't need to negate this here anymore.
	}

	//toggle mouse visibility. 
	if (keyboard->ButtonPressed(VK_TAB))
		if (mouse->IsVisible())
			mouse->HideAndLockMouse(gameWindow);
		else
			mouse->ShowAndUnlockMouse(gameWindow);
} 

//Update is for movement, AI, physics, most gameplay things.
void ExampleGame::Update(float dt)
{
	if (dt > 50)
		cout << "Framerate: " << 1000.0f/dt << "! If your framerate is this low you are doing something wrong!!!" << endl;

	if (!mouse->IsVisible())
		mouse->CenterCursor();//Only do this after using the mouse's current position/dx/dy, etc. this frame.
	
	gameSimpleCBuffer.gameTime = timer.TotalGameTime();
	sun.Update();
	camera.Update();
	sky.position = camera.position;

	for (auto it = opaqueObjects.begin(); it != opaqueObjects.end();)
	{
		it->Update(dt);
		
		if (XMVector3LengthEst(XMLoadFloat3(&it->position)).m128_f32[0] > 50)//If object is more than 50 units from (0,0,0)
			it = opaqueObjects.erase(it);//moves it to the next object after the erased one.
		else
			++it;//If we didn't have this in 'else' you'd skip any element immediately following a deleted one.

		//!!!Important!!! Notice how to handle a loop differently if you are deleting some elements as you go.
			//Also, this could be done more efficiently; erasing in the middle of a vector is expensive.
			//A linked list would be better if we don't add other effects that require accessing elements in the middle
			//Or most games use more complex memory pooling - like a vector of 'reusable' objects and we just mark them as 'deleted' but don't actually erase them.
	}

	for (auto it = transparentObjects.begin(); it != transparentObjects.end();)
	{
		(*it)->Update(dt);

		if (camera.bounds.Intersects((*it)->GetBoundingSphere()))
			cout << "Camera colliding with object. dt: " << dt << "\n";

		if (XMVector3LengthEst(XMLoadFloat3(&(*it)->position)).m128_f32[0] > 50)//If object is more than 50 units from (0,0,0)
			it = transparentObjects.erase(it);
		else
			++it;
	}	 
}

//Clear() resets the backbuffer texture so that we are drawing to a blank screen
void ExampleGame::Clear()
{
	Material::Reset();
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
	Clear();//Clear the buffer (otherwise we'll see what was drawn on it before)
	
	post1.Set(deviceContext);

	//Setup for drawing
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//every 3 vertices = 1 triangle.
	deviceContext->VSSetConstantBuffers(0, 1, &gpuSimpleCBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &gpuSimpleCBuffer); 

	//Draw opaque objects first
	for (GameObject &g : opaqueObjects)
		g.Draw(); 

	//Then draw background/skybox
	sky.Draw();
		
	//Then draw transparent objects last in back-to-front order. 
	camera.DepthSort(transparentObjects);
	for (GameObject *&g : transparentObjects)
		g->Draw(); 

	post1.UnSet(deviceContext);//Now we draw to the backbuffer normally, and it will be put on the screen on the Present() call like usual.

	sprite1.Draw();
	for (Sprite &s : sprites)
		s.Draw();

	//Draw 2D UI/sprites after all 3D game elements
	//Draw text to the screen
	//spriteBatch->Begin();
	//{
	//	float offset = spriteFont->MeasureString(L"ABC").m128_f32[1];//Get how tall the previous text was. Includes line spacing.
	//	spriteFont->DrawString(spriteBatch, std::wstring(L"FPS: " + std::to_wstring((int)frameRate)).c_str(), XMFLOAT2(0, 0), XMVectorSet(1, 0, 0, 1));
	//	spriteFont->DrawString(spriteBatch, L"Toggle Mouse: Tab", XMFLOAT2(0, offset * 1), XMVectorSet(1, 0, 0, 1));
	//}
	//spriteBatch->End();//Text doesn't actually get drawn to the screen until here

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
	gameSimpleCBuffer.screenSize.x = (float)windowWidth;
	gameSimpleCBuffer.screenSize.y = (float)windowHeight;

#pragma region New Stuff For After Thanksgiving Break
	//Static variables in a function are strange...
	//Basically this line will run the first time it gets hit, 
	//but after that the variable will just stay in memory for good,
	//and this line will be ignored after its first run (it won't re-initalize to true)
	//Don't have to do this this way, just showing you its a thing.
	static bool firstResize = true;

	if (!firstResize)
	{
		//Recreate everything for post processing to work with the new screen size.
		//This is why a long long time ago we set it up so the game doesn't render during a resize.
		textureObjects["PostTarget1"]->Release();
		textureObjects["PostTarget1"] = RenderManager::CreateBlankTexture(device, windowWidth, windowHeight,
			D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS);

		renderTargets["PostTarget1"]->Release();
		renderTargets["PostTarget1"] = RenderManager::CreateRenderTarget(textureObjects["PostTarget1"], device);

		textureMap["PostTarget1"]->Release();
		textureMap["PostTarget1"] = RenderManager::CreateTexture(textureObjects["PostTarget1"], device);
		//Because the address of the 'PostTarget1' texture changed, the material(s) using it need to get the new address
		//this is called Pointer Invalidation; until we fix it they are 'Dangling Pointers' to the old, now deleted address
		//It could be solved instead by storing a double pointer (ID3D11ShaderResourceView**) in the material instead of a normal one.
			//That way the double pointer points to the address of the pointer that will store the address of the texture. 
			//If the texture's address changes, the pointer pointing to it STORES a new address, but that pointers own address didn't change
			//so this object will still be able to access the object at its new address by 'asking' the 2nd pointer where the new address is. 
			//Think PO Box. If people send mail directly and you move, they'd have to be told where you moved. But if you have a PO box, you can move
			//and they can still just send mail to the same PO box they already knew about.
			//This is also called adding a 'level of indirection' (to add to your technical jargon vocabulary)
		sprite1.material->textures[0] = textureMap["PostTarget1"];

		post1.renderTargets.clear();
		post1.Add(renderTargets["PostTarget1"]);

		sprite1.scale.x = (float)windowWidth;
		sprite1.scale.y = (float)windowHeight;
	}

	firstResize = false;
#pragma endregion

	Globals::Get().SetAspectRatio(AspectRatio());
}

//The ~Game() destructor is for releasing and deleting all our objects
//Remember Release is for DirectX objects, and delete is for pointers that use the 'new' keyword
ExampleGame::~ExampleGame()
{
	//Delete our things
	SafeDelete(spriteFont);
	SafeDelete(spriteBatch); 

	SafeDeleteMap(meshMap);

	//Notice we used new to make these objects, so we have to delete them here
	//Why don't we have to delete the other objects here, even though we can still make more of those at runtime too?
	for (GameObject* p : transparentObjects)
		delete p;

	//Release DirectX Things
	//Reminder: the goal is to see a refcount of 2 when
		//you run in debug mode and look at the output log
		//the last 2 objects are the device and debug object, 
		//they'll release in ~BaseGame
	SafeRelease(rasterizerState);
	SafeRelease(wireframeRS);
	SafeRelease(twoSidedRS);

	SafeRelease(standardDepthState);
	SafeRelease(skyDepthState);
	SafeRelease(noDepthState); 
	SafeRelease(transparentDepthState); 

	SafeRelease(standardBlend);
	SafeRelease(additiveBlend);
	SafeRelease(noBlend);
	SafeRelease(testBlend);

	SafeRelease(gpuSimpleCBuffer);

	SafeReleaseMap(textureMap);
	SafeReleaseMap(samplerMap);
	SafeReleaseMap(inputLayoutMap);
	SafeReleaseMap(vShaderMap);
	SafeReleaseMap(pShaderMap);
	SafeReleaseMap(textureObjects);
	SafeReleaseMap(renderTargets);
}
#pragma endregion