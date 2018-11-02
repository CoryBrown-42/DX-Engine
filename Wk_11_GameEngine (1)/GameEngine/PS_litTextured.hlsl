#include "HLSLIncludeFile.hlsli"

Texture2D diffuseTexture : register(t0);
Texture2D specTexture : register(t1);
Texture2D maskTexture : register(t2);

SamplerState standardSampler : register(s0);

//Pixel Shader
//The purpose of a pixel shader is just to determine the final color of any pixel
//Pixel shaders always just return color (as a float4 - rgba - red, green, blue, and alpha values)
float4 main(VPosNormTextureToPixel input) : SV_TARGET //System Value Target - tells the GPU where to store the result of this shader, usually the back buffer of our swap chain.
{
	float4 color = diffuseTexture.Sample(standardSampler, input.texCoords);
	float4 maskColor = maskTexture.Sample(standardSampler, input.texCoords);

	input.normal = normalize(input.normal);//normalize returns a vector instead of a scalar 

	float3 diffuse = 0;
	float specularity = 0;

	float nDotL = dot(input.normal, -lightDirection);
	float3 eyeDir = normalize(cameraPosition - input.localPosition);//get the direction from the pixel to the camera
	
	nDotL = max(nDotL, maskColor.g);

	[flatten]
	if (nDotL > 0.0f)
	{
		float3 reflection = reflect(lightDirection, input.normal);//reflect the light around the surface normal
		specularity += pow(saturate(dot(reflection, eyeDir)), objectSpecularity);//compare the two to see if the light is bouncing into your eye
		specularity *= specTexture.Sample(standardSampler, input.texCoords);
		diffuse += nDotL * lightColor * lightIntensity;
	}
	
	float4 diffuseColor = lerp(color, objectColor, maskColor.r);
	//color.xyz - accessing multiple like this is called a swizzle.
	float4 finalColor = float4(diffuseColor * diffuse + specularity + ambientLightIntensity, objectColor.a);
	float4 visualizedNormals = float4(input.normal, 1);

	//linear interpolation
	return lerp(finalColor, visualizedNormals, showNormals);
	return float4(input.texCoords.xy, 0, 1);

}