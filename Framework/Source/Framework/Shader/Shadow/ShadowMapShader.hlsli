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
