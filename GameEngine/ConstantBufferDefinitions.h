#pragma once

//Constant Buffers just store and transfer data to the GPU
//that doesn't go in vertices.
struct SimpleConstantBuffer
{
	//Camera
	XMFLOAT4X4 viewProjection;//Represents position, rotation, field of view of our eye (camera) in the virtual world
	
	//For objects
	XMFLOAT4X4 objectTransform;//Represents position, rotation, scale of a game object
	XMFLOAT4X4 inverseTranspose;//The inverse-transpose of the object transform matrix is required for shading objects with non-uniform scaling
	XMFLOAT4 objectColor;

	//For a directional light
	XMFLOAT3 lightDirection;
	float lightIntensity;
	XMFLOAT3 lightColor;
	float ambientLightIntensity;

	XMFLOAT3 cameraPosition;
	float objectSpecularity;

	float uOffset = 0;
	float vOffset = 0;
	float uScale = 1;
	float vScale = 1;

	XMFLOAT2 screenSize;
	float gameTime;
	UINT postEffect;

	XMFLOAT2 spriteTexPos;
	XMFLOAT2 spriteTexSize;
	float spriteDepth;
};