//-----------------------------------------------------------------------------
// File: Effect.h
//
// エフェクシアのデータ
// TODO: Effect.hじゃなくEffekseerEffect.hのほうがいいかも
//-----------------------------------------------------------------------------
#pragma once
#include "EffekseerDeviceChild.h"

// エフェクトデータ
class EffectData : public EffekseerDeviceChild
{
public:

	// @brief コンストラクタ
	EffectData();

	// @brief エフェクト読み込み
	// @param filepath 読み込みファイルのパス
	// @return 成功...true
	bool Load(const std::string& filepath);

private:

	Effekseer::EffectRef m_effect;

};

// エフェクト操作
class EffectWork : public EffekseerDeviceChild
{
public:

	// @brief コンストラクタ
	EffectWork();

	// @brief 再生
	void Play() {}
	
	// @brief 停止
	void Stop() {}

private:

	EffectData m_data;

};
