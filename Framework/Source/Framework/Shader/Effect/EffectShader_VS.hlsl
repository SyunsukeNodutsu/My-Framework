//-----------------------------------------------------------------------------
// File: EffectShader_VS.hlsl
//
// Effect用 頂点シェーダ
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "EffectShader.hlsli"

//-----------------------------------------------------------------------------
// 頂点シェーダー
//-----------------------------------------------------------------------------
VertexOutput main( float4 position : POSITION, float2 uv : TEXCOORD0, float4 color : COLOR )
{
	// 中間補完されるPSに渡す情報を作成する
	VertexOutput ret;
	
    position.w = 1;
    ret.position = mul(position, g_world_matrix);
    ret.position = mul(ret.position, g_view_matrix);
    ret.position = mul(ret.position, g_proj_matrix);
	
	ret.uv = uv;
	ret.color = color;

	return ret;
}