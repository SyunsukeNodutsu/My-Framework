//-----------------------------------------------------------------------------
//File: PostProcess_HBright_PS.hlsl
//
//���P�x���o �s�N�Z���V�F�[�_�[
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "PostProcessShader.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_samplerState : register(s0);

//ACES�t�B�����g�[��
float3 ACESFilmicTone(float3 col)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;

    return saturate((col * (a * col + b)) / (col * (c * col + d) + e));
}

//�s�N�Z���V�F�[�_�[
float4 main(VertexOutput In) : SV_TARGET
{
    float4 texturecolor = g_texture.Sample(g_samplerState, In.uv);
    if (texturecolor.a <= 0) discard;
    
    texturecolor.rgb = max(0, texturecolor.rgb - 1.0f);
    
    //�g�[���t�B���^
    texturecolor.rgb = ACESFilmicTone(texturecolor.rgb * 0.8f);
    
    return texturecolor;
}