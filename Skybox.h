#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d11.h>
#include "SimpleShader.h"
#include "Mesh.h"
#include "camera.h"


class Skybox
{

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skyboxSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;

	std::shared_ptr<Mesh> skyboxMesh;
	std::shared_ptr<SimplePixelShader> skyboxPixelShader;
	std::shared_ptr<SimpleVertexShader> skyboxVertexShader;


public:

	Skybox(std::shared_ptr<Mesh> mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler, const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);

	~Skybox();

	//--------
	// Methods
	//--------

	void Draw(std::shared_ptr<Camera> camera);

	// Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);
};