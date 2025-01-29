#include "Mesh.h"
#include <memory>


Mesh::Mesh(Vertex* vertices, unsigned int indices[])
{
	
	// Creating the vertex buffer
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;	
	vbd.ByteWidth = sizeof(Vertex) * 3;       
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; 
	vbd.CPUAccessFlags = 0;	
	vbd.MiscFlags = 0;  
	vbd.StructureByteStride = 0;

		
	D3D11_SUBRESOURCE_DATA initialVertexData = {};
	initialVertexData.pSysMem = vertices; 

		
	Graphics::Device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());
	

	
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;	
	ibd.ByteWidth = sizeof(unsigned int) * 3;	
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;	
	ibd.CPUAccessFlags = 0;	
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

		
	D3D11_SUBRESOURCE_DATA initialIndexData = {};
	initialIndexData.pSysMem = indices; 

		
	Graphics::Device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
	
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
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	Graphics::Context->DrawIndexed(
		3,     
		0,     
		0);    
}