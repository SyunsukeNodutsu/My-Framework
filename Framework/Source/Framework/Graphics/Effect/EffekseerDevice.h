//-----------------------------------------------------------------------------
// File: EffekseerDevice.h
//
// エフェクシアの管理
//-----------------------------------------------------------------------------
#pragma once
#include "../GraphicsDeviceChild.h"

class EffekseerEffect;

//エフェクシア管理クラス
class EffekseerDevice : public GraphicsDeviceChild
{
public:

	//@brief コンストラクタ
	EffekseerDevice();

	//@brief 初期化
	//@return 成功...true
	bool Initialize();

	//@brief 終了
	void Finalize();

	//@brief 更新
	void Update();

	//@brief 描画
	void Draw();

	//@brief 再生
	//@param filepath 読み込むeffectファイルのパス
	//@param position 発生座標
	//@retrun 成功...true
	bool Play(const std::u16string& filepath, const float3& position);

	//@brief 全体の再生を止める
	void StopAll();

	//--------------------------------------------------
	//static
	//--------------------------------------------------

	//最大描画(スプライト)数
	static constexpr int MAX_EFFECT = 8000;

public:
	EffekseerRendererDX11::RendererRef g_renderer;//レンダラー
	Effekseer::ManagerRef g_manager;//マネージャー

private:
	std::map< std::u16string, std::shared_ptr<EffekseerEffect> > m_instanceMap;//各エフェクトのインスタンス管理
	float m_speed;//エフェクト再生速度

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
