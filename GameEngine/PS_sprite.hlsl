#include "Common.hlsli"

Texture2D spriteTexture : register(t0);
SamplerState standardSampler : register(s0);

float4 main(VSpriteToPixel input) : SV_TARGET
{
	float3 color = objectColor.xyz * spriteTexture.Sample(standardSampler, input.texCoords).xyz;

	//Try out a variety of simple post process effects...
	//Cooler effects, like Glow, Bloom, Blur, 
	//Screen-Space reflections, God Rays, etc. 
	//require multiple passes and render targets.

	float3 invertedColor = 1 - color;

	float3 sepiaColor = float3(
		color.r * .393f + color.g * .769f + color.b * .189f,
		color.r * .349f + color.g * .686f + color.b * .168f,
		color.r * .272f + color.g * .534f + color.b * .131f);

	float3 greyscaleColor = float3(
		color.r * .299f + color.g * .587f + color.b * .114,
		color.r * .299f + color.g * .587f + color.b * .114,
		color.r * .299f + color.g * .587f + color.b * .114);

	float3 dampenedColor = pow(color, 2);

	float3 weirdColor = color.grb;

	float3 changingColor = float3(color.r, color.g * sin(gameTime), color.b * cos(gameTime));

	//You shouldn't use if's like this in shaders, but I'm just demoing different effects lazily
	if(postEffect == 1)
		return float4(color, 1);
	else if(postEffect == 2)
		return float4(sepiaColor, 1);
	else if (postEffect == 3)
		return float4(greyscaleColor, 1);
	else if (postEffect == 4)
		return float4(invertedColor, 1);
	else if (postEffect == 5)
		return float4(weirdColor, 1);
	else if (postEffect == 6)
		return float4(dampenedColor, 1);
	else if (postEffect == 7)
		return float4(changingColor, 1);

	return float4(color, 1);
}