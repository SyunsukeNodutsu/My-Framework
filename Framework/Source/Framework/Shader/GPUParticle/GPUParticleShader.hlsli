//-----------------------------------------------------------------------------
// File: GPUParticleShader_PS.hlsl
//
// ピクセルシェーダ
//-----------------------------------------------------------------------------

//粒子単位
cbuffer cdObject : register(b7)
{
    float3 g_position;
    float3 g_velocity;
    float lifeSpan;
};

//頂点シェーダが返す値
struct VertexOutput
{
    float4 position     : SV_Position;  //射影座標
    float4 color        : TEXCOORD0;    //色
    float3 wPosition    : TEXCOORD1;    //ワールド3D座標
};
