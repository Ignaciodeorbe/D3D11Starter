#include "GlobalShaderStructs.hlsli"


cbuffer ConstantBuffer : register(b0)
{
	float4 colorTint;
	float2 scale;
	float2 offset;
	float distortionStrength;
	float time;
	float roughness;
	float3 cameraPosition;
	float3 ambient;
	Light directionalLight;
}

Texture2D SurfaceTexture : register(t0);

SamplerState BasicSampler : register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Adjusting scale and uv offset
	input.uv = input.uv * scale + offset;

	float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv);

	// Compute diffuse
	float4 diffuse = float4(Diffuse(directionalLight, input.normal, surfaceColor), 1.0f);


	float4 ambientColor = float4(ambient, 1.0f);

	input.normal = normalize(input.normal);

	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	return surfaceColor * colorTint * ambientColor + diffuse;
}