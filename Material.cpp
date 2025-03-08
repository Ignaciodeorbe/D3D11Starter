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

