//-----------------------------------------------------------------------------
// File: GPUParticleShader_PS.hlsl
//
// �s�N�Z���V�F�[�_
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticleShader.hlsli"

// �e�N�X�`��
Texture2D g_texture : register(t0);
// �T���v��
SamplerState g_samplerState : register(s0);

//-----------------------------------------------------------------------------
//�s�N�Z���V�F�[�_�[
//-----------------------------------------------------------------------------
float4 main( VertexOutput In ) : SV_TARGET
{
    if (In.color.a <= 0) discard;
    
    float4 texcolor = g_texture.Sample(g_samplerState, In.uv);
    if (texcolor.a <= 0) discard;
    
    return texcolor * In.color;
}