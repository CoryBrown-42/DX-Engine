#include "Sprite.h"

BaseMesh* Sprite::mesh = nullptr;

//Draw draws a sprite with the sprite mesh and its material and transform
//Packing and instancing is more appropriate for sprites in the future
void Sprite::Draw()
{
	XMMATRIX worldMatrix = XMMatrixAffineTransformation2D(
		XMLoadFloat3(&scale),//z scale ignored
		XMVectorSet(scale.x/2, scale.y/2, 0, 0),//sprite pivot point
		rotation.z,
		XMLoadFloat3(&position));//z position ignored

	XMStoreFloat4x4(&Globals::Get().GetGameCBuffer()->objectTransform, XMMatrixTranspose(worldMatrix));

	Globals::Get().GetGameCBuffer()->spriteTexPos = XMFLOAT2(dimensions.x, dimensions.y);
	Globals::Get().GetGameCBuffer()->spriteTexSize = XMFLOAT2(dimensions.z, dimensions.w);
	Globals::Get().GetGameCBuffer()->spriteDepth = position.z;
	Globals::Get().GetGameCBuffer()->objectColor = color;

	//Just for fun.
	bool changedPostEffect = false;
	if (Globals::Get().GetGameCBuffer()->postEffect == 8)
	{
		changedPostEffect = true;
		Globals::Get().GetGameCBuffer()->postEffect = postEffect;
	}
		
	//Send CBuffer Data (from GameCBuffer to GPUCBuffer)
	Globals::Get().GetDeviceContext()->UpdateSubresource(
		Globals::Get().GetGPUCBuffer(),
		0, NULL,
		Globals::Get().GetGameCBuffer(),
		0, 0);

	//Draw 
	material->Apply();
	mesh->Draw();

	//Just for fun.
	if (changedPostEffect)
		Globals::Get().GetGameCBuffer()->postEffect = 8;
}