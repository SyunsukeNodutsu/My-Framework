//-----------------------------------------------------------------------------
// Name: EffectShader.hlsli
//
// EffectShader�p
//-----------------------------------------------------------------------------

// ���_�V�F�[�_���Ԃ��l
struct VertexOutput
{
	float4 position : SV_POSITION;
	float2 uv		: TEXCOORD0;
	float4 color	: COLOR;
};