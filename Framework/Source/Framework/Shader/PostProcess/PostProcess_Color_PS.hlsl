//-----------------------------------------------------------------------------
//File: PostProcess_Color_PS.hlsl
//
//
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "PostProcessShader.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_samplerState : register(s0);

float4 main(VertexOutput In) : SV_Target0
{
    float4 texturecolor = g_texture.Sample(g_samplerState, In.uv);
    //if (texturecolor.a <= 0) discard;
    
    //float4 result = texturecolor * color;
    float4 result = texturecolor;
    
    return result;
}