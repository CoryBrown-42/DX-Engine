#pragma once

#include "Globals.h"

//Light is when photons bounce off of surfaces that absorb certain wavelengths and then hit our eyes.
//RayTracers are rendering engines that actually mimic photons of light
//Very expensive - usually used for rendering static images (Pixar films, Maya renders)

//Lighting in games is FAKE!

//Gives off light in all directions from a point
struct PointLight//Like a lamp, candle
{
	XMFLOAT3 position;
	XMFLOAT3 color;
	float intensity;
};

//Gives off light in one direction, but also has a point it starts from
struct SpotLight
{
	XMFLOAT3 position;
	XMFLOAT3 direction;
	XMFLOAT3 color;
	float intensity;
};

//A directional light mimics an infintely far away source emitting light in one direction
struct DirectionalLight//Usually used for the Sun in most games
{
	DirectionalLight()
		: direction(20, 0, 0),
		color(1, 1, 1),
		intensity(1)
	{

	}

	void Update()
	{
		//Set CBuffer Data
		XMStoreFloat3(&direction, XMVector3Normalize(XMLoadFloat3(&direction)));
		Globals::Get().GetGameCBuffer()->lightDirection = direction;
		Globals::Get().GetGameCBuffer()->lightColor = color;
		Globals::Get().GetGameCBuffer()->lightIntensity = intensity;

		//Send CBuffer Data (from GameCBuffer to GPUCBuffer)
		Globals::Get().GetDeviceContext()->UpdateSubresource(
			Globals::Get().GetGPUCBuffer(),
			0, NULL,
			Globals::Get().GetGameCBuffer(),
			0, 0);
	}

	XMFLOAT3 direction;
	XMFLOAT3 color;
	float intensity;
};