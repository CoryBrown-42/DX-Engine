#pragma once

#include <DirectXMath.h>

#include "ConstantBufferDefinitions.h"
#include "Mesh.h" 
#include "Globals.h"
#include "Material.h"

using namespace std;

class GameObject
{
	XMFLOAT4 color; //use SetColor function instead so it can be premultiplied by alpha if desired
	BoundingBox boundingBox;
	BoundingSphere boundingSphere;

public:
	BaseMesh* mesh = nullptr;
	Material* material = nullptr;
	
	XMFLOAT4X4 transform;//Transformation Matrix - stores position, rot, scale. Also usually called this object's 'world' matrix
	XMFLOAT3 position, forward, up, rotation, scale, velocity, acceleration;
	XMFLOAT2 uvScale;
	float specIntensity, depthFromCamera;

	GameObject()
		: position(0, 0, 0),
		rotation(0, 0, 0),
		forward(0, 0, 1),
		up(0, 1, 0),
		uvScale(1, 1),
		scale(1, 1, 1),
		velocity(0, 0, 0),
		acceleration(0, 0, 0),
		color(1, 1, 1, 1),
		specIntensity(64)
	{

	}

	void SetColor(XMFLOAT4 col, bool premultiplyAlpha = false)
	{
		color = col;
		if (premultiplyAlpha)
		{
			color.x *= color.w;
			color.y *= color.w;
			color.z *= color.w;
		}
	}

	void Update(float dt)
	{
		//AI Movement, physics (velocity/gravity), color changes, etc. could go here.
		XMVECTOR a = XMLoadFloat3(&acceleration);
		XMVECTOR v = XMLoadFloat3(&velocity);
		XMVECTOR p = XMLoadFloat3(&position);
		v += a * dt;
		p += v * dt;
		XMStoreFloat3(&velocity, v);
		XMStoreFloat3(&position, p);
	}

	BoundingBox GetBoundingBox()
	{
		boundingBox = mesh->boundingBox;
		boundingBox.Center = Utility::Add(boundingBox.Center, position);
		boundingBox.Extents.x *= scale.x;
		boundingBox.Extents.y *= scale.y;
		boundingBox.Extents.z *= scale.z;
		return boundingBox;
	}

	BoundingSphere GetBoundingSphere()
	{
		boundingSphere = mesh->boundingSphere;
		boundingSphere.Center = Utility::Add(boundingBox.Center, position);
		boundingSphere.Radius *= max(max(scale.x, scale.y), scale.z);
		return boundingSphere;
	}

	void Draw()
	{
		if (!material || !mesh)
		{
			cout << "Error, gameobject missing mesh or material." << endl;
			return;
		}

		//Set CBuffer Data
		Globals::Get().GetGameCBuffer()->objectColor = color;
		Globals::Get().GetGameCBuffer()->objectSpecularity = specIntensity;
		Globals::Get().GetGameCBuffer()->uScale = uvScale.x;
		Globals::Get().GetGameCBuffer()->vScale = uvScale.y;

		XMMATRIX world = XMMatrixRotationRollPitchYaw(rotation.y, rotation.x, rotation.z)
			* XMMatrixScaling(scale.x, scale.y, scale.z) 
			* XMMatrixTranslation(position.x, position.y, position.z);
		XMStoreFloat4x4(&Globals::Get().GetGameCBuffer()->objectTransform, XMMatrixTranspose(world));

		XMMATRIX temp = world;
		temp.r[3] = XMVectorSet(0, 0, 0, 1);//zero out its translation
		XMVECTOR determinant = XMMatrixDeterminant(temp);
		XMStoreFloat4x4(&Globals::Get().GetGameCBuffer()->inverseTranspose, XMMatrixInverse(&determinant, temp));
			
		//Send CBuffer Data (from GameCBuffer to GPUCBuffer)
		Globals::Get().GetDeviceContext()->UpdateSubresource(
			Globals::Get().GetGPUCBuffer(),
			0, NULL,
			Globals::Get().GetGameCBuffer(),
			0, 0); 
		
		//Draw 
		material->Apply();
		mesh->Draw();
	}
};