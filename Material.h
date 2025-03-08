#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "SimpleShader.h"


class Material
{
private:
	DirectX::XMFLOAT4 tint;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;


public:
	Material(DirectX::XMFLOAT4 tint, 
		std::shared_ptr<SimpleVertexShader> vertexShader, 
		std::shared_ptr<SimplePixelShader> pixelShader);


	//--------
	// Getters
	// -------
	DirectX::XMFLOAT4 Tint();
	std::shared_ptr<SimpleVertexShader> VertexShader();
	std::shared_ptr<SimplePixelShader> PixelShader();


	//--------
	// Setters
	//--------
	void SetTint(DirectX::XMFLOAT4 tint);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> vertexShader);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> pixelShader);



};