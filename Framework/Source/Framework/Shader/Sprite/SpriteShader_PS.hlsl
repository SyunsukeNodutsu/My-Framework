//-----------------------------------------------------------------------------
// File: SpriteShader_PS.hlsl
//
// 画像描画用 ピクセルシェーダー
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "SpriteShader.hlsli"

// テクスチャ
Texture2D g_inputTexture : register(t0);

// サンプラ
SamplerState g_samplerState : register(s0);

//-----------------------------------------------------------------------------
// ピクセルシェーダー
//-----------------------------------------------------------------------------
float4 main( VertexOutput In ) : SV_Target0
{
    // テクスチャ色取得
    float4 color = g_inputTexture.Sample(g_samplerState, In.uv);
    
	// アルファテスト
    if (color.a < 0.1f)
        discard;

	// テクスチャ色 * 指定色
    return color * g_color;
}