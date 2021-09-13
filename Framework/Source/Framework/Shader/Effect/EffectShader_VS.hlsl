//-----------------------------------------------------------------------------
// File: EffectShader_VS.hlsl
//
// Effect�p ���_�V�F�[�_
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "EffectShader.hlsli"

//-----------------------------------------------------------------------------
// ���_�V�F�[�_�[
//-----------------------------------------------------------------------------
VertexOutput main( float4 position : POSITION, float2 uv : TEXCOORD0, float4 color : COLOR )
{
	// ���ԕ⊮�����PS�ɓn�������쐬����
	VertexOutput ret;
	
    position.w = 1;
    ret.position = mul(position, g_world_matrix);
    ret.position = mul(ret.position, g_view_matrix);
    ret.position = mul(ret.position, g_proj_matrix);
	
	ret.uv = uv;
	ret.color = color;

	return ret;
}