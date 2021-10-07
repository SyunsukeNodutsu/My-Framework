//-----------------------------------------------------------------------------
// Name: ModelShader.hlsli
//
// ModelShader����
//-----------------------------------------------------------------------------

// ���_�V�F�[�_���Ԃ��l
struct VertexOutput
{
    float4 position : SV_Position;  // �ˉe���W
    float2 uv       : TEXCOORD0;    // UV���W
    float4 color    : TEXCOORD1;    // �F
    float3 wNormal  : TEXCOORD2;    // ���[���h�@��
    float3 wPosition: TEXCOORD3;    // ���[���h3D���W
    
    float3 wTangent : TEXCOORD4;    // ���[���h�ڐ�
    float3 wBinormal: TEXCOORD5;    // ���[���h�]�@��
    
    float4 posInLVP[3] : TEXCOORD6; // ���C�g�r���[�X�N���[����Ԃł̃s�N�Z���̍��W
};