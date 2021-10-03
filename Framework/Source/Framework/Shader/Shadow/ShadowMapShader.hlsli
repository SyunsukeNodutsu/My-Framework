//-----------------------------------------------------------------------------
// Name: ShadowMapShader.hlsli
//
// ShadowMapShader共通
//-----------------------------------------------------------------------------

// 頂点シェーダが返す値
struct VertexOutput
{
    float4 position     : SV_Position;  // 射影座標
    float2 uv           : TEXCOORD0;    // UV座標
    float4 wvpPosition  : TEXCOORD1;    // 射影座標 保存用
};
