#include "Common.hlsli"

//TextureCube is an array of 6 textures representing left, right, forward, backward, up and down
TextureCube skyboxTexture : register(t0);
SamplerState standardSampler : register(s0);

float4 main(VSkyToPixel input) : SV_TARGET
{
	return skyboxTexture.Sample(standardSampler, input.worldPosition);
}