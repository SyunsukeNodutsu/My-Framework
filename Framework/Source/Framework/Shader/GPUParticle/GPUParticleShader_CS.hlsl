//-----------------------------------------------------------------------------
//File: GPUParticleShader_CS.hlsl
//
//���q�V�~�����[�V���� �v�Z�V�F�[�_
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticleShader.hlsli"

StructuredBuffer<ParticleCompute> BufIn : register(t0); //���� SRV
RWStructuredBuffer<ParticleCompute> BufOut : register(u0); //�o�� URV

[numthreads(256, 1, 1)]
void main( const ComputeInput In )
{
    int index = In.dispatch.x;
    if (BufIn[index].life <= 0) return;
    
    //���W,�ړ���,�������Ԃ̌v�Z
    float3 result = BufIn[index].pos + (BufIn[index].vel * g_delta_time);
    
    BufOut[index].pos = result;
    BufOut[index].vel = BufIn[index].vel;
    BufOut[index].life = BufIn[index].life - g_delta_time;
    BufOut[index].color = BufIn[index].color;
    BufOut[index].lifeSpanMax = BufIn[index].lifeSpanMax;
    
    //�������Ԃɉ����ē����x��ݒ�
    BufOut[index].color.a = (BufOut[index].life / BufIn[index].lifeSpanMax);
    BufOut[index].color.a = clamp(BufOut[index].color.a, 0, 1);

}