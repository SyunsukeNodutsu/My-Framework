//-----------------------------------------------------------------------------
//File: PostProcess_Blur_PS.hlsl
//
//�u���[ �s�N�Z���V�F�[�_�[
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "PostProcessShader.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_samplerState : register(s0);

cbuffer cb : register(b5)
{
    float4 g_offset[31]; //�s�N�Z���̂��ꕪ
};

float4 main(VertexOutput In) : SV_TARGET
{
    float3 color = 0;
    
    //�K�E�X�W�����g�p����31�s�N�Z��������
    for (int i = 0; i < 31; i++)
    {
        color += g_texture.Sample(g_samplerState, In.uv + g_offset[i].xy).rgb * g_offset[i].z;
    }

    return float4(color, 1);
}