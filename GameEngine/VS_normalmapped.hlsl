#include "Common.hlsli"

//Vertex Shader
//The main purpose of a vertex shader is to project our 3D world onto our 2D screen
VPosTexNormTangentToPixel main(VPosTexNormTangentInput input)
{
	VPosTexNormTangentToPixel output;

	output.worldPosition = mul(float4(input.position, 1.0f), objectTransform).xyz;
	output.position = mul(float4(output.worldPosition, 1.0f), viewProjection);
	output.texCoords = input.texCoords * float2(uScale, vScale) + float2(uOffset, vOffset);
	output.normal = mul(input.normal, (float3x3)objectInverseTranspose);
	output.tangent = mul(input.tangent, (float3x3)objectTransform);

	return output;
}