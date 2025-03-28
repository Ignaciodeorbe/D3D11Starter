#include "GlobalShaderStructs.hlsli"


cbuffer ConstantBuffer : register(b0)
{
	Light lights[6];
	int lightsCount;
	float4 colorTint;
	float2 scale;
	float2 offset;
	float3 cameraPosition;
	float time;
	float3 ambient;
	float distortionStrength;
	float roughness;

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

	float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv);

	// The variable for all the lighting
	float3 sceneLighting = surfaceColor * colorTint * ambient;


	for (int i = 0; i < lightsCount; i++)
	{
		// Compute the lighting
		sceneLighting += ComputeLighting(lights[i], input.normal, surfaceColor, cameraPosition, input.worldPosition, roughness);

	}



	input.normal = normalize(input.normal);

	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	return float4(sceneLighting, 1.0f);
}