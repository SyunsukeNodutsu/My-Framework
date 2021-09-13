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
    float3 wNormal  : NORMAL;       // ���[���h�@��
    float4 color    : COLOR;        // �F
    float4 wPosition: TEXCOORD1;    // ���[���h3D���W
    
    float3 wTangent : TEXCOORD2;    // ���[���h�ڐ��x�N�g��
    float3 wBinormal: TEXCOORD3;    // ���[���h�]�@���x�N�g��
};