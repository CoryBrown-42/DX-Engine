#pragma once

#include <vector>
#include <set>

#include "Globals.h"

class Material
{
	static vector<ID3D11ShaderResourceView*> appliedTextures;
	static vector<ID3D11SamplerState*> appliedSamplers;
	static Material* appliedMaterial;
	static ID3D11PixelShader* appliedPShader;
	static ID3D11VertexShader* appliedVShader;
	static ID3D11InputLayout* appliedInputLayout;
	static ID3D11DepthStencilState* appliedDepthState;
	static ID3D11BlendState* appliedBlendState;
	static ID3D11RasterizerState* appliedRasterizerState;

public:
	static void Init()
	{
		appliedTextures.resize(16);//sizes here are arbitrary
		appliedSamplers.resize(8);
	}

	static void Reset()
	{
		//This wouldn't be necessary except the DXTK's Spritebatch code sets shaders/textures too. But its cheap.
		//One could always edit the DXTK code instead since we have access to it...
		appliedTextures[0] = nullptr;//This will 'force/trick' our game objects to set a texture in this slot next time one gets drawn.
		appliedSamplers[0] = nullptr;
		appliedPShader = nullptr;
		appliedVShader = nullptr;
		appliedInputLayout = nullptr;
		appliedDepthState = nullptr;
		appliedBlendState = nullptr;
		appliedRasterizerState = nullptr;
	}

	vector<ID3D11ShaderResourceView*> textures;
	vector<ID3D11SamplerState*> samplers;
	ID3D11DepthStencilState* depthState = nullptr;
	ID3D11BlendState* blendState = nullptr;
	ID3D11RasterizerState* rasterizerState = nullptr;
	ID3D11PixelShader* pShader = nullptr;
	ID3D11VertexShader* vShader = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;
	bool needsSorting = false;

	void Apply()
	{
		//Optimization: Ideally your objects should be sorted by material, then by texture, so that this code is excecuted as little as possible per frame.
		if (appliedMaterial == this)
			return;

		if (!vShader || !pShader || !inputLayout)
		{
			cout << "Error: All materials must have at least a vertex shader, pixel shader, and input layout.\n";
			return;
		}

		//Apply shader if not already applied
		if (appliedVShader != vShader)
		{
			Globals::Get().GetDeviceContext()->VSSetShader(vShader, nullptr, 0);
			appliedVShader = vShader;
		}

		//Apply input layout if not already applied
		if (appliedInputLayout != inputLayout)
		{
			Globals::Get().GetDeviceContext()->IASetInputLayout(inputLayout);
			appliedInputLayout = inputLayout;
		}
		
		//Apply shader if not already applied
		if (appliedPShader != pShader)
		{
			Globals::Get().GetDeviceContext()->PSSetShader(pShader, nullptr, 0);
			appliedPShader = pShader;
		}

		//Apply shader if not already applied
		if (depthState && appliedDepthState != depthState)
		{
			Globals::Get().GetDeviceContext()->OMSetDepthStencilState(depthState, 0);
			appliedDepthState = depthState;
		}

		//Apply shader if not already applied
		if (rasterizerState && appliedRasterizerState != rasterizerState)
		{
			Globals::Get().GetDeviceContext()->RSSetState(rasterizerState);
			appliedRasterizerState = rasterizerState;
		}

		//Apply shader if not already applied
		if (blendState && appliedBlendState != blendState)
		{
			Globals::Get().GetDeviceContext()->OMSetBlendState(blendState, NULL, 0xffffff);
			appliedBlendState = blendState;
		}

		//Apply textures if not already applied. Texture Slots are in order textures are added to this material
		//Optimization: You could keep frequently used textures applied only until you change 'zones', levels, etc. instead of applying them per-object, per-frame
		//Optimization: You should use as few textures as possible by using Texture Atlases/Sprite Sheets,
			//and sort your objects by texture so they don't get applied more than once per frame.
			//too many, too large textures sent over (to the gpu) too frequently is a common performance drain in many games
		for (UINT i = 0; i < textures.size(); ++i)
			if (appliedTextures[i] != textures[i])
			{
				Globals::Get().GetDeviceContext()->PSSetShaderResources(i, 1, &textures[i]);
				appliedTextures[i] = textures[i];
			}

		//Apply sampler if not already applied. Sampler Slots are in order samplers are added to this material 
		//Optimization: Because there are so few settings to change on samplers (just 4 common filter modes and 4 address modes), they are often just sent 
			//over once per game (init function) and you hardcode which to use in your shaders by specifying which sampler slot to use in each one.
		for (UINT i = 0; i < samplers.size(); ++i)
			if (appliedSamplers[i] != samplers[i])
			{
				Globals::Get().GetDeviceContext()->PSSetSamplers(i, 1, &samplers[i]);
				appliedSamplers[i] = samplers[i];
			}

		appliedMaterial = this;
	}
};