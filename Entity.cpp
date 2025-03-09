#include "Entity.h"
#include "BufferStructs.h"


// Initializing mesh and transform shared pointers
Entity::Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
	: mesh(mesh), transform(std::make_shared<Transform>()), material(material)
{
}


//--------
// Getters
//--------

std::shared_ptr<Mesh> Entity::GetMesh() { return mesh; }
std::shared_ptr<Transform> Entity::GetTransform() { return transform; }
std::shared_ptr<Material> Entity::GetMaterial() { return material; }

void Entity::SetMaterial(std::shared_ptr<Material> mat) { material = mat; }


//--------
// Methods
//--------

/// <summary>
/// Draws individual entity
/// </summary>
/// <param name="constantBuffer">The constant buffer that holds the transformation data for the vertex shader</param>
/// <param name="vertexShaderData">The data that will be sent to the vertex shader (like tint and transforms</param>
void Entity::Draw(std::shared_ptr<Camera> camera)
{
	std::shared_ptr<SimpleVertexShader> vertexShaderData = material->VertexShader();

	vertexShaderData->SetFloat4("colorTint", material->Tint()); // Strings here MUST
	vertexShaderData->SetMatrix4x4("world", transform->GetWorldMatrix()); // match variable
	vertexShaderData->SetMatrix4x4("view", camera->ViewMatrix()); // names in your
	vertexShaderData->SetMatrix4x4("projection", camera->ProjectionMatrix()); // shader’s cbuffer!


	//// Copy data to the constant buffer
	//D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	//Graphics::Context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	//memcpy(mappedBuffer.pData, &vertexShaderData, sizeof(vertexShaderData));
	//Graphics::Context->Unmap(constantBuffer.Get(), 0);
	//
	//Graphics::Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	vertexShaderData->CopyAllBufferData();

	material->VertexShader()->SetShader();
	material->PixelShader()->SetShader();

	// Draw the mesh 
	mesh->Draw();
}
