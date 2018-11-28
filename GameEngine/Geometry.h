#pragma once

#include "Mesh.h"
#include "RenderManager.h"

namespace Geometry
{
	template<class T>
	static void CreateTriangle(Mesh<T> *mesh, ID3D11Device* device)
	{
		//Create our own objects
		VertexPositionColor vertices[]
		{
			{ XMFLOAT3(0, 0, 0),	XMFLOAT4(1, 0, 0, 1) },
			{ XMFLOAT3(.5f, 1, 0),	XMFLOAT4(0, 0, 1, 1) },
			{ XMFLOAT3(1, 0, 0),	XMFLOAT4(0, 1, 0, 1) },
		};
		mesh->numVertices = 3;
		RenderManager::CreateVertexBuffer<T>(mesh->numVertices, vertices, mesh->vertexBuffer,  device);
	}

	template<class T>
	static void CreateSquare(Mesh<T> *mesh, ID3D11Device* device)
	{
		//Create our own objects
		VertexPositionColor vertices[]
		{
			//The point of backface culling is to improve performance -to get rid of 50% of the triangles we have to draw
			//Counter-Clockwise draw order is the 'back' or 'inside-out' face - those get backface-culled
			{ XMFLOAT3(0, 0, 0),	XMFLOAT4(1, 0, 0, 1) },
			{ XMFLOAT3(0, 1, 0),	XMFLOAT4(0, 1, 0, 1) },//
			{ XMFLOAT3(1, 0, 0),	XMFLOAT4(0, 0, 1, 1) },//

			//Clockwise is the 'front' of the object (or outside) so we draw those
			{ XMFLOAT3(0, 1, 0),	XMFLOAT4(1, 0, 0, 1) },//
			{ XMFLOAT3(1, 1, 0),	XMFLOAT4(0, 0, 1, 1) },
			{ XMFLOAT3(1, 0, 0),	XMFLOAT4(0, 1, 0, 1) },//
		};
		mesh->numVertices = 6;
		RenderManager::CreateVertexBuffer<T>(mesh->numVertices, vertices, mesh->vertexBuffer, device);
	}

	template<class T>
	static void CreateEfficientSquare(Mesh<T> *mesh, ID3D11Device* device)
	{
		//Create our own objects
		VertexPositionColor vertices[]
		{
		 	{ XMFLOAT3(0, 0, 0),	XMFLOAT4(1, 0, 0, 1) },
			{ XMFLOAT3(0, 1, 0),	XMFLOAT4(0, 1, 0, 1) },//
			{ XMFLOAT3(1, 0, 0),	XMFLOAT4(0, 0, 1, 1) },//			 
			{ XMFLOAT3(1, 1, 0),	XMFLOAT4(0, 0, 1, 1) },
		};

		UINT indices[]
		{
			0, 1, 2, 3, 2, 1
		};
		
		mesh->numVertices = 4;
		mesh->numIndices = 6;
		RenderManager::CreateVertexBuffer<T>(mesh->numVertices, vertices, mesh->vertexBuffer, device);
		RenderManager::CreateIndexBuffer(mesh->numIndices, indices, mesh->indexBuffer, device);
	}

	template<class T>
	static void CreateTexturedSquare(Mesh<T> *mesh, ID3D11Device* device)
	{
		//Create our own objects
		VertexPosNormTexture vertices[]
		{
			{ XMFLOAT3(-.5f,-.5f,0), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 1) },//BL
			{ XMFLOAT3(-.5f,.5f, 0), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },//TL
			{ XMFLOAT3(.5f,	-.5f,0), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 1) },//BR			 
			{ XMFLOAT3(.5f,	.5f, 0), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 0) }//TR
		};

		UINT indices[]
		{
			0, 1, 2, 3, 2, 1
		};

		mesh->numVertices = 4;
		mesh->numIndices = 6;
		RenderManager::CreateVertexBuffer<T>(mesh->numVertices, vertices, mesh->vertexBuffer, device);
		RenderManager::CreateIndexBuffer(mesh->numIndices, indices, mesh->indexBuffer, device);
	}

	template<class T>
	static void CreateNormalMappedSquare(Mesh<T> *mesh, ID3D11Device* device)
	{
		//Create our own objects
		vector<VertexPosTexNormTan> vertices
		{
			{ XMFLOAT3(-.5f,-.5f,0), XMFLOAT2(0, 1), XMFLOAT3(0, 0, -1)},//BL
			{ XMFLOAT3(-.5f,.5f, 0), XMFLOAT2(0, 0), XMFLOAT3(0, 0, -1)},//TL
			{ XMFLOAT3(.5f,	-.5f,0), XMFLOAT2(1, 1), XMFLOAT3(0, 0, -1)},//BR			 
			{ XMFLOAT3(.5f,	.5f, 0), XMFLOAT2(1, 0), XMFLOAT3(0, 0, -1)}//TR
		};

		vector<UINT> indices
		{
			0, 1, 2, 3, 2, 1
		}; 

		ModelImporter::CalcTangent(vertices, indices);
		mesh->numVertices = 4;
		mesh->numIndices = 6;
		RenderManager::CreateVertexBuffer(vertices, mesh->vertexBuffer, device);
		RenderManager::CreateIndexBuffer(indices, mesh->indexBuffer, device);
	}
}