//-----------------------------------------------------------------------------
// Name: ModelShader.hlsli
//
// ModelShader����
//-----------------------------------------------------------------------------

// �萔�o�b�t�@
cbuffer cdObject : register(b4)
{
    float4 g_color;
};

// ���_�V�F�[�_���Ԃ��l
struct VertexOutput
{
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
};
