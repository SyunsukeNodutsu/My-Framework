//-----------------------------------------------------------------------------
// File: ShadowMapShader_PS.hlsl
//
// シャドウマップ描画用 ピクセルシェーダー
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "ShadowMapShader.hlsli"

// テクスチャ
Texture2D g_baseColorTexture : register(t0); // 基本色

// サンプラ
SamplerState g_samplerState  : register(s0);

//-----------------------------------------------------------------------------
// ピクセルシェーダー
//-----------------------------------------------------------------------------
float4 main( VertexOutput In ) : SV_TARGET
{
    float4 albedo = g_baseColorTexture.Sample(g_samplerState, In.uv);
    
    if (albedo.a < 0.1)
        discard;

    // R成分に射影座標系での深度値(最近接距離：0.0 ～ 最遠方距離：1.0)を出力
    // ※射影座標系のxyzはwで除算する必要がある
    return float4(In.wvpPosition.z / In.wvpPosition.w, 0, 0, 1);
}