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
#include "Skybox.h"




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

	// Helper methods added
	void LoadPBRTexturesFromFile(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& albedoSRV, std::wstring& albedoRelativeFilePath,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& normalMapSRV, std::wstring& normalRelativeFilePath,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& roughnessMapSRV, std::wstring& roughnessRelativeFilePath,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& metalnessMapSRV, std::wstring& metalnessRelativeFilePath);

	void PBR_SRV_TextureInitialize(std::shared_ptr<Material> material, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> 
		albedoSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMapSRV,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughnessMapSRV,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalnessMapSRV);

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

	int blurRadius = 0;

	float fogStartDistance = 10.0f;
	float fogEndDistance = 40.0f;


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

	// Sky box pointer
	std::shared_ptr<Skybox> skybox;


	Microsoft::WRL::ComPtr<ID3D11SamplerState > samplerState;

	// Shadow map variables
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;
	DirectX::XMFLOAT4X4 lightViewMatrix;
	DirectX::XMFLOAT4X4 lightProjectionMatrix;
	std::shared_ptr<SimpleVertexShader> shadowVS;
	int shadowMapResolution = 1024; // Ideally a power of 2


	// Resources that are shared among all post processes
	Microsoft::WRL::ComPtr<ID3D11SamplerState> ppSampler;
	std::shared_ptr<SimpleVertexShader> ppVS;

	// Resources that are tied to a particular post process
	std::shared_ptr<SimplePixelShader> ppPS;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> ppRTV; // For rendering
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ppSRV; // For sampling


};

