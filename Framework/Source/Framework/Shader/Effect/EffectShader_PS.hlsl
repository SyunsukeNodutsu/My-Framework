//-----------------------------------------------------------------------------
// File: EffectShader_PS.hlsl
//
// Effect�p �s�N�Z���V�F�[�_
//-----------------------------------------------------------------------------
#include "EffectShader.hlsli"

// ���̓e�N�X�`��
Texture2D		g_baseColorTexture	: register(t0);
// �T���v��
SamplerState	g_samplerState		: register(s0);

//-----------------------------------------------------------------------------
// �s�N�Z���V�F�[�_�[
//-----------------------------------------------------------------------------
float4 main(VertexOutput In) : SV_TARGET
{
    const float4 albedo = g_baseColorTexture.Sample(g_samplerState, In.uv);
    const float4 color = albedo * In.color;

    // �A���t�@�e�X�g
    if (color.a <= 0.0f) discard;
    
	return color;
}