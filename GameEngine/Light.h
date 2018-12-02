#pragma once

#include "Globals.h"


//Gives off light in all directions from a point
struct PointLight//Like a lamp, candle
{
	XMFLOAT3 position;
	float range;

	XMFLOAT3 color;
	float intensity;

	XMFLOAT3 attenuation;
	float pad;

	PointLight():
		position(1, .5f, 3),
		range(5),
		color(0.25f, 1, 0.25f),
		intensity(1.0f),
		attenuation(1, 1, 1),
		pad(0)
	{}

	void Update()
	{
	
		XMStoreFloat3(&position, XMLoadFloat3(&position));
		Globals::Get().GetGameCBuffer()->lightPos = position;
		Globals::Get().GetGameCBuffer()->lightRange = range;
		Globals::Get().GetGameCBuffer()->lightColor = color;
		Globals::Get().GetGameCBuffer()->lightIntensity = intensity;
		Globals::Get().GetGameCBuffer()->attenuation = attenuation;

		Globals::Get().GetDeviceContext()->UpdateSubresource(
			Globals::Get().GetGPUCBuffer(),
			0, NULL,
			Globals::Get().GetGameCBuffer(),
			0, 0);
	}
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
		: direction(1, 0, 0),
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