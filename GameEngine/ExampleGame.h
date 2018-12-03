#pragma once

#include <DirectXMath.h>
#include <d3dcompiler.h> 
#include <vector>
#include <unordered_map>//Hash Table

#include "Mesh.h"
#include "GameObject.h"
#include "Camera.h"
#include "BaseGame.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "Light.h"
#include "ConstantBufferDefinitions.h"
#include "VertexDefinitions.h"
#include "PostProcess.h"
#include "Sprite.h"

/* This represents a particular game and its entities, meshes, shaders, etc.  Just remember that the Program
actually starts in main.cpp and the Game Loop is defined in BaseGame.cpp */
class ExampleGame final : public BaseGame
{
public:
	ExampleGame(HINSTANCE instance);
 	~ExampleGame();
	bool Init() override;
	void GetInput(float dt) override;
	void Update(float dt) override;
	void Clear() override;
	void Draw() override;
	void OnResize() override;
	void OnFocus() override;

private:
	//Assets
	SpriteFont *spriteFont = nullptr;
	SpriteBatch *spriteBatch = nullptr;
	unordered_map<string, ID3D11ShaderResourceView*> textureMap;
	unordered_map<string, ID3D11SamplerState*> samplerMap;
	unordered_map<string, ID3D11InputLayout*> inputLayoutMap;
	unordered_map<string, ID3D11VertexShader*> vShaderMap;
	unordered_map<string, ID3D11PixelShader*> pShaderMap;
	unordered_map<string, Material> materialMap;
	unordered_map<string, BaseMesh*> meshMap;
	
	//Render States and Objects
	ID3D11RasterizerState *rasterizerState = nullptr, *wireframeRS = nullptr, *twoSidedRS;
	ID3D11BlendState *standardBlend = nullptr, *additiveBlend = nullptr, *noBlend = nullptr, *testBlend = nullptr;
	ID3D11DepthStencilState *standardDepthState = nullptr, *skyDepthState = nullptr, *noDepthState = nullptr, *transparentDepthState;
	SimpleConstantBuffer gameSimpleCBuffer;//constant buffer FOR Game
	ID3D11Buffer *gpuSimpleCBuffer = nullptr;//constant buffer FOR GPU
	Camera camera;
	DirectionalLight sun;


	PointLight pointLight;

	//Other Settings
	float clearcolor[4];
	float ambient = .025f;
	
	//GameObjects
	GameObject sky;
	vector<GameObject> opaqueObjects;
	vector<GameObject*> transparentObjects;//Significantly more efficient to sort pointers than objects (much less copying)
	vector<Sprite> sprites;

	//Post Processing
	PostProcess post1;
	Sprite sprite1;
	unordered_map<string, ID3D11Texture2D*> textureObjects;
	unordered_map<string, ID3D11RenderTargetView*> renderTargets;
};