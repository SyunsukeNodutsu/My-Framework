//-----------------------------------------------------------------------------
// File: EffectShader_PS.hlsl
//
// Effect用 ピクセルシェーダ
//-----------------------------------------------------------------------------
#include "EffectShader.hlsli"

// 入力テクスチャ
Texture2D		g_baseColorTexture	: register(t0);
// サンプラ
SamplerState	g_samplerState		: register(s0);

//-----------------------------------------------------------------------------
// ピクセルシェーダー
//-----------------------------------------------------------------------------
float4 main(VertexOutput In) : SV_TARGET
{
    const float4 albedo = g_baseColorTexture.Sample(g_samplerState, In.uv);
    const float4 color = albedo * In.color;

    // アルファテスト
    if (color.a <= 0.0f) discard;
    
	return color;
}