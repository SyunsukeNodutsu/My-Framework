//-----------------------------------------------------------------------------
// File: GPUParticleShader_PS.hlsl
//
// ピクセルシェーダ
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticleShader.hlsli"

// テクスチャ
Texture2D g_texture : register(t0);
// サンプラ
SamplerState g_samplerState : register(s0);

//-----------------------------------------------------------------------------
//ピクセルシェーダー
//-----------------------------------------------------------------------------
float4 main( VertexOutput In ) : SV_TARGET
{
    //if (color.a <= 0.0f) discard;
    
    //g_texture.Sample(g_samplerState, In.)
    
    return In.color;
}