
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 screenPosition : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
};

cbuffer ConstantBuffer : register(b0)
{
	float4 colorTint;
	float2 scale;
	float2 offset;
	float distortionStrength;
	float time;
}

Texture2D SurfaceTexture : register(t0);

Texture2D DistortionSurfaceTexture : register(t1);

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

	// Sample distortion texture
	float2 distortion = DistortionSurfaceTexture.Sample(BasicSampler, input.uv).rg * 2.0 - 1.0;
	distortion *= distortionStrength + sin(time) / 4.0;
	
	// Apply distortion to UVs
	float2 distortedUV = input.uv + distortion;
	
	// Sample the base texture using distorted UVs
	float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, distortedUV);
	
	// Return the final color with tint applied
	return surfaceColor * colorTint;
}