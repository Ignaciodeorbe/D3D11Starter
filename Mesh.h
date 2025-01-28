#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Graphics.h"
#include "Vertex.h"

class Mesh
{
public:

	// Constructor
	Mesh(Vertex *vertices, int indices[]);

	// Destructor
	~Mesh();

	// Getters for data
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer>  GetIndexBuffer();
	int GetIndexCount();
	int GetVertexCount();

	// Method for drawing
	void Draw();


private:

	// Buffers
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	//Index and vertex count
	int indexCount;
	int vertexCount;
};