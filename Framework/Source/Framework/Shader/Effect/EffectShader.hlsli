//-----------------------------------------------------------------------------
// Name: EffectShader.hlsli
//
// EffectShader用
//-----------------------------------------------------------------------------

// 頂点シェーダが返す値
struct VertexOutput
{
	float4 position : SV_POSITION;
	float2 uv		: TEXCOORD0;
	float4 color	: COLOR;
};