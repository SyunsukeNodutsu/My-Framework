//-----------------------------------------------------------------------------
// File: ModelShader_VS.hlsl
//
// Model�`��p ���_�V�F�[�_�[
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "ModelShader.hlsli"

//-----------------------------------------------------------------------------
// ���_�V�F�[�_�[
//-----------------------------------------------------------------------------
VertexOutput main(
    float4 position : POSITION,
    float2 uv       : TEXCOORD0,
    float3 normal   : NORMAL,
    float4 color    : COLOR,
    float3 tangent  : TANGENT
)
{
    VertexOutput ret = (VertexOutput)0;
    
    ret.position    = mul(position, g_world_matrix);
    ret.wPosition   = ret.position.xyz;
    ret.position    = mul(ret.position, g_view_matrix);
    ret.position    = mul(ret.position, g_proj_matrix);
	
    ret.uv = uv * g_uv_tiling + g_uv_offset;
    
    ret.wNormal     = normalize(mul(normal, (float3x3) g_world_matrix));
    ret.wTangent    = normalize(mul(tangent, (float3x3) g_world_matrix));
    
    float3 binormal = cross(normal, tangent);
    ret.wBinormal   = normalize(mul(binormal, (float3x3) g_world_matrix));
    
    ret.color = color;
    
    // ���C�g�r���[�X�N���[����Ԃ̍��W���v�Z
    float4 worldPos = mul(g_world_matrix, position);
    ret.posInLVP[0] = mul(g_mLVPC[0], worldPos);
    ret.posInLVP[1] = mul(g_mLVPC[1], worldPos);
    ret.posInLVP[2] = mul(g_mLVPC[2], worldPos);

    return ret;
}