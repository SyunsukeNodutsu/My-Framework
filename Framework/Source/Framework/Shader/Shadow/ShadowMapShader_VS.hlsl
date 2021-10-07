//-----------------------------------------------------------------------------
// File: ShadowMapShader_VS.hlsl
//
// �V���h�E�}�b�v�`��p ���_�V�F�[�_�[
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "ShadowMapShader.hlsli"

//-----------------------------------------------------------------------------
// ���_�V�F�[�_�[
//-----------------------------------------------------------------------------
VertexOutput main(
    float4 position : POSITION,
    float2 uv       : TEXCOORD0
)
{
    VertexOutput ret = (VertexOutput) 0;
    
    ret.position = mul(g_world_matrix, position);
    ret.position = mul(g_view_matrix, ret.position);
    ret.position = mul(g_proj_matrix, ret.position);
    
    ret.uv = uv;
    
    ret.wvpPosition = ret.position;
    
	return ret;
}