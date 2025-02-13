#pragma once
#include <DirectXMath.h>
#include <memory>
#include "Mesh.h"
#include "Transform.h"


class Entity
{

private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;

public:

	// Constructor
	Entity(std::shared_ptr<Mesh> mesh);

	//--------
	// Getters
	//--------
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
};