#include "Entity.h"

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
