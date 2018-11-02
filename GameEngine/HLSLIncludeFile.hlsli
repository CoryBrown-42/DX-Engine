//HLSL - High Level Shader Language

//Constant Buffers store data the GPU needs to draw our effects that isn't stored in a vertex
//This cbuffer is where constant buffer data gets stored on the GPU
cbuffer SimpleCBuffer : register(b0)//b0 means buffer 0, and matches the '0' we use when we call deviceContext->VSSetConstantBuffers in ExampleGame::Draw()
{
	//THE ORDER MUST MATCH HERE AND IN YOUR CONSTANT BUFFER STRUCTS
	float4x4 viewProjection;

	//For objects
	float4x4 objectTransform;
	float4 objectColor;

	//Constant buffers always send 4 floats at a time
	//So if you send 3 over, then make sure to have
	//A single float following them 
	//if you have:
	//float3
	//float3

	//Change it to:
	//float3
	//float pad
	//float3

	//For lights
	float3 lightDirection;
	float lightIntensity;
	float3 lightColor;
	float ambientLightIntensity;

	float3 cameraPosition;
	float objectSpecularity;

	float showNormals;
	float uOffset;
	float vOffset;
	float uScale;
	float vScale;
}

struct VPosColInput//Where vertex data gets stored on the GPU when we call deviceContext->VSSetVertexBuffer in ExampleGame::Draw
{
	float3 position : POSITION;
	float4 color	: COLOR;
};

struct VPosColToPixel//What the vertex shader outputs, and the pixel shader will receive
{
	float4 position	: SV_POSITION;//System-Value Position - tells the GPU this is the 2D screen position of the vertex and use it to determine which pixels to process
	float4 color	: COLOR;
};

//Surface Normals are vectors that indicate the direction a face is facing
struct VPosColNormInput
{
	float3 position : POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
};

struct VPosColNormToPixel
{
	float4 position : SV_POSITION;
	float3 localPosition : POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
};

//Surface Normals are vectors that indicate the direction a face is facing
struct VPosNormTextureInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoords : TEXCOORD0;
};

struct VPosNormTextureToPixel
{
	float4 position : SV_POSITION;
	float3 localPosition : POSITION;
	float2 texCoords : TEXCOORD0;
	float3 normal : NORMAL;
};