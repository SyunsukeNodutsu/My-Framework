//-----------------------------------------------------------------------------
// File: GPUParticleShader_VS.hlsl
//
// ���_�V�F�[�_
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticleShader.hlsli"

StructuredBuffer<float3> Position : register(t2);//SRV

//-----------------------------------------------------------------------------
//���_�V�F�[�_�[
//-----------------------------------------------------------------------------
VertexOutput main(
    float4 position : POSITION,
    float2 uv       : TEXCOORD0,
    float4 color    : COLOR,
    uint InstanceID : SV_InstanceID
)
{
    VertexOutput ret = (VertexOutput)0;
    position.w = 1;
    
    position.xyz += Position[InstanceID];
    
    //���W�ϊ�
    ret.position = mul(position, g_world_matrix);
    ret.wPosition = ret.position.xyz;
    ret.position = mul(ret.position, g_view_matrix);
    ret.position = mul(ret.position, g_proj_matrix);
    
    ret.uv = uv * g_uv_tiling + g_uv_offset;
    
    ret.color = color;
    ret.color.xyz = Position[InstanceID];
    
    //matrix wvp;
    //wvp = mul(g_world_matrix, g_view_matrix);
    //wvp = mul(wvp, g_proj_matrix);

    //position.xyz += Position[InstanceID];

    //ret.position = mul(position, wvp);
    //ret.wPosition = mul(position, g_world_matrix);
    
	return ret;
}