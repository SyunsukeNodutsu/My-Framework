//-----------------------------------------------------------------------------
// File: SpriteShader_VS.hlsl
//
// �摜�`��p ���_�V�F�[�_�[
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "SpriteShader.hlsli"

//-----------------------------------------------------------------------------
// ���_�V�F�[�_�[
//-----------------------------------------------------------------------------
VertexOutput main( float4 position : POSITION, float2 uv : TEXCOORD0 )
{
    VertexOutput ret = (VertexOutput)0;

	// ���_���W���ˉe�ϊ�
    ret.position = mul(position, g_world_matrix);
    ret.position = mul(ret.position, g_proj_matrix);

	// UV
    ret.uv = uv;

    return ret;
}
