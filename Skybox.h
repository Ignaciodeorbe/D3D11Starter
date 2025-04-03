#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d11.h>
#include "SimpleShader.h"
#include "Mesh.h"

class Skybox
{

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skyboxSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;

	std::shared_ptr<Mesh> skybox;
	std::shared_ptr<SimplePixelShader> skyboxPixelShader;
	std::shared_ptr<SimpleVertexShader> skyboxPixelShader;


public:


	//--------
	// Getters
	//--------
	


	//--------
	// Setters
	//--------



	//--------
	// Methods
	//--------
};