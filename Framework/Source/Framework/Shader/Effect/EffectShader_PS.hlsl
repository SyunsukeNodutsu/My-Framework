//-----------------------------------------------------------------------------
// File: EffectShader_PS.hlsl
//
// EditHistory:
//  2021/05/12 初回作成
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
	// テクスチャから色を取得
	const float4 baseColor = g_baseColorTexture.Sample(g_samplerState, In.uv);
	// 頂点の色と合成
	float4 color = baseColor * In.color;

	return color;
}