#include "Common.hlsli"

//Pixel Shader 
float4 main(VPosColNormToPixel input) : SV_TARGET 
{
	input.normal = normalize(input.normal);
//Use as if bool
float DirectionalLight = 0;

float3 diffuse = 0;
float specularity = 0;

float pixelLightAmount = saturate(dot(-lightDirection, input.normal));
diffuse += pixelLightAmount * lightColor * lightIntensity;


float3 lightVec = lightPos - input.worldPosition;

float dis = length(lightVec);

if (dis > lightRange)
{
	return float4(
		0.1f,
		0.1f,
		0.1f,
		0.1f
		);
}

		lightVec /= dis;

		float3 eyeDir = normalize(cameraPosition - input.worldPosition);
		float3 reflection = reflect(lightDirection, input.normal);
		specularity += pixelLightAmount * pow(saturate(dot(reflection, eyeDir)), objectSpecularity);
		float diffuseFactor = dot(lightVec, input.normal);

		[flatten]
		if (diffuseFactor > 0.0f)
		{
			float3 reflection = reflect(-lightVec, input.normal);
			specularity += pow(saturate(dot(reflection, eyeDir)), objectSpecularity);

			diffuse += diffuseFactor * lightColor * lightIntensity;
		}

		float att = 1.0f / dot(attenuation, float3(1.0f, dis, dis*dis));

		diffuse *= att;
		specularity *= att;

		float4 finalColor = float4(input.color.xyz * diffuse + specularity + ambientLightIntensity, input.color.a);
		float4 visualizedNormals = float4(input.normal, 1);

		/*[flatten]
		if (DirectionalLight == 1)
		{
			return float4((input.color.xyz * diffuse) + specularity + ambientLightIntensity, input.color.a);
		}
		else
		{*/
		//return lerp(finalColor, float4(input.normal, 1), 1);
		return finalColor;
		//}


	}

