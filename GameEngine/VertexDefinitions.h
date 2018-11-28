#pragma once

#include <DirectXMath.h>
#include "ModelImporter.h"

//Vertex - single unit of data given to graphics card to draw a surface on the screen. 
struct VertexPositionColor
{
	//static in a class/struct means each object of this class will share that variable as opposed to having its own copy
	//If we have 100 vertices in a model of an Apple, they each need a different position, but they all will have the same 'numElements'
	static D3D11_INPUT_ELEMENT_DESC vertexDesc[];//this is the directx struct that tells the GPU about this vertex.
	static UINT numElements;//uint - unsigned int. numElements is how many different elements this vertex has - in this case 2, position and color
	
	//stride is how many bytes are in 1 of these vertices.
	//In this case position = 3 floats, color = 4 floats, 1 float = 4 bytes, so 7 * 4 = 28 bytes. 
	//But just use the sizeof() operator to figure it out for you!
	static UINT stride;

	XMFLOAT3 position;
	XMFLOAT4 color;
	
	VertexPositionColor(XMFLOAT3 pos, XMFLOAT4 col)
		: position(pos), color(col)
	{

	}

	VertexPositionColor(ModelImporter::VertexObjFormat v)
	{
		position = v.Pos;
		color = XMFLOAT4(1, 1, 1, 1);// Utility::RandomColor();
	}
};

//Surface Normal - A vector representing the direction a face is pointing
struct VertexPosColNormal
{
	//static in a class/struct means each object of this class will share that variable as opposed to having its own copy
	//If we have 100 vertices in a model of an Apple, they each need a different position, but they all will have the same 'numElements'
	static D3D11_INPUT_ELEMENT_DESC vertexDesc[];//this is the directx struct that tells the GPU about this vertex.
	static UINT numElements;//uint - unsigned int. numElements is how many different elements this vertex has - in this case 2, position and color

							//stride is how many bytes are in 1 of these vertices.
							//In this case position = 3 floats, color = 4 floats, 1 float = 4 bytes, so 7 * 4 = 28 bytes. 
							//But just use the sizeof() operator to figure it out for you!
	static UINT stride;

	XMFLOAT3 position;
	XMFLOAT4 color;
	XMFLOAT3 normal;

	VertexPosColNormal(XMFLOAT3 pos, XMFLOAT4 col, XMFLOAT3 norm)
		: position(pos), color(col), normal(norm)
	{

	}

	VertexPosColNormal(ModelImporter::VertexObjFormat v)
	{
		position = v.Pos;
		color = XMFLOAT4(1, 1, 1, 1);// Utility::RandomColor();
		normal = v.Normal;
	}

};

//Surface Normal - A vector representing the direction a face is pointing
struct VertexPosNormTexture
{
	//static in a class/struct means each object of this class will share that variable as opposed to having its own copy
	//If we have 100 vertices in a model of an Apple, they each need a different position, but they all will have the same 'numElements'
	static D3D11_INPUT_ELEMENT_DESC vertexDesc[];//this is the directx struct that tells the GPU about this vertex.
	static UINT numElements;//uint - unsigned int. numElements is how many different elements this vertex has - in this case 2, position and color

							//stride is how many bytes are in 1 of these vertices.
							//In this case position = 3 floats, color = 4 floats, 1 float = 4 bytes, so 7 * 4 = 28 bytes. 
							//But just use the sizeof() operator to figure it out for you!
	static UINT stride;

	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 texcoords;

	VertexPosNormTexture(XMFLOAT3 pos, XMFLOAT3 norm, XMFLOAT2 tex)
		: position(pos), normal(norm), texcoords(tex)
	{

	}

	VertexPosNormTexture(ModelImporter::VertexObjFormat v)
	{
		position = v.Pos;
		normal = v.Normal;
		texcoords = v.TextureCoords;
	}
};

//Surface Normal - A vector representing the direction a face is pointing
struct VertexSky
{
	//static in a class/struct means each object of this class will share that variable as opposed to having its own copy
	//If we have 100 vertices in a model of an Apple, they each need a different position, but they all will have the same 'numElements'
	static D3D11_INPUT_ELEMENT_DESC vertexDesc[];//this is the directx struct that tells the GPU about this vertex.
	static UINT numElements;//uint - unsigned int. numElements is how many different elements this vertex has - in this case 2, position and color

							//stride is how many bytes are in 1 of these vertices.
							//In this case position = 3 floats, color = 4 floats, 1 float = 4 bytes, so 7 * 4 = 28 bytes. 
							//But just use the sizeof() operator to figure it out for you!
	static UINT stride;

	XMFLOAT3 position; 

	VertexSky(XMFLOAT3 pos)
		: position(pos)
	{

	}

	VertexSky(ModelImporter::VertexObjFormat v)
	{
		position = v.Pos;
	}
};

struct VertexSprite
{
	static D3D11_INPUT_ELEMENT_DESC vertexDesc[];
	static UINT numElements;
	static UINT stride;

	XMFLOAT3 Position;
	BYTE index[2];
};

struct VertexPosTexNormTan
{
	static D3D11_INPUT_ELEMENT_DESC vertexDesc[];
	static UINT numElements;
	static UINT stride;

	XMFLOAT3 position;
	XMFLOAT2 texture;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;

	VertexPosTexNormTan()
	{

	}

	VertexPosTexNormTan(XMFLOAT3 Pos, XMFLOAT2 Tex, XMFLOAT3 Norm)
		: position(Pos),
		texture(Tex),
		normal(Norm)
	{

	}

	VertexPosTexNormTan(ModelImporter::VertexObjFormat v)
	{
		position = v.Pos;
		texture = v.TextureCoords;
		normal = v.Normal;
	}
};
typedef VertexPosTexNormTan VertexStandard;