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
	material->PrepareMaterial(camera, transform);

	// Draw the mesh 
	mesh->Draw();
}
