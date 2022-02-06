//-----------------------------------------------------------------------------
// File: GPUParticleShader_PS.hlsl
//
// �s�N�Z���V�F�[�_
//-----------------------------------------------------------------------------

//���q�P��
cbuffer cdObject : register(b7)
{
    float3 g_position;
    float3 g_velocity;
    float lifeSpan;
};

//���_�V�F�[�_���Ԃ��l
struct VertexOutput
{
    float4 position     : SV_Position;  //�ˉe���W
    float4 color        : TEXCOORD0;    //�F
    float3 wPosition    : TEXCOORD1;    //���[���h3D���W
};
