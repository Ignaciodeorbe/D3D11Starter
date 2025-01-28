#include "Mesh.h"
#include <memory>


Mesh::Mesh(Vertex vertices, int vertexCount, int indices)
{

}

// Destructor
Mesh::~Mesh()
{

}

//---------------
// Getter Methods
//---------------

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer>  Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

int Mesh::GetIndexCount()
{
	return indexCount;
}

int Mesh::GetVertexCount()
{
	return vertexCount;
}

//--------
// Methods
//--------

void Mesh::Draw()
{

}