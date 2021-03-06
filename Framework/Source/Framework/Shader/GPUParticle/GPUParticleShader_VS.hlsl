//-----------------------------------------------------------------------------
//File: GPUParticleShader_VS.hlsl
//
//粒子シミュレーション 頂点シェーダ
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticleShader.hlsli"

StructuredBuffer<ParticleCompute> particle : register(t2); //SRV

VertexOutput main(float4 position : POSITION, float2 uv : TEXCOORD0, uint InstanceID : SV_InstanceID)
{
    VertexOutput ret = (VertexOutput)0;
    position.w = 1;
    
    //計算シェーダーの結果からシミュレーション
    position.xyz += particle[InstanceID].pos;
    ret.color = particle[InstanceID].color;
    
    //座標変換
    ret.position = mul(position, g_world_matrix);
    ret.wPosition = ret.position.xyz;
    ret.position = mul(ret.position, g_view_matrix);
    ret.position = mul(ret.position, g_proj_matrix);
    
    ret.uv = uv * g_uv_tiling + g_uv_offset;
    
	return ret;
}