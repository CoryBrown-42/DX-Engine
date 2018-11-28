#include "Common.hlsli"

//Vertex Shader
//The main purpose of a vertex shader is to project our 3D world onto our 2D screen
VSkyToPixel main(VSkyInput input)
{
	VSkyToPixel output;

	//Transform to Screen Space
	//Take 3D position, put it where the object is, then project it into 2D
	output.position = mul(float4(input.position, 1), mul(objectTransform, viewProjection)).xyww;
	output.worldPosition = input.position;

	return output;
}