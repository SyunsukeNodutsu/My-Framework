//-----------------------------------------------------------------------------
// File: ShadowMapShader_PS.hlsl
//
// �V���h�E�}�b�v�`��p �s�N�Z���V�F�[�_�[
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "ShadowMapShader.hlsli"

// �e�N�X�`��
Texture2D g_baseColorTexture : register(t0);

// �T���v��
SamplerState g_samplerState  : register(s0);

//-----------------------------------------------------------------------------
// �s�N�Z���V�F�[�_�[
//-----------------------------------------------------------------------------
float4 main( VertexOutput In ) : SV_TARGET
{
    //float4 albedo = g_baseColorTexture.Sample(g_samplerState, In.uv);
    
    //if (albedo.a < 0.1)
    //    discard;
    
    return float4(In.position.z, In.position.z, In.position.z, 1.0f);
}