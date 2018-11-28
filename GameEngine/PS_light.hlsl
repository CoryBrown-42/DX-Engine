#include "Common.hlsli"

//Pixel Shader 
float4 main(VPosColNormToPixel input) : SV_TARGET //System Value Target - tells the GPU where to store the result of this shader, usually the back buffer of our swap chain.
{
	input.normal = normalize(input.normal);//normalize returns a vector instead of a scalar  

	float3 diffuse = 0;
	float specularity = 0;

	//Calculate diffuse lighting for light 1
	float pixelLightAmount = saturate(dot(-lightDirection, input.normal));
	diffuse += pixelLightAmount * lightColor * lightIntensity;

	//Calculate specular lighting for light 1
	float3 eyeDir = normalize(cameraPosition - input.worldPosition);//get the direction from the pixel to the camera
	float3 reflection = reflect(lightDirection, input.normal);//reflect the light around the surface normal
	specularity += pixelLightAmount * pow(saturate(dot(reflection, eyeDir)), objectSpecularity);//compare the two to see if the light is bouncing into your eye

	return float4((input.color.xyz * diffuse) + specularity + ambientLightIntensity, input.color.a);
}