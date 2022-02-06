//-----------------------------------------------------------------------------
// File: SpriteShader_VS.hlsl
//
// 画像描画用 頂点シェーダー
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "SpriteShader.hlsli"

//-----------------------------------------------------------------------------
// 頂点シェーダー
//-----------------------------------------------------------------------------
VertexOutput main( float4 position : POSITION, float2 uv : TEXCOORD0 )
{
    VertexOutput ret = (VertexOutput)0;

	// 頂点座標を射影変換
    ret.position = mul(position, g_world_matrix);
    ret.position = mul(ret.position, g_proj_matrix);

	// UV
    ret.uv = uv;

    return ret;
}
