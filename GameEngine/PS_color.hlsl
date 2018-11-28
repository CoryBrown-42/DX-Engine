#include "Common.hlsli"

//Pixel Shader
//The purpose of a pixel shader is just to determine the final color of any pixel
//Pixel shaders always just return color (as a float4 - rgba - red, green, blue, and alpha values)
float4 main(VPosColToPixel input) : SV_TARGET //System Value Target - tells the GPU where to store the result of this shader, usually the back buffer of our swap chain.
{
	return input.color;
}