#include "Common.hlsli"

Texture2D diffuseTexture : register(t0);
Texture2D specTexture : register(t1);
Texture2D maskTexture : register(t2);
Texture2D normalMap : register(t3);
TextureCube skyboxTexture : register(t4);

SamplerState standardSampler : register(s0);

//Pixel Shader
//The purpose of a pixel shader is just to determine the final color of any pixel
//Pixel shaders always just return color (as a float4 - rgba - red, green, blue, and alpha values)
float4 main(VPosTexNormTangentToPixel input) : SV_TARGET //System Value Target - tells the GPU where to store the result of this shader, usually the back buffer of our swap chain.
{
	input.normal = normalize(input.normal);//normalize returns a vector instead of a scalar 
	//return float4(input.normal, 1);//Visualize mesh normals.

	float3 eyeDir = normalize(cameraPosition - input.worldPosition);//get the direction from the pixel to the camera
	float4 skyColor = skyboxTexture.Sample(standardSampler, reflect(-eyeDir, input.normal));

	float4 color = diffuseTexture.Sample(standardSampler, input.texCoords);
	float4 maskColor = maskTexture.Sample(standardSampler, input.texCoords);
	float3 diffuse = 0;
	float3 specularity = 0;
	float specTextureValue = specTexture.Sample(standardSampler, input.texCoords).r;

	//Normal Mapping
	input.tangent = normalize(input.tangent); 
	float3 normalMapValue = normalMap.Sample(standardSampler, input.texCoords).xyz;
	float3 normalTangentSpace = 2.0f * normalMapValue - 1.0f;//Transforms normal from 0-1 to -1-1 range
	float3 N = input.normal;
	float3 T = normalize(input.tangent - dot(input.tangent, N) * N);
	float3 B = -cross(N, T);//Calculate BiTangent.
	float3x3 normalTransformMatrix = float3x3(T, B, N);
	input.normal = mul(normalTangentSpace, normalTransformMatrix);//Modifies surface normal by per-pixel value.
	//return float4(input.normal, 1);//Visualize Normal-Mapped normals

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
	
	float4 diffuseColor = lerp(color, objectColor, maskColor.r);//r channel is where to use randomized object color instead of texture color
	
	//Should just be in a different set of shaders that include the sky, but I wanted to save class-time not making a separate one.
	[flatten]
	if (skyColor.a > 0.0f)
		diffuseColor = lerp(diffuseColor, skyColor, specTextureValue);

	float4 finalColor = float4(diffuseColor.xyz * diffuse + specularity + ambientLightIntensity, objectColor.a);
	return finalColor;
}