//-----------------------------------------------------------------------------
//File: EffekseerEffect.h
//
//エフェクシアのエフェクトクラス
//-----------------------------------------------------------------------------
#pragma once
#include "EffekseerDeviceChild.h"

//エフェクトクラス
class EffekseerEffect : public EffekseerDeviceChild
{
public:

	//@brief コンストラクタ
	EffekseerEffect();

	//@brief 初期化
	//@param filepath 読み込むエフェクトのファイルパス
	void Initialize(const std::u16string& filepath);

	//@brief 再生
	//@param position 再生座標
	//@param speed 再生速度
	void Play(const float3& position, float speed = 60.0f);

	//@brief 停止
	void Stop();

	//@brief 再生中のEffectを移動
	//@param addPosition 加算するベクトル
	void AddLocation(const float3& addPosition);

private:
	Effekseer::EffectRef m_effect;//エフェクトデータ
	Effekseer::Handle m_handle;//ハンドル(index)

};
