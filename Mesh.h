#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Graphics.h"
#include "Vertex.h"

class Mesh
{

private:

	// Buffers
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	//Index and vertex count
	int indexCount;
	int vertexCount;

	// Helper method to create buffers from vertex and index data
	void CreateBuffers(Vertex* vertices, int vertexCount, unsigned int* indices, int indexCount);

public:

	// Constructor
	Mesh(Vertex *vertices, int vertexCount, unsigned int* indices, int indexCount);
	Mesh(const char* filename);

	// Destructor
	~Mesh();

	// Getters for data
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer>  GetIndexBuffer();
	int GetIndexCount();
	int GetVertexCount();

	// Method for drawing
	void Draw();

};