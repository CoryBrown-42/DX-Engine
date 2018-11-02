#include "HLSLIncludeFile.hlsli"

//Vertex Shader
//The main purpose of a vertex shader is to project our 3D world onto our 2D screen
VPosColNormToPixel main(VPosColNormInput input)
{
	VPosColNormToPixel output;

	//Transform to Screen Space
	//Take 3D position, put it where the object is, then project it into 2D
	output.position = mul(float4(input.position, 1),
		mul(objectTransform, viewProjection));
	output.localPosition = mul(float4(input.position, 1), objectTransform).xyz;//swizzle

	output.color = input.color * objectColor;

	//mul, normalize, etc. are called HLSL intrinsics
	output.normal = mul(input.normal, (float3x3)objectTransform);

	return output;
}