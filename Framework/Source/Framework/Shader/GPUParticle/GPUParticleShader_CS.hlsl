//-----------------------------------------------------------------------------
// File: GPUParticleShader_CS.hlsl
//
// �v�Z�V�F�[�_
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticleShader.hlsli"

#define size_x 256
#define size_y   1
#define size_z   1

// In
StructuredBuffer<ParticleCompute> particle : register(t0); //SRV
// Out
RWStructuredBuffer<ParticleCompute> BufOut : register(u0); //URV

//-----------------------------------------------------------------------------
//�v�Z�V�F�[�_�[
//-----------------------------------------------------------------------------
[numthreads(size_x, size_y, size_z)]
void main( const ComputeInput In )
{
    int index = In.dispatch.x;
    
    //���W,�ړ���,�������Ԃ̌v�Z
    float3 result = particle[index].pos + particle[index].vel;
    BufOut[index].pos = result;
    BufOut[index].vel = BufOut[index].vel;
    BufOut[index].life = BufOut[index].life - 1.0f;
    //BufOut[index].life = BufOut[index].life - 1.0f * g_delta_time;
}