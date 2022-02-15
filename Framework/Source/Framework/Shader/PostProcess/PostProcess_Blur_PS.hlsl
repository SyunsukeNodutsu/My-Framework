//-----------------------------------------------------------------------------
//File: PostProcess_Blur_PS.hlsl
//
//
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "PostProcessShader.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_samplerState : register(s0);

cbuffer cb : register(b0)
{
    float4 g_offset[31]; //ピクセルのずれ分
};

float4 main(VertexOutput In) : SV_TARGET
{
    float3 color = 0;
    
    //ガウス係数を使用して31ピクセルを合成
    for (int i = 0; i < 31; i++)
    {
        color += g_texture.Sample(g_samplerState, In.uv + g_offset[i].xy).rgb * g_offset[i].z;
    }

    return float4(color, 1);
}