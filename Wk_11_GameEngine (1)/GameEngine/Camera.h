#pragma once

#include <DirectXMath.h>

#include "ConstantBufferDefinitions.h"

using namespace std;

class Camera
{
public:
	XMFLOAT3 position, forward, up, right, rotation;
	float totalPitch, maxPitch;
	XMFLOAT4X4 view, projection;
	bool useSpaceCamera = false;

	Camera()//Constructor gets called automatically when we create an object
		: position(0, 0, -3),
		rotation(0, 0, 0),
		forward(0, 0, 1),
		right(1, 0, 0),
		up(0, 1, 0),
		totalPitch(0),
		maxPitch(XMConvertToRadians(89.9f))
	{

	}

	void MoveForward(float amount)
	{
		position.x += forward.x * amount;
		position.y += forward.y * amount;
		position.z += forward.z * amount;
	}

	void Strafe(float amount)
	{
		position.x += right.x * amount;
		position.y += right.y * amount;
		position.z += right.z * amount;
	}

	void Update()
	{
		//Load XMVECTORS for doing efficient math things.
		XMVECTOR forwardVec = XMLoadFloat3(&forward);
		XMVECTOR rightVec = XMLoadFloat3(&right);
		XMVECTOR upVec = XMLoadFloat3(&up);

		if (!useSpaceCamera)//Rotating in a FPS/RPG type game (on a planet with a horizon) - up is always global up axis.
		{
			//Constrain pitch so we can't flip over upside down
			if (totalPitch + rotation.y > maxPitch)
			{
				rotation.y = maxPitch - totalPitch;//Rotate the rest of the way to maxPitch
				totalPitch = maxPitch;
			}
			else if (totalPitch + rotation.y < -maxPitch)
			{
				rotation.y = -maxPitch - totalPitch;
				totalPitch = -maxPitch;
			}
			else
				totalPitch += rotation.y;
			
			XMMATRIX yawRotMatrix = XMMatrixRotationY(rotation.x);
			rightVec = XMVector3Transform(rightVec, yawRotMatrix);//Yaw right vector around global y/up vector
			forwardVec = XMVector3Transform(forwardVec, yawRotMatrix);//Yaw forward vector around global y vector
			forwardVec = XMVector3Transform(forwardVec, XMMatrixRotationAxis(rightVec, rotation.y));//pitch forward vector around local right vector
		}
		else//Rotate all 3 axis so we can go completely upside down
		{
			//Yaw first around current (last frame's) local up vector
			XMMATRIX rotMatrix = XMMatrixRotationAxis(upVec, rotation.x);
			rightVec = XMVector3Transform(rightVec, rotMatrix);
			forwardVec = XMVector3Transform(forwardVec, rotMatrix);

			//Then Pitch around new local right vector
			rotMatrix = XMMatrixRotationAxis(rightVec, rotation.y);
			forwardVec = XMVector3Transform(forwardVec, rotMatrix);
			upVec = XMVector3Transform(upVec, rotMatrix);

			//Then Roll around new local forward vector
			rotMatrix = XMMatrixRotationAxis(forwardVec, rotation.z);
			rightVec = XMVector3Transform(rightVec, rotMatrix);
			upVec = XMVector3Transform(upVec, rotMatrix);
		}

		//Store results.
		XMStoreFloat3(&forward, forwardVec);
		XMStoreFloat3(&right, rightVec);
		XMStoreFloat3(&up, upVec);
		
		rotation.x = rotation.y = rotation.z = 0;//Reset for next frame

		//The view matrix represents where our camera is.
		XMStoreFloat4x4(&view, XMMatrixLookAtLH(
			XMLoadFloat3(&position),				//Position of the camera, our 'eye' in the game
			XMLoadFloat3(&position) + forwardVec,	//Where the camera is looking at
			upVec));								//The direction that is 'up' relative to the camera.

		//The projection matrix represents how our camera sees the world.
		XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovLH(
			XMConvertToRadians(45),
			Globals::Get().GetAspectRatio(),
			.1f,//Near plane
			1000));//Far plane

		//XMStoreFloat4x4(&projection, XMMatrixOrthographicLH(
		//	80,
		//	65,
		//	.1f,//Near plane
		//	1000));//Far plane

		XMStoreFloat4x4(&Globals::Get().GetGameCBuffer()->viewProjection,
			XMMatrixTranspose(XMLoadFloat4x4(&view) * XMLoadFloat4x4(&projection)));

		Globals::Get().GetGameCBuffer()->cameraPosition = position;

		//Just copies the game's constant buffer data over to the GPU's constant buffer 
		Globals::Get().GetDeviceContext()->UpdateSubresource(
			Globals::Get().GetGPUCBuffer(),
			0, NULL,
			Globals::Get().GetGameCBuffer(),
			0, 0);
	}
};