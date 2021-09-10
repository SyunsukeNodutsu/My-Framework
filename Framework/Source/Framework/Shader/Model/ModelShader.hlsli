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
    float3 normal   : NORMAL;       // ワールド法線
    float4 color    : COLOR;        // 色
    float4 wPosition: TEXCOORD1;    // ワールド3D座標
};