#include "Material.h"


Material::Material(DirectX::XMFLOAT4 tint, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader)
	: tint(tint), vertexShader(vertexShader), pixelShader(pixelShader)
{
}

//--------
// Getters
//--------
DirectX::XMFLOAT4 Material::Tint() { return tint; }
std::shared_ptr<SimpleVertexShader> Material::VertexShader() { return vertexShader; }
std::shared_ptr<SimplePixelShader> Material::PixelShader() { return pixelShader; }


//--------
// Setters
//--------
void Material::SetTint(DirectX::XMFLOAT4 t) { tint = t; }
void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> vs) { vertexShader = vs; }
void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> ps) { pixelShader = ps; }

void Material::AddTextureSRV(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
	textureSRVs.insert({ shaderVariableName, srv });

}

void Material::AddSampler(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
	samplers.insert({ shaderVariableName, sampler });

}


void Material::PrepareMaterial(std::shared_ptr<Camera> camera, std::shared_ptr<Transform> transform)
{

	vertexShader->SetShader();
	pixelShader->SetShader();

	vertexShader->SetFloat4("colorTint", tint); // Strings here MUST
	vertexShader->SetMatrix4x4("world", transform->GetWorldMatrix()); // match variable
	vertexShader->SetMatrix4x4("view", camera->ViewMatrix()); // names in your
	vertexShader->SetMatrix4x4("projection", camera->ProjectionMatrix()); // shader’s cbuffer!

	pixelShader->SetFloat4("colorTint", tint);
	pixelShader->CopyAllBufferData();
	vertexShader->CopyAllBufferData();

	for (auto& t : textureSRVs) { pixelShader->SetShaderResourceView(t.first.c_str(), t.second); }
	for (auto& s : samplers) { pixelShader->SetSamplerState(s.first.c_str(), s.second); }

	pixelShader->SetFloat4("colorTint", tint);

}

