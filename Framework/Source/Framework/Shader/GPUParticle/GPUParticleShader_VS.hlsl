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
    
    ret.color = color;
	
	return ret;
}