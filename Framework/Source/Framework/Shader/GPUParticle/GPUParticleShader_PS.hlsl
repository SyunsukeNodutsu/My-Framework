//-----------------------------------------------------------------------------
//File: GPUParticleShader_PS.hlsl
//
//���q�V�~�����[�V���� �s�N�Z���V�F�[�_
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticleShader.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_samplerState : register(s0);

float4 main( VertexOutput In ) : SV_TARGET
{
    if (In.color.a <= 0) discard;
    
    float4 texcolor = g_texture.Sample(g_samplerState, In.uv);
    if (texcolor.a <= 0) discard;
    
    return texcolor * In.color * 1.6f;
}