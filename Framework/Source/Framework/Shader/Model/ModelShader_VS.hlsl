//-----------------------------------------------------------------------------
// File: ModelShader_VS.hlsl
//
// Model描画用 頂点シェーダー
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "ModelShader.hlsli"

//-----------------------------------------------------------------------------
// 頂点シェーダー
//-----------------------------------------------------------------------------
VertexOutput main( float4 position : POSITION, float2 uv : TEXCOORD0, float3 normal : NORMAL, float4 color : COLOR )
{
    VertexOutput ret = (VertexOutput)0;
    
    position.w = 1;
    ret.wPosition = mul(position, g_world_matrix);
    ret.position = mul(ret.wPosition, g_view_matrix);
    ret.position = mul(ret.position, g_proj_matrix);
	
    ret.uv = uv;
    
    ret.normal = mul(normal, (float3x3) g_world_matrix);
    ret.normal = normalize(ret.normal);
    
    ret.color = color;

    return ret;
}