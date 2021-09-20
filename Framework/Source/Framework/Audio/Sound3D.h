//-----------------------------------------------------------------------------
// File: Sound3D.h
//
// 3Dサウンド操作クラス
// TODO: メンバに持つべきかどうかなど、いろいろ調査が必要
//-----------------------------------------------------------------------------
#pragma once
#include "Sound.h"

// 3Dサウンド操作クラス
class SoundWork3D : public SoundWork
{
public:

	// @brief コンストラクタ
	SoundWork3D();

	// @brief 再生(3D)
	// @param pos 発生座標
	// @param delay 再生までの待ち時間(ミリ秒)
	void Play3D(const float3& pos, DWORD delay = 0);

	// 更新
	virtual void Update() override;

private:

	X3DAUDIO_EMITTER		m_emitter;
	X3DAUDIO_CONE			m_emitterCone;
	X3DAUDIO_DSP_SETTINGS	m_dspSettings;

private:

	// エミッターの設定
	void SetEmitter(const float3& pos);

};
