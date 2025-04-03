#include "GlobalShaderStructs.hlsli"

cbuffer ConstantBuffer : register(b0)
{
	float4x4 view;
	float4x4 projection;
}

VertexToPixel_Sky main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixel_Sky output;

	// Copy of view matrix amd translations set to 0
	matrix viewNoTranslation = view;
	viewNoTranslation._14 = 0;
	viewNoTranslation._24 = 0;
	viewNoTranslation._34 = 0;

	// Multiplying projection and view matrix
	matrix projView = mul(projection, viewNoTranslation);

	// Apply projection and updated view matrix
	output.position = mul(projView, float4(input.localPosition, 1.0f));

	// change depth of each vertex 1.0
	output.position.z = output.position.w;

	// Compute sample direction 
	output.sampleDir = input.localPosition;


	return output;
}