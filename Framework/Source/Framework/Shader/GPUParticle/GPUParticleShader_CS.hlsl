//-----------------------------------------------------------------------------
// File: GPUParticleShader_CS.hlsl
//
// 計算シェーダ
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticleShader.hlsli"

//入力
StructuredBuffer<ParticleCompute> particle : register(t0); //SRV

//出力
RWStructuredBuffer<ParticleCompute> BufOut : register(u0); //URV

//-----------------------------------------------------------------------------
//計算シェーダー
//-----------------------------------------------------------------------------
[numthreads(256, 1, 1)]
void main( const ComputeInput In )
{
    int index = In.dispatch.x;
    if (particle[index].life <= 0) return;
    
    //座標,移動量,生存期間の計算
    float3 result = particle[index].pos + (particle[index].vel * g_delta_time);
    
    BufOut[index].pos = result;
    BufOut[index].pos += float3(0, -1 * g_delta_time, 0);//重力
    BufOut[index].vel = particle[index].vel;
    BufOut[index].life = particle[index].life - g_delta_time;
    BufOut[index].color = particle[index].color;
    BufOut[index].lifeSpanMax = particle[index].lifeSpanMax;
    
    BufOut[index].color.a = (BufOut[index].life / particle[index].lifeSpanMax);
    BufOut[index].color.a = clamp(BufOut[index].color.a, 0, 1);

}