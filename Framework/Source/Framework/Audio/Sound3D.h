//-----------------------------------------------------------------------------
// File: Sound3D.h
//
// 3Dサウンド操作クラス
// TODO: メンバに持つべきかどうかなど、いろいろ調査が必要
//-----------------------------------------------------------------------------
#pragma once
#include "AudioDeviceChild.h"

// 3Dサウンド操作クラス
// TODO: Sound継承する
class SoundWork3D : public AudioDeviceChild
{
public:

	// @brief コンストラクタ
	SoundWork3D();

	// @brief 読み込み
	// @param filepath 音源のファイルパス
	// @param loop ループ再生？
	// @return 成功...true
	bool Load(const std::string& filepath, bool loop = false);

	// 解放
	void Release();

	// @brief 再生(3D)
	// @param pos 発生座標
	// @param delay 再生までの待ち時間(ミリ秒)
	void Play3D(const float3& pos, DWORD delay = 0);

	// 更新
	void Update();

private:

	IXAudio2SourceVoice* m_pSourceVoice;
	XAUDIO2_BUFFER              m_buffer;

	std::unique_ptr<uint8_t[]> waveData;

	X3DAUDIO_EMITTER		m_emitter;
	X3DAUDIO_CONE			m_emitterCone;
	X3DAUDIO_DSP_SETTINGS	m_dspSettings;

private:

	// @brief ボイス作成
	// @param loop ループ再生？
	// @return 成功...true
	bool Create(bool loop);

	// エミッターの設定
	void SetEmitter(const float3& pos);

};
