#include "Common.hlsli"

Texture2D diffuseTexture : register(t0);
Texture2D specTexture : register(t1);
Texture2D maskTexture : register(t2);
TextureCube skyboxTexture : register(t3);

SamplerState standardSampler : register(s0);

//Pixel Shader
//The purpose of a pixel shader is just to determine the final color of any pixel
//Pixel shaders always just return color (as a float4 - rgba - red, green, blue, and alpha values)
float4 main(VPosNormTextureToPixel input) : SV_TARGET //System Value Target - tells the GPU where to store the result of this shader, usually the back buffer of our swap chain.
{
	float4 color = diffuseTexture.Sample(standardSampler, input.texCoords);
	
	//Discard pixels if the value passed in is < 0. So if a pixel has alpha 0 - an extra 'buffer' value, it gets discarded
	//This isn't 'Transparency' so much as being either drawn or not.
	//Do this as early in the shader as possible since everything after this line gets skipped if it discards the pixel.
	clip(color.a - .3f);//OpenGL: discard;

	input.normal = normalize(input.normal);//normalize returns a vector instead of a scalar 
	float3 eyeDir = normalize(cameraPosition - input.worldPosition);//get the direction from the pixel to the camera
	float4 skyColor = skyboxTexture.Sample(standardSampler, reflect(-eyeDir, input.normal));
	float4 maskColor = maskTexture.Sample(standardSampler, input.texCoords);
	float3 diffuse = 0;
	float3 specularity = 0;
	float specTextureValue = specTexture.Sample(standardSampler, input.texCoords).r;

	float nDotL = dot(input.normal, -lightDirection);	
	nDotL = max(nDotL, maskColor.g);//Green channel is 'emissive' - ignored shading.

	[flatten]
	if (nDotL > 0.0f)
	{
		float3 reflection = reflect(lightDirection, input.normal);//reflect the light around the surface normal
		specularity += pow(saturate(dot(reflection, eyeDir)), objectSpecularity);//compare the two to see if the light is bouncing into your eye
		specularity *= specTextureValue;
		diffuse += nDotL * lightColor * lightIntensity;
	}

	float4 diffuseColor = lerp(color, objectColor, maskColor.r);//r channel is where to use randomized object color instead of texture color.
	
	//Should just be in a different set of shaders that include the sky, but I wanted to save class-time not making a separate one.
	[flatten]
	if(skyColor.a > 0.0f)
		diffuseColor = lerp(diffuseColor, skyColor, .5f);

	float4 finalColor = float4(diffuseColor.xyz * diffuse + specularity + ambientLightIntensity, objectColor.a);
	return finalColor;
}