//-----------------------------------------------------------------------------
// File: SpriteShader_PS.hlsl
//
// �摜�`��p �s�N�Z���V�F�[�_�[
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "SpriteShader.hlsli"

// �e�N�X�`��
Texture2D g_inputTexture : register(t0);

// �T���v��
SamplerState g_samplerState : register(s0);

//-----------------------------------------------------------------------------
// �s�N�Z���V�F�[�_�[
//-----------------------------------------------------------------------------
float4 main( VertexOutput In ) : SV_Target0
{
    // �e�N�X�`���F�擾
    float4 color = g_inputTexture.Sample(g_samplerState, In.uv);
    
	// �A���t�@�e�X�g
    if (color.a < 0.1f)
        discard;

	// �e�N�X�`���F * �w��F
    return color * g_color;
}