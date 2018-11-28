#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>
#include <fstream>
#include <unordered_map>

#include "Mesh.h"

using namespace std;

class ModelImporter
{
public:
	//This is the type of vertex stored in an obj file.
	//To make any of your custom vertex types work with an imported file,
	//They just need a constructor that accepts a VertexObjFormat object to convert from.
	struct VertexObjFormat
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 TextureCoords;//u, v in texture space  
		VertexObjFormat()
		{

		}
		VertexObjFormat(XMFLOAT3 p, XMFLOAT2 t, XMFLOAT3 n)
		{
			Pos = p;
			TextureCoords = t;
			Normal = n;
		}
	};

private:
	struct OBJVertIndex//Helper for reading model file
	{
		UINT vIndex;
		UINT vtIndex;
		UINT vnIndex;
		string indexString;
	};

	struct Triangle//Helper for reading model file
	{
		OBJVertIndex vert1, vert2, vert3;
	};

public:
	//Added to support 3D Builder .obj files. This process can also be used to calculate normals on 
	//handcoded or procedurally generated models (such as terrain)
	static void CalculateIndexedTriListNormals(const vector<XMFLOAT3> &vertexPositions, vector<Triangle> &tris, vector<XMFLOAT3> &outNormals)
	{
		//Get rid of existing normals
		outNormals.clear();

		//fill normals with empty values
		for (UINT i = 0; i < vertexPositions.size(); i++)
			outNormals.push_back(XMFLOAT3(0, 0, 0));

		//Actual Calculation of Normals
		for (UINT i = 0; i < tris.size(); i += 1)
		{
			//calculate normal of a polygon by taking the cross product of two of its sides
			XMVECTOR cross1, cross2, normal;
			cross1 = XMVectorAdd(XMLoadFloat3(&vertexPositions[tris[i].vert1.vIndex]), -XMLoadFloat3(&vertexPositions[tris[i].vert2.vIndex]));
			cross2 = XMVectorAdd(XMLoadFloat3(&vertexPositions[tris[i].vert1.vIndex]), -XMLoadFloat3(&vertexPositions[tris[i].vert3.vIndex]));
			normal = XMVector3Cross(cross1, cross2);

			tris[i].vert1.vnIndex = tris[i].vert1.vIndex;
			tris[i].vert2.vnIndex = tris[i].vert2.vIndex;
			tris[i].vert3.vnIndex = tris[i].vert3.vIndex;

			//Add to the normal already in the vertex, if any. 'Smooth shading' will have normals from multiple faces being stored in one vertex.
			XMStoreFloat3(&outNormals[tris[i].vert1.vnIndex], normal + XMLoadFloat3(&outNormals[tris[i].vert1.vnIndex]));
			XMStoreFloat3(&outNormals[tris[i].vert2.vnIndex], normal + XMLoadFloat3(&outNormals[tris[i].vert2.vnIndex]));
			XMStoreFloat3(&outNormals[tris[i].vert3.vnIndex], normal + XMLoadFloat3(&outNormals[tris[i].vert3.vnIndex]));
		}

		//Normalize normals
		for (UINT i = 0; i < outNormals.size(); ++i)
			XMStoreFloat3(&outNormals[i], XMVector3Normalize(XMLoadFloat3(&outNormals[i])));
	}

	//Assumes T has Position, TexCoords, Normals, and Tangents (at least)
	//Tangents are required for applying normal maps to an object
	//Lengyel, Eric.“Computing Tangent Space Basis Vectors for an Arbitrary Mesh”.Terathon Software 3D Graphics Library, 2001. 
	template <class T> static void CalcTangent(std::vector<T> &vertices, std::vector<UINT> &indices)
	{
		std::vector<XMFLOAT3> tan1(vertices.size()), tan2(vertices.size());

		for (unsigned int i = 0; i < indices.size(); i += 3)//for each triangle
		{
			UINT index1 = indices[i];
			UINT index2 = indices[i + 1];
			UINT index3 = indices[i + 2];

			//Get triangle edges (positions)
			XMFLOAT3 e0 = Utility::Subtract(vertices[index2].position, vertices[index1].position);
			XMFLOAT3 e1 = Utility::Subtract(vertices[index3].position, vertices[index1].position); 

			//Get UV edges
			XMFLOAT2 uv0 = Utility::Subtract(vertices[index2].texture, vertices[index1].texture);
			XMFLOAT2 uv1 = Utility::Subtract(vertices[index3].texture, vertices[index1].texture); 

			//Determine left or right handedness (r will be positive or negative)
			float r = 1.0f / (uv0.x * uv1.y - uv1.x * uv0.y);

			//Calc tangent and bitangent
			XMFLOAT3 tangent;
			tangent.x = (uv1.y * e0.x - uv0.y * e1.x) * r;
			tangent.y = (uv1.y * e0.y - uv0.y * e1.y) * r;
			tangent.z = (uv1.y * e0.z - uv0.y * e1.z) * r;

			XMFLOAT3 tangent2;
			tangent2.x = (uv0.x * e1.x - uv1.x * e0.x) * r;
			tangent2.y = (uv0.x * e1.y - uv1.x * e0.y) * r;
			tangent2.z = (uv0.x * e1.z - uv1.x * e0.z) * r;

			//Add tangents to existing ones for this vertex (we'll average them across the vertex, like we do with normals)
			tan1[index1] = Utility::Add(tan1[index1], tangent);
			tan1[index2] = Utility::Add(tan1[index2], tangent);
			tan1[index3] = Utility::Add(tan1[index3], tangent);

			tan2[index1] = Utility::Add(tan2[index1], tangent2);
			tan2[index2] = Utility::Add(tan2[index2], tangent2);
			tan2[index3] = Utility::Add(tan2[index3], tangent2);
		}

		for (unsigned int i = 0; i < vertices.size(); i++)
		{
			XMVECTOR normal = XMLoadFloat3(&vertices[i].normal);
			XMVECTOR tempTangent = XMLoadFloat3(&tan1[i]);

			//Gram-Schmidt method for orthoganlizing vectors (making sure they are truly perpendicular)
			XMVECTOR tangent = XMVector3Normalize(tempTangent - normal * XMVector3Dot(normal, tempTangent));

			//calculate handedness (tangent direction)
			//XMVECTOR::m128_f32[0] gives us the x value in an XMVECTOR
			float dir = ((XMVector3Dot(XMVector3Cross(normal, tempTangent), XMLoadFloat3(&tan2[i])).m128_f32[0] < 0.0f) ? -1.0f : 1.0f);
			tangent.m128_f32[3] = dir;

			//Store tangent
			XMStoreFloat3(&vertices[i].tangent, tangent);

			//calculate bitangent, if we store in the vertices
			//alternatively, we can calculate it in the shader so vertices have a smaller memory footprint
			//XMStoreFloat3(&vertices[i].BiTangent, XMVector3Cross(normal, tangent));
		}
	}

private:
	//Helper function for reading file
	static void GetVertIndices(std::string &indexStr, OBJVertIndex &vertIndex, UINT &texIndex)
	{
		//indexString is for storing the vertex in a hash table to determine if it can be shared by faces later.
		vertIndex.indexString = indexStr;

		//indices are in format v/vt/vn in obj files, where v are for vertex indices, vt are texture coordinate indices, and vn are normal indices
		//we only need the first value in each, but have to take the whole thing as a string using fin, so then we parse out that first value. 
		int indexOfSlash;

		//Get Position Index
		indexOfSlash = (UINT)indexStr.find('/');
		std::string temp = indexStr.substr(0, indexOfSlash);
		vertIndex.vIndex = (UINT)stoi(temp) - 1;//convert v component of string v/vt/vn to an int, and store it in index

		//Get Texture Index
		indexStr = indexStr.substr(indexOfSlash + 1);
		indexOfSlash = (UINT)indexStr.find('/');
		temp = indexStr.substr(0, indexOfSlash);
		if (indexOfSlash > 0)
			vertIndex.vtIndex = (UINT)stoi(temp) - 1;
		else//No texture index - this vertex doesn't have uvs assigned
		{
			vertIndex.vtIndex = texIndex++;
			if (texIndex == 4)
				texIndex = 1;
		}

		//Get Normal Index
		indexStr = indexStr.substr(indexOfSlash + 1);
		vertIndex.vnIndex = (UINT)stoi(indexStr) - 1;
	}

	template<class T> static void MakeBuffers(std::vector<T> &vertices, std::vector<UINT> &indices, Mesh<T> *mesh, ID3D11Device* device)
	{
		D3D11_BUFFER_DESC bufferDescription;
		bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDescription.ByteWidth = vertices.size() * sizeof(T);
		bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDescription.CPUAccessFlags = 0;
		bufferDescription.MiscFlags = 0;
		bufferDescription.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = vertices.data();
		HR(device->CreateBuffer(&bufferDescription, &data, &mesh->vertexBuffer));

		bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDescription.ByteWidth = indices.size() * sizeof(UINT);
		data.pSysMem = indices.data();
		HR(device->CreateBuffer(&bufferDescription, &data, &mesh->indexBuffer));

		mesh->numIndices = indices.size();
		mesh->numVertices = vertices.size();
	}

	static void ConvertRightToLeftHanded(vector<XMFLOAT3> &positions, vector<XMFLOAT3> &normals, vector<Triangle> &tris)
	{
		//Most current modeling software use a Right-Handed Coordinate system (-Z is forward)
		//But DirectX uses a Left-Handed one - so models would be flipped horizontally
		//This function fixes that.

		for (UINT i = 0; i < positions.size(); i++)
			positions[i].z *= -1;

		for (UINT i = 0; i < normals.size(); i++)
			normals[i].z *= -1;

		for (UINT i = 0; i < tris.size(); i++)
			swap(tris[i].vert2, tris[i].vert3);//Change draw order of vertices so we don't draw inside-out.
	}

public:
	template<class T> static void LoadFromFile(std::string file, Mesh<T> *mesh, ID3D11Device* device)
	{
		std::vector<T> vertices;
		std::vector<UINT> indices;
		XMVECTOR minPos, maxPos;
		ReadModelFile(file, vertices, indices, minPos, maxPos);

		MakeBuffers(vertices, indices, mesh, device);
		mesh->CreateBounds(minPos, maxPos);
	} 

	template<class T> static void LoadFromFileWithTangents(std::string file, Mesh<T> *mesh, ID3D11Device* device)
	{
		std::vector<T> vertices;
		std::vector<UINT> indices;
		XMVECTOR minPos, maxPos;
		ReadModelFile(file, vertices, indices, minPos, maxPos);

		CalcTangent(vertices, indices);

		MakeBuffers(vertices, indices, mesh, device);
		mesh->CreateBounds(minPos, maxPos);
	}

private:
	template<class T> static void ReadModelFile(std::string file, std::vector<T> &vertices, std::vector<UINT> &indices, XMVECTOR &minPos, XMVECTOR &maxPos)
	{
		std::fstream fin;
		fin.open(file, std::ios_base::in);
		if (!fin)
		{
			std::cout << "File " << file << " Not Found." << endl;
			return;
		}

		//Temporaries
		std::vector<Triangle> tris;
		std::vector<VertexObjFormat> tempVertices;
		std::vector<XMFLOAT3> positions, normals;
		std::vector<XMFLOAT2> texCoords;
		std::string type = "", indexStr = "";
		float x, y, z, u, v;//Position, UVs
		float minX, maxX, minY, maxY, minZ, maxZ;//For creating bounding box/sphere for model.
		x = y = z = u = v = 0;
		minX = minY = minZ = numeric_limits<float>::infinity();
		maxX = maxY = maxZ = numeric_limits<float>::lowest();

		UINT texIndex = 1;//If model has no texcoords, this is used instead
		bool endFace, isMoreThan3Sides, skipGetType = false;

		fin >> type;
		while (!fin.eof())
		{
			skipGetType = false;

			if (type == "v")
			{
				fin >> x >> y >> z;
				x < minX ? minX = x : maxX = max(maxX, x);
				y < minY ? minY = y : maxY = max(maxY, y);
				z < minZ ? minZ = z : maxZ = max(maxZ, z);

				positions.push_back(XMFLOAT3(x, y, z));
			}
			else if (type == "vt")
			{
				fin >> u >> v;
				texCoords.push_back(XMFLOAT2(u, 1 - v));
			}
			else if (type == "vn")
			{
				fin >> x >> y >> z;
				normals.push_back(XMFLOAT3(x, y, z));
			}
			else if (type == "f")
			{
				endFace = false;
				isMoreThan3Sides = false;
				skipGetType = true;

				fin >> indexStr;
				while (!endFace)
				{
					tris.push_back(Triangle());
					if (isMoreThan3Sides)//If there are more than 3 sets of indices, the face is not a triangle, so break it into triangles
					{
						tris[tris.size() - 1].vert1 = tris[tris.size() - 2].vert1;
						tris[tris.size() - 1].vert2 = tris[tris.size() - 2].vert3;
						GetVertIndices(indexStr, tris[tris.size() - 1].vert3, texIndex);
					}
					else//Get the first triangle. There must be at least 3 sets of indices on each line.
					{
						GetVertIndices(indexStr, tris[tris.size() - 1].vert1, texIndex);
						fin >> indexStr;
						GetVertIndices(indexStr, tris[tris.size() - 1].vert2, texIndex);
						fin >> indexStr;
						GetVertIndices(indexStr, tris[tris.size() - 1].vert3, texIndex);
					}

					fin >> type;
					if (fin.eof())
						break;
					else if (type.find('/') == std::string::npos)
						endFace = true;
					else
					{
						indexStr = type;
						isMoreThan3Sides = true;
					}
				}
			}
			if (!skipGetType)
				fin >> type;
		}
		fin.close();

		//If model had no texcoords, make dummy ones so things don't break.
		if (texCoords.size() < 4)
		{
			texCoords.push_back(XMFLOAT2(0, 0));
			texCoords.push_back(XMFLOAT2(0, 1));
			texCoords.push_back(XMFLOAT2(1, 0));
			texCoords.push_back(XMFLOAT2(1, 1));
		}

		//If model had no normals, calculate them
		if (normals.size() == 0)
			CalculateIndexedTriListNormals(positions, tris, normals);

		ConvertRightToLeftHanded(positions, normals, tris);

		//Process each triangle
		unordered_map<string, UINT> triMap;//used to determine when to share a vertex or add a new vertex for hard edges/UV seams
		for (unsigned int i = 0; i < tris.size(); i++)
		{
			auto it = triMap.find(tris[i].vert1.indexString);
			if (it != triMap.end())
				indices.push_back(it->second);
			else
			{
				tempVertices.push_back(VertexObjFormat(positions[tris[i].vert1.vIndex], texCoords[tris[i].vert1.vtIndex], normals[tris[i].vert1.vnIndex]));
				indices.push_back(tempVertices.size() - 1);
				triMap[tris[i].vert1.indexString] = indices[indices.size() - 1];
			}

			it = triMap.find(tris[i].vert2.indexString);
			if (it != triMap.end())
				indices.push_back(it->second);
			else
			{
				tempVertices.push_back(VertexObjFormat(positions[tris[i].vert2.vIndex], texCoords[tris[i].vert2.vtIndex], normals[tris[i].vert2.vnIndex]));
				indices.push_back(tempVertices.size() - 1);
				triMap[tris[i].vert2.indexString] = indices[indices.size() - 1];
			}

			it = triMap.find(tris[i].vert3.indexString);
			if (it != triMap.end())
				indices.push_back(it->second);
			else
			{
				tempVertices.push_back(VertexObjFormat(positions[tris[i].vert3.vIndex], texCoords[tris[i].vert3.vtIndex], normals[tris[i].vert3.vnIndex]));
				indices.push_back(tempVertices.size() - 1);
				triMap[tris[i].vert3.indexString] = indices[indices.size() - 1];
			}
		}

		//Create final vertex array. This converts the OBJ vertices into your custom Vertex (T)
		for (unsigned int i = 0; i < tempVertices.size(); i++)
			vertices.push_back(T(tempVertices[i]));

		minPos = XMVectorSet(minX, minY, minZ, 1);
		maxPos = XMVectorSet(maxX, maxY, maxZ, 1);
	}
};