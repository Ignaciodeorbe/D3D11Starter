#include "GlobalShaderStructs.hlsli"


cbuffer ConstantBuffer : register(b0)
{
	float4 colorTint;
	float distortion;
	float time;
}



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
	float2 uv = (input.uv + 0.5) * 2.0;
	float2 uv0 = input.uv;
	float3 finalColor = float3(0.0, 0.0, 0.0);

	for (float i = 0.0; i < 4.0; i++)
	{
		uv = frac(uv * 1.5) - 0.5;
		float d = length(uv) * exp(-length(uv0));

		float t = (length(uv0) + i * 0.4f + time * 0.4f);

		float3 col = float3(0.5 + 0.5 * cos(6.28318 * (t)),
			0.5 + 0.5 * sin(6.28318 * (t)),
			0.5 - 0.5 * cos(6.28318 * (t)));
			
		
		d = sin(d * 8.0 + time) / 8.0;
		d = abs(d);
		d = pow(0.01 / d, 1.2);
		
		finalColor += col * d;
	}
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	return float4(finalColor, 1.0);
}