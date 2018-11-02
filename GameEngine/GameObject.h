#pragma once

#include <DirectXMath.h>

#include "ConstantBufferDefinitions.h"
#include "Mesh.h" 
#include "Globals.h"

using namespace std;

class GameObject
{
public:
	XMFLOAT3 position, forward, up, rotation, scale;
	XMFLOAT4X4 transform;//Transformation Matrix - stores position, rot, scale. Also usually called this object's 'world' matrix
	XMFLOAT4 color;  
	float specIntensity;
	BaseMesh* mesh = nullptr;

	GameObject()
		: position(0, 0, 0),
		rotation(0, 0, 0),
		forward(0, 0, 1),
		up(0, 1, 0),
		scale(1, 1, 1),
		color(1, 1, 1, 1),
		specIntensity(8)
	{

	}

	float Distance(XMFLOAT3 d)
	{

	}

	void MoveTo(XMFLOAT3 move, XMFLOAT3 speed)
	{

	}

	void Update()
	{
		//AI Movement, physics (velocity/gravity), color changes, etc. could go here.
	}

	void Draw()
	{		
		//Set CBuffer Data
		Globals::Get().GetGameCBuffer()->objectColor = color;
		Globals::Get().GetGameCBuffer()->objectSpecularity = specIntensity;

		XMStoreFloat4x4(&Globals::Get().GetGameCBuffer()->objectTransform,//Store result in cbuffer transform matrix
			XMMatrixTranspose(//Flip the matrix rows and columns because DirectX
				XMMatrixRotationRollPitchYaw(rotation.y, rotation.x, rotation.z)//Multiply rotation first...
				* XMMatrixScaling(scale.x, scale.y, scale.z)//...Scale 2nd...
				* XMMatrixTranslation(position.x, position.y, position.z)));//... and Position last to properly create transform matrix

		//Send CBuffer Data (from GameCBuffer to GPUCBuffer)
		Globals::Get().GetDeviceContext()->UpdateSubresource(
			Globals::Get().GetGPUCBuffer(),
			0, NULL,
			Globals::Get().GetGameCBuffer(),
			0, 0); 
		
		//Draw
		if(mesh)
			mesh->Draw(Globals::Get().GetDeviceContext());
	}
};