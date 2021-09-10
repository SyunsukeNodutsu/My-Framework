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
    float3 normal   : NORMAL;       // ���[���h�@��
    float4 color    : COLOR;        // �F
    float4 wPosition: TEXCOORD1;    // ���[���h3D���W
};