//-----------------------------------------------------------------------------
// File: GPUParticleShader_PS.hlsl
//
// �s�N�Z���V�F�[�_
//-----------------------------------------------------------------------------

//�p�[�e�B�N���\���� ��SRV�ł����
struct ParticleCompute
{
    float3 pos;
    float tmp;
    float3 vel;
    float life;
};

//���_�V�F�[�_���Ԃ��l
struct VertexOutput
{
    float4 position     : SV_Position;  //�ˉe���W
    float2 uv           : TEXCOORD0;    //UV���W
    float4 color        : TEXCOORD1;    //�F
    float3 wPosition    : TEXCOORD2;    //���[���h3D���W
};

//�v�Z�V�F�[�_�[�ɓn�����l
struct ComputeInput
{
    uint3 groupThread   : SV_GroupThreadID;
    uint3 group         : SV_GroupID;
    uint  groupIndex    : SV_GroupIndex;
    uint3 dispatch      : SV_DispatchThreadID;
};
