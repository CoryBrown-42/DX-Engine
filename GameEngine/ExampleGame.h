#pragma once

#include <DirectXMath.h>
#include <d3dcompiler.h> 
#include <vector>//NOTHING TO DO WITH XMVECTOR - this is C++'s Smart Array, equivalent to C#/Java's 'List'
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

/*
This represents a particular game and its entities, meshes, shaders, etc.  Just remember that the Program
actually starts in main.cpp and the Game Loop is defined in BaseGame.cpp
*/
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
	float clearcolor[4];
	bool useWireframe = false;

	SpriteFont *spriteFont;
	SpriteBatch *spriteBatch;

	unordered_map<string, ID3D11ShaderResourceView*> textureMap;
	unordered_map<string, ID3D11SamplerState*> samplerMap;

	unordered_map<string, ID3D11InputLayout*> inputLayoutMap;
	unordered_map<string, ID3D11VertexShader*> vShaderMap;
	unordered_map<string, ID3D11PixelShader*> pShaderMap;
	unordered_map<string, BaseMesh*> meshMap;
	ID3D11RasterizerState *rasterizerState, *wireframeRS;
	ID3D11DepthStencilState *depthState;
	SimpleConstantBuffer gameSimpleCBuffer;//constant buffer FOR Game
	ID3D11Buffer *gpuSimpleCBuffer = nullptr;//constant buffer FOR GPU
	DirectionalLight sun;
	Camera camera;
	vector<GameObject> gameObjects;
	vector<GameObject> barrels;
	vector<GameObject> kirbies;
	float ambient = .025f;
	float showNormals = 0;

	int whichSampler = 0;
};