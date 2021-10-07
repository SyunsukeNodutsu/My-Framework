//-----------------------------------------------------------------------------
// Name: ModelShader.hlsli
//
// ModelShader共通
//-----------------------------------------------------------------------------

// 頂点シェーダが返す値
struct VertexOutput
{
    float4 position : SV_Position;  // 射影座標
    float2 uv       : TEXCOORD0;    // UV座標
    float4 color    : TEXCOORD1;    // 色
    float3 wNormal  : TEXCOORD2;    // ワールド法線
    float3 wPosition: TEXCOORD3;    // ワールド3D座標
    
    float3 wTangent : TEXCOORD4;    // ワールド接線
    float3 wBinormal: TEXCOORD5;    // ワールド従法線
    
    float4 posInLVP[3] : TEXCOORD6; // ライトビュースクリーン空間でのピクセルの座標
};