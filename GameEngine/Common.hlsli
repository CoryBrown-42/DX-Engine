//HLSL - High Level Shader Language

//Constant Buffers store data the GPU needs to draw our effects that isn't stored in a vertex
//This cbuffer is where constant buffer data gets stored on the GPU
cbuffer SimpleCBuffer : register(b0)//b0 means buffer 0, and matches the '0' we use when we call deviceContext->VSSetConstantBuffers in ExampleGame::Draw()
{
	//RULES:
	//1. THE ORDER MUST MATCH HERE AND IN YOUR CONSTANT BUFFER STRUCTS IN ConstantBufferDefinitions.h
	//2. YOU MUST NEVER REACH A MULTIPLE OF 4 FLOATS IN THE MIDDLE OF AN OBJECT
		//In other words, float4, float4 is ok, 
		//float2, float2 is ok, float2, float, float is ok. 
		//but float3, float3 is NOT OK and should be 'padded' to float3, float, float3 ...
	//3. For Efficiency, you should have many small cbuffers instead of one big one (we'll do that in class toward the end of the semester)
		//Each cbuffer should happen at different frequencies, aka. 1 per camera update, 1 per frame, 1 per 'window size change', 1 per object, etc.
		//That way you don't send EVERYTHING everytime you change 1 object's color/texture/position.

	float4x4 viewProjection;

	//For objects
	float4x4 objectTransform;
	float4x4 objectInverseTranspose;
	float4 objectColor; 

	//For lights
	float3 lightDirection;
	float lightIntensity;
	float3 lightColor;
	float ambientLightIntensity;

	float3 cameraPosition;
	float objectSpecularity;

	float uOffset;
	float vOffset;
	float uScale;
	float vScale;
	//could do float2 texOffset, float2 texScale
	//or float4 texModifier instead

	float2 screenSize;
	float gameTime;
	uint postEffect;//same byte-size as 1 float; just won't have those dang pesky decimals.

	//Sprites will reuse objectTransform above as well for position/rotation/scale
	float2 spriteTexPos;//Where in the texture/sprite sheet to pull from - top left corner
	float2 spriteTexSize;//width and height of sprite to pull from texture/spritesheet
	float spriteDepth;
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
	float4 position : SV_POSITION;//position is in screen space based on camera
	float3 worldPosition : POSITION;//3D world space position (including gameobject's position, rotation, scale)
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
	float3 worldPosition : POSITION;
	float2 texCoords : TEXCOORD0;
	float3 normal : NORMAL;
};

struct VSkyInput
{
	float3 position : POSITION;
};

struct VSkyToPixel
{
	float4 position : SV_POSITION;
	float3 worldPosition : POSITION;
};

struct VPosTexNormTangentInput
{
	float3 position		: POSITION;
	float2 texCoords    : TEXCOORD0;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 worldPosition		: POSITION;
};

struct VPosTexNormTangentToPixel
{
	float4 position		: SV_POSITION;
	float2 texCoords    : TEXCOORD0;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 worldPosition		: POSITION;
};

typedef VPosTexNormTangentInput StandardVertInput;
typedef VPosTexNormTangentToPixel StandardVertToPixel;

struct VSpriteInput
{
	float3 position		: POSITION;
	uint2 index			: INDEX;
};

struct VSpriteToPixel
{
	float4 position		: SV_POSITION;
	float2 texCoords	: TEXCOORD0;
};