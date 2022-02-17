//-----------------------------------------------------------------------------
//File: GPUParticleShader_CS.hlsl
//
//粒子シミュレーション 計算シェーダ
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticleShader.hlsli"

StructuredBuffer<ParticleCompute> BufIn : register(t0); //入力 SRV
RWStructuredBuffer<ParticleCompute> BufOut : register(u0); //出力 URV

[numthreads(256, 1, 1)]
void main( const ComputeInput In )
{
    int index = In.dispatch.x;
    if (BufIn[index].life <= 0) return;
    
    //座標,移動量,生存期間の計算
    float3 result = BufIn[index].pos + (BufIn[index].vel * g_delta_time);
    
    BufOut[index].pos = result;
    BufOut[index].vel = BufIn[index].vel;
    BufOut[index].life = BufIn[index].life - g_delta_time;
    BufOut[index].color = BufIn[index].color;
    BufOut[index].lifeSpanMax = BufIn[index].lifeSpanMax;
    
    //生存期間に応じて透明度を設定
    BufOut[index].color.a = (BufOut[index].life / BufIn[index].lifeSpanMax);
    BufOut[index].color.a = clamp(BufOut[index].color.a, 0, 1);

}