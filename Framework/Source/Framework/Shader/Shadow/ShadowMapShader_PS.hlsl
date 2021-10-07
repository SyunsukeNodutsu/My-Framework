//-----------------------------------------------------------------------------
// File: ShadowMapShader_PS.hlsl
//
// シャドウマップ描画用 ピクセルシェーダー
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "ShadowMapShader.hlsli"

// テクスチャ
Texture2D g_baseColorTexture : register(t0);

// サンプラ
SamplerState g_samplerState  : register(s0);

//-----------------------------------------------------------------------------
// ピクセルシェーダー
//-----------------------------------------------------------------------------
float4 main( VertexOutput In ) : SV_TARGET
{
    //float4 albedo = g_baseColorTexture.Sample(g_samplerState, In.uv);
    
    //if (albedo.a < 0.1)
    //    discard;
    
    return float4(In.position.z, In.position.z, In.position.z, 1.0f);
}