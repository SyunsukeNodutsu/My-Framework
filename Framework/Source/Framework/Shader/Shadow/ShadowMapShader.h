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
	void Begin();

	// @brief 描画終了
	void End();

	// @brief モデル描画
	// @param model 描画モデル
	// @param worldMatrix ワールド行列
	void DrawModel(const ModelWork& model, const mfloat4x4& worldMatrix);

	//--------------------------------------------------
	// 設定・取得
	//--------------------------------------------------

	// @brief 平行光用深度マップを返す
	std::shared_ptr<Texture> GetDirShadowMap() const { return m_dirLightShadowMap; }

private:

	std::shared_ptr<Texture> m_dirLightShadowMap;
	std::shared_ptr<Texture> m_dirLightZBuffer;

	// State記憶用
	ID3D11RenderTargetView* m_saveRT;
	ID3D11DepthStencilView* m_saveZ;
	UINT					m_numVP;
	D3D11_VIEWPORT			m_saveVP;

	// ライトカメラの向き
	float3 m_lightCameraLook;

	// シャドウ用 定数バッファ
	struct cbShadow {
		mfloat4x4 m_lvpcMatrix;
	};
	ConstantBuffer<cbShadow> m_cb02Shadow;

private:

	// @brief GPUに転送
	void SetToDevice();

	// @brief メッシュ描画
	// @param mesh メッシュ
	// @param materials マテリアル配列
	void DrawMeshDepth(const Mesh* mesh, const std::vector<Material>& materials);

	//
	void SettingLightCamera();

};
