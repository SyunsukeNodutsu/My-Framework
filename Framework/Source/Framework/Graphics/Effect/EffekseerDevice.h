//-----------------------------------------------------------------------------
// File: EffekseerDevice.h
//
// エフェクシアの管理
//-----------------------------------------------------------------------------
#pragma once
#include "../GraphicsDeviceChild.h"

class EffectWork;

// エフェクシア管理クラス
class EffekseerDevice : public GraphicsDeviceChild
{
public:

	// @brief コンストラクタ
	EffekseerDevice();

	// @brief 初期化
	// @return 成功...true
	bool Initialize();

	// @brief 終了
	void Finalize();

	// @brief 更新
	// @param deltaTime 前フレームからの経過時間
	void Update(float deltaTime);

	// @brief 再生
	// @param filepath 読み込むeffectファイルのパス
	// @param position 発生座標
	// @retrun 成功...true
	bool Play(const std::u16string& filepath, float3& position);

	// @brief エフェクト読み込み
	// @param filepath 読み込むeffectファイルのパス
	// @retrun
	std::shared_ptr<EffectWork> LoadEffect(const std::u16string& filepath);

	// @brief エフェクト検索
	// @param filepath 検索するeffectファイルのパス
	// @retrun
	std::shared_ptr<EffectWork> FindEffect(const std::u16string& filepath);

	// @brief 全体の再生を止める
	void StopAll();

	//--------------------------------------------------
	// static
	//--------------------------------------------------

	// 最大描画(スプライト)数
	static constexpr int MAX_EFFECT = 8000;

public:

	// Direct11用Renderer
	EffekseerRendererDX11::Renderer* g_renderer;

	// マネージャー
	Effekseer::Manager* g_manager;

private:

	// 各エフェクトのインスタンス管理
	std::map< std::u16string, std::shared_ptr<EffectWork> > m_instanceMap;

	// エフェクト再生速度
	float m_speed;

};

//--------------------------------------------------
// 便利関数
//--------------------------------------------------

// @brief float3をEffekseer::Vector3Dに変換して返す
// @param vector 変換するfloat3
// @return 変換後のEffekseer::Vector3D
static Effekseer::Vector3D ToE3D(float3 vector)
{
	return Effekseer::Vector3D(vector.x, vector.y, vector.z);
}

// @brief mfloat4x4をEffekseer::Matrix44に変換して返す
// @param matrix 変換するmfloat4x4
// @return 変換後のEffekseer::Matrix44
static Effekseer::Matrix44 ToE4x4(mfloat4x4 matrix)
{
	Effekseer::Matrix44 result = {};
	for (int height = 0; height < 4; height++) {
		for (int width = 0; width < 4; width++)
			result.Values[height][width] = matrix.m[height][width];
	}
	return result;
}
