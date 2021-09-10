//-----------------------------------------------------------------------------
// File: EffectShader_PS.hlsl
//
// EditHistory:
//  2021/05/12 ����쐬
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
	// �e�N�X�`������F���擾
	const float4 baseColor = g_baseColorTexture.Sample(g_samplerState, In.uv);
	// ���_�̐F�ƍ���
	float4 color = baseColor * In.color;

	return color;
}