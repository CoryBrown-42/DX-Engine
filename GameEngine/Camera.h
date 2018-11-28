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
	BoundingSphere bounds;

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

	void DepthSort(vector<GameObject*> &vec)
	{
		for(size_t i = 0; i < vec.size(); ++i)//precalc distances so they never get calc'd more than once per object.
			vec[i]->depthFromCamera = XMVector3LengthSq(XMLoadFloat3(&vec[i]->position) - XMLoadFloat3(&position)).m128_f32[0];

		sort(vec.begin(), vec.end(), [this](GameObject *a, GameObject *b)//Anonymous/Lambda function
		{
			//skip if possible (additive blending for instance). This will avoid some swapping.
				//This extra if statement may not even be worth it if sorting pointers instead of whole objects; benchmarking is needed to compare.
			if (!a->material->needsSorting && !b->material->needsSorting)
				return false;

			return a->depthFromCamera > b->depthFromCamera;
		});
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

	//Normalizes basis vectors (X, Y, Z) and guarantees that they are all perfectly perpendicular. forward vec is normalized but not changed in direction
	//Gram-Schmidt algorithm: Arfken, G. "Gram-Schmidt Orthogonalization." §9.3 in Mathematical Methods for Physicists, 3rd ed. Orlando, FL: Academic Press, pp. 516-520, 1985. 
	static void Orthonormalize(XMVECTOR &fVec, XMVECTOR &rVec, XMVECTOR &uVec)
	{ 
		XMVECTOR newRVec = rVec - (XMVector3Dot(rVec, fVec) / XMVector3LengthSq(fVec) * fVec);
		XMVECTOR newUVec = uVec - (XMVector3Dot(uVec, fVec) / XMVector3LengthSq(fVec) * fVec) 
								- (XMVector3Dot(uVec, newRVec) / XMVector3LengthSq(newRVec) * newRVec);
		
		XMVector3NormalizeEst(fVec);
		XMVector3NormalizeEst(newRVec);
		XMVector3NormalizeEst(newUVec);
	}

	void RecalculatedUpdate()
	{
		//Notice these always start with the global axiis
		//We could do this the 'continuous' method
		//This is preferrable because we won't accumulate float-precision errors in our rotations.
		//But this won't work for 'free' rotation (like a spaceship) where different 
		//combinations of yaw/pitch/roll can get you to the same orientation
		XMVECTOR forwardVec = XMVectorSet(0, 0, 1, 0);
		XMVECTOR rightVec = XMVectorSet(1, 0, 0, 0);
		XMVECTOR upVec = XMVectorSet(0, 1, 0, 0);
		
		//Constrain pitch so we can't flip over upside down 
		if (rotation.y > maxPitch)
			rotation.y = maxPitch;
		else if (rotation.y < -maxPitch)
			rotation.y = -maxPitch;

		XMVECTOR yawRotQuaternion = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), rotation.x);
		rightVec = XMVector3Rotate(rightVec, yawRotQuaternion);//Yaw right vector around global y/up vector
		forwardVec = XMVector3Rotate(forwardVec, yawRotQuaternion);//Yaw forward vector around global y vector
		forwardVec = XMVector3Rotate(forwardVec, XMQuaternionRotationAxis(rightVec, rotation.y));//pitch forward vector around local right vector
		
		//Gimbal Lock.
		//XMVECTOR rollQuat = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), rotation.z);
		//upVec = XMVector3Rotate(upVec, rollQuat);

		//Store results.
		XMStoreFloat3(&forward, forwardVec);
		XMStoreFloat3(&right, rightVec);
		XMStoreFloat3(&up, upVec);

		XMVECTOR posVec = XMLoadFloat3(&position);

		//The view matrix represents where our camera is.
		XMStoreFloat4x4(&view, XMMatrixLookAtLH(
			posVec,					//Position of the camera, our 'eye' in the game
			posVec + forwardVec,	//Where the camera is looking at
			upVec));				//The direction that is 'up' relative to the camera. 
	}

	void ContinuousUpdate()
	{
		//Load XMVECTORS for doing efficient math things.
		XMVECTOR forwardVec = XMLoadFloat3(&forward);
		XMVECTOR rightVec = XMLoadFloat3(&right);
		XMVECTOR upVec = XMLoadFloat3(&up); 

		//Yaw first around current (last frame's) local up vector
		XMVECTOR rotQuat = XMQuaternionRotationAxis(upVec, rotation.x);
		rightVec = XMVector3Rotate(rightVec, rotQuat);
		forwardVec = XMVector3Rotate(forwardVec, rotQuat);

		//Then Pitch around new local right vector
		rotQuat = XMQuaternionRotationAxis(rightVec, rotation.y);
		forwardVec = XMVector3Rotate(forwardVec, rotQuat);
		upVec = XMVector3Rotate(upVec, rotQuat);

		//Then Roll around new local forward vector
		rotQuat = XMQuaternionRotationAxis(forwardVec, rotation.z);
		rightVec = XMVector3Rotate(rightVec, rotQuat);
		upVec = XMVector3Rotate(upVec, rotQuat);

		//Orthonormalize to prevent accumulation of float-precision errors
		Orthonormalize(forwardVec, rightVec, upVec);

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
	} 

	void Update()
	{
		if(useSpaceCamera)
			ContinuousUpdate(); 
		else
			RecalculatedUpdate();

		bounds.Center = position;
		bounds.Radius = 1;

		Globals::Get().GetGameCBuffer()->cameraPosition = position;

		//The projection matrix represents how our camera sees the world.
		XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovLH(
			XMConvertToRadians(45),
			Globals::Get().GetAspectRatio(),
			.1f,//Near plane
			1000));//Far plane

		XMStoreFloat4x4(&Globals::Get().GetGameCBuffer()->viewProjection,
			XMMatrixTranspose(XMLoadFloat4x4(&view) * XMLoadFloat4x4(&projection)));

		//Just copies the game's constant buffer data over to the GPU's constant buffer 
		Globals::Get().GetDeviceContext()->UpdateSubresource(
			Globals::Get().GetGPUCBuffer(),
			0, NULL,
			Globals::Get().GetGameCBuffer(),
			0, 0);
	}
};