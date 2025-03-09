#pragma once
#include <DirectXMath.h>
#include <memory>
#include "Mesh.h"
#include "Transform.h"
#include <vector>
#include "BufferStructs.h"
#include "Material.h"
#include "Camera.h"



class Entity
{

private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;

public:

	// Constructor
	Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

	//--------
	// Getters
	//--------
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Material> GetMaterial();


	//--------
	// Setters
	//--------
	void SetMaterial(std::shared_ptr<Material> material);



	//--------
	// Methods
	//--------

	void Draw(std::shared_ptr<Camera> camera);
};