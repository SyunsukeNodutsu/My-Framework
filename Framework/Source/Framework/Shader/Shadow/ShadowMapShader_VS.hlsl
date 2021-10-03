//-----------------------------------------------------------------------------
// File: ShadowMapShader_VS.hlsl
//
// シャドウマップ描画用 頂点シェーダー
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "ShadowMapShader.hlsli"

//-----------------------------------------------------------------------------
// 頂点シェーダー
//-----------------------------------------------------------------------------
VertexOutput main(
    float4 position : POSITION,
    float2 uv       : TEXCOORD0
)
{
    VertexOutput ret = (VertexOutput) 0;
    
    ret.position = mul(position, g_world_matrix);
    ret.position = mul(ret.position, g_light_vp_matrix);
    
    ret.wvpPosition = ret.position;
    
    ret.uv = uv;
    
	return ret;
}