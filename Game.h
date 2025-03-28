#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

#include "Mesh.h"
#include "Entity.h"
#include <vector>
#include <DirectXMath.h>
#include "Camera.h"
#include "Lights.h"



class Game
{
public:
	// Basic OOP setup
	Game() = default;
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Initialize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();

	// Helper methods added by me (Ignacio de Orbe)
	void RefreshUI(float deltaTime);
	void CreateUI();

	// Background color will start as cornflour blue
	float bgColor[4] = { 0.4f, 0.6f, 0.75f, 0.0f };
	bool showDemoWindow = true;
	float fontSize = 1.0f;
	float time;
	bool stopwatch = false;

	// Declaration for Mesh class objects
	std::vector<Entity> entities;

	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT3 translation;

	// Track the current camera
	std::shared_ptr<Camera> currentCamera;

	std::vector<Light> lights;
	DirectX::XMFLOAT3 ambientColor;


private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void CreateGeometry();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// Shaders and shader-related constructs

	// Camera shared pointer
	std::vector<std::shared_ptr<Camera>> cameras;
	std::vector<std::shared_ptr<Material>> materials;

	Microsoft::WRL::ComPtr<ID3D11SamplerState > samplerState;
};

