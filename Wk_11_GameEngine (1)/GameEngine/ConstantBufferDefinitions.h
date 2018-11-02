#pragma once

/*
In Math,
A single value is called a Scalar,
A vector is a series of values, usually 3 (XYZ) or 4(RGBA)
A Matrix is like a Table of values

This is a 4x4 Identity Matrix - equivalent to the number '1' in normal math
X * 1 = X

Identity Matrix (I) I * A = A

[1 1 5 0]
[0 1 0 0]
[0 0 1 0]
[0 0 0 1]

A
A'
[1 0 0 0]
[1 1 0 0]
[5 0 1 0]
[0 0 0 1] 

*/

//Constant Buffers just store and transfer data to the GPU
//that doesn't go in vertices.
struct SimpleConstantBuffer
{
	//Camera
	XMFLOAT4X4 viewProjection;//Represents position, rotation, field of view of our eye (camera) in the virtual world
	
	//For objects
	XMFLOAT4X4 objectTransform;//Represents position, rotation, scale of a game object
	XMFLOAT4 objectColor;

	//For a directional light
	XMFLOAT3 lightDirection;
	float lightIntensity;
	XMFLOAT3 lightColor;
	float ambientLightIntensity;

	XMFLOAT3 cameraPosition;
	float objectSpecularity;

	float showNormals;
	float uOffset = 0;
	float vOffset = 0;
	float uScale = 1;
	float vScale = 1;
};