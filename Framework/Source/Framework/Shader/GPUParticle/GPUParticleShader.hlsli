//-----------------------------------------------------------------------------
// File: GPUParticleShader_PS.hlsl
//
// ピクセルシェーダ
//-----------------------------------------------------------------------------

//パーティクル構造体 ※SRVでやり取り
struct ParticleCompute
{
    float3 pos;
    float tmp;
    float3 vel;
    float life;
};

//頂点シェーダが返す値
struct VertexOutput
{
    float4 position     : SV_Position;  //射影座標
    float2 uv           : TEXCOORD0;    //UV座標
    float4 color        : TEXCOORD1;    //色
    float3 wPosition    : TEXCOORD2;    //ワールド3D座標
};

//計算シェーダーに渡される値
struct ComputeInput
{
    uint3 groupThread   : SV_GroupThreadID;
    uint3 group         : SV_GroupID;
    uint  groupIndex    : SV_GroupIndex;
    uint3 dispatch      : SV_DispatchThreadID;
};
