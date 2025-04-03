#include "GlobalShaderStructs.hlsli"

TextureCube SkyboxTexture : register(t0);

SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel_Sky input) : SV_TARGET
{
	return SkyboxTexture.Sample(BasicSampler, input.sampleDir);
}