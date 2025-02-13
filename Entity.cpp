#include "Entity.h"
#include "BufferStructs.h"


// Initializing mesh and transform shared pointers
Entity::Entity(std::shared_ptr<Mesh> mesh)
	: mesh(mesh), transform(std::make_shared<Transform>())
{
}


//--------
// Getters
//--------

std::shared_ptr<Mesh> Entity::GetMesh() { return mesh; }
std::shared_ptr<Transform> Entity::GetTransform() { return transform; }

void Entity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, VertexShaderData vertexShaderData)
{
	
	

	// Copy data to the constant buffer
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	Graphics::Context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &vertexShaderData, sizeof(vertexShaderData));
	Graphics::Context->Unmap(constantBuffer.Get(), 0);

	Graphics::Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	mesh->Draw();

	
}
