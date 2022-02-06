//-----------------------------------------------------------------------------
// File: GPUParticleShader_VS.hlsl
//
// 頂点シェーダ
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticleShader.hlsli"

VertexOutput main(
    float4 position : POSITION,
    float4 color    : COLOR
)
{
    VertexOutput ret = (VertexOutput)0;
    position.w = 1;
    position.xyz += g_position;
    
    //座標変換
    ret.position = mul(position, g_world_matrix);
    ret.wPosition = ret.position.xyz;
    ret.position = mul(ret.position, g_view_matrix);
    ret.position = mul(ret.position, g_proj_matrix);
    
    ret.color = color;
	
	return ret;
}