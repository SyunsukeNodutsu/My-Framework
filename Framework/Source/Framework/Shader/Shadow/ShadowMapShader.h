//-----------------------------------------------------------------------------
// File: GameSystem.h
//
// シャドウイング カスケードシャドウ
//-----------------------------------------------------------------------------
#pragma once
#include "../Shader.h"

// シャドウマップシェーダー
class ShadowMapShader : public Shader
{
public:

	// @brief コンストラクタ
	ShadowMapShader();

	// @brief 初期化
	// @return 成功...true
	bool Initialize();

	// @brief 描画開始
	// @param numShadow 何枚目のシャドウ
	void Begin(int numShadow = 0);

	// @brief 描画終了
	void End();

	// @brief モデル描画
	// @param model 描画モデル
	// @param worldMatrix ワールド行列
	void DrawModel(const ModelWork& model, const mfloat4x4& worldMatrix);

	//--------------------------------------------------
	// 設定・取得
	//--------------------------------------------------

	// @brief シャドウマップを返す
	// @param numShadow 何枚目のシャドウマップ
	// @return 平行光用深度マップ
	std::shared_ptr<Texture> GetShadowMap(int numShadow = 0) const {
		return m_shadowMaps[numShadow];
	}

private:

	// シャドウマップの数
	static constexpr int NUM_SHADOW_MAP = 3;

	// レンダーターゲット
	std::shared_ptr<Texture> m_shadowMaps[NUM_SHADOW_MAP];
	std::shared_ptr<Texture> m_zBuffer[NUM_SHADOW_MAP];

	// ライトビュープロジェクションクロップ行列
	mfloat4x4 m_lvpcMatrix[NUM_SHADOW_MAP];

	// 分割エリアの最大深度値
	float m_cascadeAreaTable[NUM_SHADOW_MAP];

	// エリアの近平面までの距離
	float m_nearDepth;

	// State記憶用
	ID3D11RenderTargetView* m_saveRT;
	ID3D11DepthStencilView* m_saveZ;
	UINT					m_numVP;
	D3D11_VIEWPORT			m_saveVP;

	//
	struct ShadowParam {
		mfloat4x4 m_mLVPC;
		mfloat4x4 tmp[2];
	};
	ConstantBuffer<ShadowParam> m_cb2;

private:

	// @brief ライトカメラの設定
	void SettingLightCamera(int numShadow);

	// @brief メッシュ描画
	// @param mesh メッシュ
	// @param materials マテリアル配列
	void DrawMeshDepth(const Mesh* mesh, const std::vector<Material>& materials);

};
