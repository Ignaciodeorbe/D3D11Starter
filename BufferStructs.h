#pragma once
#include <DirectXMath.h>

// C++ struct that matches the layout of the shaders cbuffer
struct VertexShaderData
{
	DirectX::XMFLOAT4 tint;
	DirectX::XMFLOAT4X4 world;

};