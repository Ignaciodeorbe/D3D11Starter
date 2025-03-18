#include "Material.h"


Material::Material(DirectX::XMFLOAT4 tint, std::shared_ptr<SimpleVertexShader> vertexShader, 
	std::shared_ptr<SimplePixelShader> pixelShader, DirectX::XMFLOAT2 scale, 
	DirectX::XMFLOAT2 offset, float distortionStrength, float time)
	: tint(tint), vertexShader(vertexShader), pixelShader(pixelShader), scale(scale), offset(offset), distortionStrength(distortionStrength), time(time)
{
}

//--------
// Getters
//--------
DirectX::XMFLOAT4 Material::Tint() { return tint; }
DirectX::XMFLOAT2 Material::Scale() { return scale; }
DirectX::XMFLOAT2 Material::Offset() { return offset; }
float Material::DistortionStrength() { return distortionStrength; }
float Material::Time() { return time; }
std::shared_ptr<SimpleVertexShader> Material::VertexShader() { return vertexShader; }
std::shared_ptr<SimplePixelShader> Material::PixelShader() { return pixelShader; }

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetTextureSRV()
{
	// Search for texture
	auto it = textureSRVs.find(shaderName);

	// Return the found texture
	if (it != textureSRVs.end())
	{
		return it->second;  
	}

	return nullptr;
}

std::string Material::ShaderName() { return shaderName; }


//--------
// Setters
//--------
void Material::SetTint(DirectX::XMFLOAT4 t) { tint = t; }
void Material::SetScale(DirectX::XMFLOAT2 s) { scale = s; }
void Material::SetOffset(DirectX::XMFLOAT2 o) { offset = o; }
void Material::SetDistortionStrength(float distortion) { distortionStrength = distortion; }
void Material::SetTime(float t) { time = t; }
void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> vs) { vertexShader = vs; }
void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> ps) { pixelShader = ps; }

void Material::AddTextureSRV(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
	textureSRVs.insert({ shaderVariableName, srv });
	shaderName = shaderVariableName;

}

void Material::AddSampler(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
	samplers.insert({ shaderVariableName, sampler });

}


void Material::PrepareMaterial(std::shared_ptr<Camera> camera, std::shared_ptr<Transform> transform, float totalTime)
{

	vertexShader->SetShader();
	pixelShader->SetShader();

	vertexShader->SetFloat4("colorTint", tint); // Strings here MUST
	vertexShader->SetMatrix4x4("world", transform->GetWorldMatrix()); // match variable
	vertexShader->SetMatrix4x4("view", camera->ViewMatrix()); // names in your
	vertexShader->SetMatrix4x4("projection", camera->ProjectionMatrix()); // shader’s cbuffer!

	pixelShader->SetFloat4("colorTint", tint);
	pixelShader->SetFloat2("scale", scale);
	pixelShader->SetFloat2("offset", offset);
	pixelShader->SetFloat("distortionStrength", distortionStrength);
	pixelShader->SetFloat("time", totalTime);
	pixelShader->CopyAllBufferData();
	vertexShader->CopyAllBufferData();

	for (auto& t : textureSRVs) { pixelShader->SetShaderResourceView(t.first.c_str(), t.second); }
	for (auto& s : samplers) { pixelShader->SetSamplerState(s.first.c_str(), s.second); }

	pixelShader->SetFloat4("colorTint", tint);

}

