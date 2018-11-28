#include "Common.hlsli"

//Vertex Shader
VPosColNormToPixel main(VPosColNormInput input)
{
	VPosColNormToPixel output;

	output.worldPosition = mul(float4(input.position, 1.0f), objectTransform).xyz;
	output.position = mul(float4(output.worldPosition, 1.0f), viewProjection);
	output.color = input.color * objectColor;
	output.normal = mul(input.normal, (float3x3)objectTransform);
	//output.normal = mul(input.normal, (float3x3)objectInverseTranspose);

	return output;
}