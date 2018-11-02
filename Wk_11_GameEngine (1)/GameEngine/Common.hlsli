//For global shader variables and commonly used functions

//Constants.
#define PT_EMITTER 0
#define PT_FLARE   1
static const float3 ambient = float3(.01f, .01f, .01f);  

//Lights
struct DirectionalLight
{
	float3 diffusecolor;
	float intensity;
	float3 specularcolor;
	float pad1;
	float3 direction;
	float pad2;
};

struct PointLight
{
	float3 diffusecolor;
	float intensity;
	float3 specularcolor;
	float range;
	float3 position;
	float pad1;
	float3 attenuation;
	float pad2;
};

struct SpotLight
{
	float3 diffusecolor;
	float intensity;
	float3 specularcolor;
	float range;
	float3 position;
	float spot;
	float3 direction;
	float pad1;
	float3 attenuation;
	float pad2;
};

//Constant Buffers
cbuffer perMesh : register(b0)
{
	matrix mesh_world;
};

cbuffer perSprite : register(b1)
{
	matrix sprite_world;
	float2 sprite_texPos;
	float2 sprite_texSize;
};

cbuffer perLight : register(b2)
{	
	float3 diffusecolor1;
	float intensity1;
	float3 specularcolor1;
	float padL1;
	float3 direction1;
	float padL2;

	float3 diffusecolor2;
	float intensity2;
	float3 specularcolor2;
	float padL3;
	float3 direction2;
	float padL4;
};

cbuffer perFrame : register(b3)
{
	float frame_time;
	float frame_deltaTime;
	float2 padF1;
};

cbuffer perCamera : register(b4)
{
	float4x4 camera_view;
	float4x4 camera_projection;
	float4x4 camera_viewProjection;
	float3 camera_position;
	float padC1;
	float3 camera_up;
	float padC2;

	// Frustum!
	float4 frustum_top;
	float4 frustum_bot;
	float4 frustum_left;
	float4 frustum_right;
	float4 frustum_near;
	float4 frustum_far;
};

cbuffer perTerrain : register(b5)
{
	// Block 1
	float cellSize;
	float maxHeight;
	uint  tileFactor;
    float texelSpacing;

	// Block 2
	float minSquareDistance;
	float maxSquareDistance;
   	float maxTessellation;
    float minTessellation;

	float3 tOrigin;
};

cbuffer noise : register(b6)
{
	float4 nFBM;
	float nScale;
};

cbuffer perResize : register(b7)
{ 
	uint screen_width;
	uint screen_height;
}

cbuffer perAnimation : register(b8)
{
	float4x4 bone_transforms[96];
};

cbuffer perParticleSystem : register(b9)
{
	float3 particle_emitPos;
	float3 particle_emitDir;
} 

//Shader inputs/outputs
struct VPosColInput
{
	float3 position		: POSITION;
	float4 color		: COLOR;
};

struct VPosColToPixel
{
	float4 position		: SV_POSITION;	// System Value Position - Has specific meaning to the pipeline!
	float4 color		: COLOR;
};

struct VPosTexInput
{
	float3 position		: POSITION;
	float2 texCoords    : TEXCOORD0;
};

struct VSpriteInput
{
	float3 position		: POSITION;
	uint2 index			: INDEX;
};

struct VSpriteInstanceInput
{
	float3 position		: POSITION;
	uint2 index			: INDEX;
	float4x4 world		: WORLD;
	float4 dimensions	: DIMENSIONS;
	//uint InstanceID		: SV_InstanceID;
};

struct VPosTexToPixel
{
	float4 position		: SV_POSITION;
	float2 texCoords	: TEXCOORD0;
};

struct VCubeToPixel
{
	float4 position		: SV_POSITION;
	float3 worldpos		: POSITION;
};

struct VPosColNormInput
{
	float3 position		: POSITION;
	float4 color        : COLOR0;
	float3 normal		: NORMAL;
};

struct VPosColNormInstanceInput	
{	
	float3 position		: POSITION;
	float4 color		: Color;
	float3 normal		: NORMAL; 
	float4x4 world		: WORLD;
	//uint InstanceID		: SV_InstanceID;
};

struct VPosColNormToPixel
{
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	float3 normal		: NORMAL;
	float3 worldpos		: POSITION;
};

struct VPosTexNormInput
{	
	float3 position		: POSITION;
	float2 texCoords    : TEXCOORD0;
	float3 normal		: NORMAL;
};

struct VPosTexNormInstanceInput
{	
	float3 position		: POSITION;
	float2 texCoords    : TEXCOORD;
	float3 normal		: NORMAL; 
	float4x4 world		: WORLD;
	uint InstanceID		: SV_InstanceID;
};

struct VPosTexNormToPixel
{
	float4 position		: SV_POSITION;
	float2 texCoords    : TEXCOORD0;
	float3 normal		: NORMAL; 
	float3 worldpos		: POSITION;
};

struct VAnimationInput
{
	float3 position		: POSITION;
	float2 texCoords	: TEXCOORD;
	float3 normal		: NORMAL;
	float4 tangent		: TANGENT;
	float3 weights		: WEIGHTS;
	uint4 boneIndices	: BONEINDICES;
};

struct VAnimatedToPixel
{
	float4 position		: SV_POSITION;
	float2 texCoords    : TEXCOORD0;
	float3 normal		: NORMAL; 
	float4 tangent		: TANGENT;
	float3 worldpos		: POSITION;
};

//Particle System
struct VSParticleInput
{
    float3 initialPos : POSITION;
	float3 initialVel : VELOCITY;
    float2 size       : SIZE; 
    float  age        : AGE;
    uint   type       : TYPE;  
};

struct VSParticleToGeometry
{
	float4 color    : COLOR;
	float3 position : POSITION;
	float2 size     : SIZE;
	uint   type     : TYPE;
};

struct GSParticleToPixel
{
	float4 position : SV_POSITION;
	float4 color    : COLOR;
	float2 tex      : TEXCOORD;
};

//Points and Lines
struct VSPointToGeometry
{
	float3 position : POSITION;
	float  size : SIZE;
	float4 color : COLOR;
};

struct GSPointToPixel
{
	float4 position : SV_POSITION;
	float4 color    : COLOR;
}; 

//Billboards (face camera, have texture, width, height, may or may not rotate on some axis)
struct VSBillboardToGeometry
{
	float3 position		: POSITION;
	float4 color		: COLOR;
	float2 dimensions	: DIMENSIONS;
};

struct GSBillboardToPixel
{
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	float2 texCoords	: TEXCOORD0;
};