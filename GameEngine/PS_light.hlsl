#include "HLSLIncludeFile.hlsli"

//Pixel Shader
//The purpose of a pixel shader is just to determine the final color of any pixel
//Pixel shaders always just return color (as a float4 - rgba - red, green, blue, and alpha values)
float4 main(VPosColNormToPixel input) : SV_TARGET //System Value Target - tells the GPU where to store the result of this shader, usually the back buffer of our swap chain.
{
	input.normal = normalize(input.normal);//normalize returns a vector instead of a scalar 

	float3 diffuse = 0;
	float specularity = 0;

	float nDotL = dot(input.normal, -lightDirection);
	float3 eyeDir = normalize(cameraPosition - input.localPosition);//get the direction from the pixel to the camera
	
	[flatten]
	if (nDotL > 0.0f)
	{
		float3 reflection = reflect(lightDirection, input.normal);//reflect the light around the surface normal
		specularity += pow(saturate(dot(reflection, eyeDir)), objectSpecularity);//compare the two to see if the light is bouncing into your eye

		diffuse += nDotL * lightColor * lightIntensity;
	}
		
	//color.xyz - accessing multiple like this is called a swizzle.
	float4 finalColor = float4(input.color.xyz * diffuse + specularity + ambientLightIntensity, input.color.a);
	float4 visualizedNormals = float4(input.normal, 1);

	//linear interpolation
	return lerp(finalColor, visualizedNormals, showNormals);
}