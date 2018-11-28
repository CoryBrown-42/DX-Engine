#include "VertexDefinitions.h"

D3D11_INPUT_ELEMENT_DESC VertexPositionColor::vertexDesc[] =
{
	//This will tell the GPU that when it receieves vertices as a series of bytes, the first 12 bytes will be 32-bit (4bytes) of floats and we want it stored in POSITION
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},

	//Then the rest of the bytes in this vertex will be 4 floats we want stored in COLOR
	//Notice the '12' here means 'this color data starts 12 bytes from the first byte of the vertex'
	//we get 12 because POSITION above has 3 floats. There are 4 bytes in 1 float. so 3 * 4 = 12 bytes.
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
UINT VertexPositionColor::numElements = 2;
UINT VertexPositionColor::stride = sizeof(VertexPositionColor);

D3D11_INPUT_ELEMENT_DESC VertexPosColNormal::vertexDesc[] =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
UINT VertexPosColNormal::numElements = 3;
UINT VertexPosColNormal::stride = sizeof(VertexPosColNormal);

D3D11_INPUT_ELEMENT_DESC VertexPosNormTexture::vertexDesc[] =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
	{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
UINT VertexPosNormTexture::numElements = 3;
UINT VertexPosNormTexture::stride = sizeof(VertexPosNormTexture);

D3D11_INPUT_ELEMENT_DESC VertexSky::vertexDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
UINT VertexSky::numElements = 1;
UINT VertexSky::stride = sizeof(VertexSky);

D3D11_INPUT_ELEMENT_DESC VertexSprite::vertexDesc[] =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "INDEX",		0, DXGI_FORMAT_R8G8_UINT,		0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 }//Notice Index is format_r8g8 (2 bytes, instead of 8)
};
UINT VertexSprite::stride = sizeof(VertexSprite);
UINT VertexSprite::numElements = 2;

D3D11_INPUT_ELEMENT_DESC VertexPosTexNormTan::vertexDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 20,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 32,  D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
UINT VertexPosTexNormTan::stride = sizeof(VertexPosTexNormTan);
UINT VertexPosTexNormTan::numElements = 4;