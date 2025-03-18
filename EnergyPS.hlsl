
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

Texture2D EnergySurfaceTexture : register(t1);

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
	// Adjust scale and UV offset
	input.uv = input.uv * scale + offset;

	// Animate the distortion over time
	float2 animatedUV = input.uv + float2(0, time * 0.2); // Scrolls vertically
	
	// Sample the distortion texture
	float2 distortion = EnergySurfaceTexture.Sample(BasicSampler, animatedUV).rg * sin(time) - 1.0;
	distortion *= distortionStrength; 
	
	// Apply distortion to UVs
	float2 distortedUV = input.uv + distortion;
	
	// Sample the texture using the distorted UVs
	float4 fireColor = SurfaceTexture.Sample(BasicSampler, distortedUV);
	
	// Increase brightness for an emissive effect
	fireColor.rgb += fireColor.rgb * 0.5;
	
	// Apply tint and return final color
	return fireColor * colorTint;
}