//-----------------------------------------------------------------------------
// Name: ShadowMapShader.hlsli
//
// ShadowMapShader����
//-----------------------------------------------------------------------------

// ���_�V�F�[�_���Ԃ��l
struct VertexOutput
{
    float4 position     : SV_Position;  // �ˉe���W
    float2 uv           : TEXCOORD0;    // UV���W
    float4 wvpPosition  : TEXCOORD1;    // �ˉe���W �ۑ��p
};

// �萔�o�b�t�@
cbuffer cbObject : register(b2)
{
    row_major float4x4 g_light_vp_matrix; // ���C�g�J�����̃r���[�s��*�ˉe�s��
};