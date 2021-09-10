//-----------------------------------------------------------------------------
// File: EffectShader.h
//
// エフェクト用シェーダ
// 光の計算を行いません
//-----------------------------------------------------------------------------
#pragma once

// エフェクト用シェーダ
class EffectShader
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

	// @brief デストラクタ
	~EffectShader() = default;

	// @brief 初期化
	// @return 成功...true 失敗...false
	bool Initialize();

	// @brief 開始
	void Begin();

	// @brief Vertices描画
	void DrawVertices(const std::vector<Vertex>& vertices, D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// @brief モデル描画
	void DrawModel(ModelWork* modelWork);

private:

	ComPtr<ID3D11VertexShader>	m_cpVS;					// 頂点シェーダ
	ComPtr<ID3D11PixelShader>	m_cpPS;					// ピクセルシェーダ
	ComPtr<ID3D11InputLayout>	m_cpInputLayout;		// 頂点入力レイアウト
	ComPtr<ID3D11InputLayout>	m_cpInputLayout_model;	// モデル描画用 頂点入力レイアウト
	std::vector<Buffer>			m_vertexBuffers;		// 頂点ごとのバッファ 3の倍数である程度確保

};
