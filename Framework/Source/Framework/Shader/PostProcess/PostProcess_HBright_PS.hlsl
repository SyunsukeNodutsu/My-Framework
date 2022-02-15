//-----------------------------------------------------------------------------
//File: PostProcess_HBright_PS.hlsl
//
//
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "PostProcessShader.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_samplerState : register(s0);

float4 main(VertexOutput In) : SV_TARGET
{
    float4 texturecolor = g_texture.Sample(g_samplerState, In.uv);
    if (texturecolor.a <= 0) discard;
    
    float3 result = texturecolor.rgb = max(0, texturecolor.rgb - 1.0f);
    
    return float4(result, 1);
}