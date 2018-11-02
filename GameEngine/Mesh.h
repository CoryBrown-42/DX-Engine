#pragma once

//Abstract class (because it has a pure virtual function)
//We can't instantiate an abstract class object, only inherit from it or make pointers to them
class BaseMesh
{
protected://Private except child or derived classes can access it
public:
	virtual void Draw(ID3D11DeviceContext* deviceContext) = 0;//pure virtual function
	virtual ~BaseMesh() {}//Must have virtual destructor so the correct child destructor gets called polymorphically
};

template<class T>
class Mesh : public BaseMesh //Mesh inherits from BaseMesh
{
public:
	ID3D11Buffer *vertexBuffer = nullptr, *indexBuffer = nullptr;
	UINT numVertices = 0, numIndices = 0;

	void Draw(ID3D11DeviceContext* deviceContext) override 
	{
		UINT offset = 0;//unsigned int = int that can't be negative. offset is used if we have more than one separate mesh stored in one vertex buffer for optimizing memory usage
		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &T::stride, &offset);

		if (indexBuffer)
		{
			deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			deviceContext->DrawIndexed(numIndices, 0, 0);
		}
		else
			deviceContext->Draw(numVertices, 0);//arguments are number of vertices and which vertex we start at, usually 0
	}

	//Destructor - destroy object, for deallocating memory made using 'new'
	~Mesh()
	{
		SafeRelease(vertexBuffer);
		SafeRelease(indexBuffer);
	}
};