#pragma once

#include <DirectXCollision.h>
#include <DirectXMath.h>

using namespace DirectX;

//Abstract class (because it has a pure virtual function)
//We can't instantiate an abstract class object, only inherit from it or make pointers to them
class BaseMesh
{
protected://Private except child or derived classes can access it
public:
	virtual void Draw() = 0;//pure virtual function
	virtual ~BaseMesh() {}//Must have virtual destructor so the correct child destructor gets called polymorphically

	BoundingBox boundingBox;
	BoundingSphere boundingSphere;
};

template<class T>
class Mesh : public BaseMesh //Mesh inherits from BaseMesh
{
public:
	ID3D11Buffer *vertexBuffer = nullptr, *indexBuffer = nullptr;
	UINT numVertices = 0, numIndices = 0;

	//should be called by model importer or function that generates geometry
	void CreateBounds(XMVECTOR minPoints, XMVECTOR maxPoints)
	{
		BoundingBox::CreateFromPoints(boundingBox, minPoints, maxPoints);
		BoundingSphere::CreateFromBoundingBox(boundingSphere, boundingBox);
	}

	void Draw() override 
	{
#if defined(_DEBUG)
		if (numVertices == 0)
			cout << "Error: Mesh vertices shouldn't be 0.\n";
#endif

		UINT offset = 0;//unsigned int = int that can't be negative. offset is used if we have more than one separate mesh stored in one vertex buffer for optimizing memory usage
		Globals::Get().GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &T::stride, &offset);

		if (indexBuffer)
		{
			Globals::Get().GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			Globals::Get().GetDeviceContext()->DrawIndexed(numIndices, 0, 0);
		}
		else
			Globals::Get().GetDeviceContext()->Draw(numVertices, 0);//arguments are number of vertices and which vertex we start at, usually 0
	}

	//Destructor - destroy object, for deallocating memory made using 'new'
	virtual ~Mesh() override
	{
		SafeRelease(vertexBuffer);
		SafeRelease(indexBuffer);
	}
};