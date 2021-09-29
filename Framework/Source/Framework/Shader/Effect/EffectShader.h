//-----------------------------------------------------------------------------
// File: EffectShader.h
//
// 光の計算を行わないシェーダ
// デバッグ用に直線描画などを行います
//-----------------------------------------------------------------------------
#pragma once
#include "../Shader.h"

// エフェクト用シェーダ
class EffectShader : public Shader
{
public:

	// １頂点の形式
	struct Vertex
	{
		float3 m_position;
		float2 m_uv;
		float4 m_color;
	};

	// @brief コンストラクタ
	EffectShader();

	// @brief 初期化
	// @return 成功...true 失敗...false
	bool Initialize();

	// @brief 開始
	void Begin();

	// 3D線描画
	void DrawLine(const float3& p1, const float3& p2, const cfloat4x4& color);

private:


};
