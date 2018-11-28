#ifndef POSTPROCESS
#define POSTPROCESS

#include <d3d11.h>
#include <vector>

#include "Sprite.h"
#include "Utility.h"
 
/*
PostProcess contains everything necessary to create a single PostProcess effect.

Currently it performs two tasks which may be refactored into two separate classes.
A PostProcess can set the current rendertarget so subsequent draw calls are drawn to that target,
or a post process can draw its own sprite with a PostProcess effect to its rendertarget.
*/
class PostProcess
{
public://This shouldn't all be public but I've got other classes to prepare for :)
	static float clearcolor[4];
	static ID3D11RenderTargetView **screenView;
	static ID3D11DepthStencilView **screenDepthView;

	std::vector<ID3D11RenderTargetView*> renderTargets;
	ID3D11DepthStencilView **depthStencilView;//This will generally always be same as screenDepthView - some effects may use it
	Sprite *screenSprite;

	//unset any rendertargets so subsequent draw calls draw to the screen
	inline static void UnSet(ID3D11DeviceContext *deviceContext)
	{		
		deviceContext->ClearDepthStencilView(*screenDepthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);	
		deviceContext->OMSetRenderTargets(1, screenView, *screenDepthView);
	}

	//Constructor for PostProcess that will only set the render target
	PostProcess()
		: screenSprite(nullptr)
	{

	}

	//Constructor for PostProcess that will draw its own effect to a render target
	PostProcess(UINT width, UINT height)
		: screenSprite(new Sprite())
	{ 
		screenSprite->scale.x = (float)width;
		screenSprite->scale.y = (float)height;
		screenSprite->position.y = .5f;
	}

	//Destructor
	~PostProcess()
	{
		if(screenSprite != nullptr)
			SafeDelete(screenSprite); 
	}

	//Add a rendertarget to be used in the post process effect
	void Add(ID3D11RenderTargetView *r)
	{
		renderTargets.push_back(r); 
	}

	//Set the rendertarget(s) of this PostProcess to the GPU
	void Set(ID3D11DeviceContext *deviceContext) const
	{
		for(unsigned int i = 0; i < renderTargets.size(); ++i)
			deviceContext->ClearRenderTargetView(renderTargets[i], clearcolor);
		deviceContext->OMSetRenderTargets(renderTargets.size(), renderTargets.data(), *depthStencilView);
	}

	void Reset(ID3D11DeviceContext *deviceContext) const
	{
		deviceContext->ClearDepthStencilView(*depthStencilView,	D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);	
	}

	//Draw the post process applying it's particular effect
	void Draw(ID3D11DeviceContext *deviceContext)
	{
		Set(deviceContext);
		screenSprite->Draw(); 
	}
};
#endif