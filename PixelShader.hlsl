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

	float fogStartDistance;
	float fogEndDistance;

}

Texture2D Albedo : register(t0);

Texture2D NormalMap : register(t1);

Texture2D RoughnessMap : register(t2);

Texture2D MetalnessMap : register(t3);

Texture2D ShadowMap : register(t4);

SamplerState BasicSampler : register(s0);

SamplerComparisonState ShadowSampler : register(s1);

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

	// Sampling from textures
	float3 surfaceColor = pow(Albedo.Sample(BasicSampler, input.uv).rgb, 2.2f);
	float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
	float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;

	// Assume albedo texture is actually holding specular color where metalness == 1
	// Note the use of lerp here - metal is generally 0 or 1, but might be in between
	// because of linear texture sampling, so we lerp the specular color to match
	float3 specularColor = lerp(F0_NON_METAL, surfaceColor.rgb, metalness);

	input.normal = normalize(input.normal);
	
	input.normal = ComputeNormalMap(input.normal, input.tangent, NormalMap, BasicSampler, input.uv);


	// Perform the perspective divide (divide by W) ourselves
	input.shadowMapPos /= input.shadowMapPos.w;

	// Convert the normalized device coordinates to UVs for sampling
	float2 shadowUV = input.shadowMapPos.xy * 0.5f + 0.5f;
	shadowUV.y = 1 - shadowUV.y; // Flip the Y

	// Grab the distances we need: light-to-pixel and closest-surface
	float distToLight = input.shadowMapPos.z;

	// Get a ratio of comparison results using SampleCmpLevelZero()
	float shadowAmount = ShadowMap.SampleCmpLevelZero(
		ShadowSampler,
		shadowUV,
		distToLight).r;


	// The variable for all the lighting
	float3 sceneLighting = surfaceColor * colorTint.rgb;

	float3 totalLight = float3(0, 0, 0);


	for (int i = 0; i < lightsCount; i++)
	{
		float3 lightResult = ComputeLighting(
			lights[i],
			input.normal,
			surfaceColor,
			cameraPosition,
			input.worldPosition,
			roughness,
			specularColor,
			metalness
		);

		// Apply shadow amount only to the first light (typically directional)
		if (i == 0)
		{
			lightResult *= shadowAmount;
		}

		totalLight += lightResult;

	}

	// Create fog effect
	float dist = distance(cameraPosition, input.worldPosition);
	float fog = smoothstep(fogStartDistance, fogEndDistance, dist);

	float3 fogColor = { 0.1f, 0.1f, 0.1f };

	sceneLighting += lerp(totalLight, fogColor, fog);


	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	return float4(pow(sceneLighting, 1.0f / 2.2f), 1.0f);
}