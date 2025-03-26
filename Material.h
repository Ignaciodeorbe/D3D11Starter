#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "SimpleShader.h"
#include <unordered_map>
#include "Camera.h"
#include "Transform.h"

class Material
{
private:
	DirectX::XMFLOAT4 tint;
	DirectX::XMFLOAT2 scale;
	DirectX::XMFLOAT2 offset;
	float distortionStrength;
	float time;
	float roughness;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;

	std::string shaderName;


public:
	Material(DirectX::XMFLOAT4 tint, 
		std::shared_ptr<SimpleVertexShader> vertexShader, 
		std::shared_ptr<SimplePixelShader> pixelShader,
		DirectX::XMFLOAT2 scale,
		DirectX::XMFLOAT2 offset,
		float distortionStrength = 1.0f,
		float time = 0.0f,
		float roughness = 1.0f);


	//--------
	// Getters
	// -------
	DirectX::XMFLOAT4 Tint();
	DirectX::XMFLOAT2 Scale();
	DirectX::XMFLOAT2 Offset();
	float DistortionStrength();
	float Time();
	float Roughness();
	std::shared_ptr<SimpleVertexShader> VertexShader();
	std::shared_ptr<SimplePixelShader> PixelShader();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTextureSRV();
	std::string ShaderName();


	//--------
	// Setters
	//--------
	void SetTint(DirectX::XMFLOAT4 tint);
	void SetScale(DirectX::XMFLOAT2 scale);
	void SetOffset(DirectX::XMFLOAT2 offset);
	void SetDistortionStrength(float distortionStrength);
	void SetTime(float time);
	void SetRoughness(float roughness);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> vertexShader);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> pixelShader);

	//--------
	// Methods
	//--------
	void AddTextureSRV(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void AddSampler(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);
	void PrepareMaterial(std::shared_ptr<Camera> camera, std::shared_ptr<Transform> transform, float deltaTime);

};