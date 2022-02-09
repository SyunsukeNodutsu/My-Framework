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

//����
StructuredBuffer<ParticleCompute> particle : register(t0); //SRV

//�o��
RWStructuredBuffer<ParticleCompute> BufOut : register(u0); //URV

//-----------------------------------------------------------------------------
//�v�Z�V�F�[�_�[
//-----------------------------------------------------------------------------
[numthreads(size_x, size_y, size_z)]
void main( const ComputeInput In )
{
    int index = In.dispatch.x;
    
    //���W,�ړ���,�������Ԃ̌v�Z
    float3 result = particle[index].pos + (particle[index].vel * g_delta_time);
    
    BufOut[index].pos = result;
    BufOut[index].vel = particle[index].vel;
    BufOut[index].life = particle[index].life - g_delta_time;
}