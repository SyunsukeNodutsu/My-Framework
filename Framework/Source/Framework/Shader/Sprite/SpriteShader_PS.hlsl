//-----------------------------------------------------------------------------
// File: SpriteShader_PS.hlsl
//
// �摜�`��p �s�N�Z���V�F�[�_�[
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "SpriteShader.hlsli"

Texture2D g_inputTexture : register(t0);
SamplerState g_samplerState : register(s0);

//-----------------------------------------------------------------------------
//�s�N�Z���V�F�[�_�[
//-----------------------------------------------------------------------------
float4 main( VertexOutput In ) : SV_Target0
{
    float4 color = g_inputTexture.Sample(g_samplerState, In.uv);
    if (color.a <= 0.0f) discard;
    
    return color * g_color;
}