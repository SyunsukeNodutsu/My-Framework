//-----------------------------------------------------------------------------
// File: GPUParticleShader_CS.hlsl
//
// 計算シェーダ
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticleShader.hlsli"

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}