//-----------------------------------------------------------------------------
// File: Audio.h
//
// 音管理 2年の時に授業で書いたコードをコピー
// DirectXTKのAudio.hを使用
//-----------------------------------------------------------------------------
#pragma once

// 音量を個別に設定する予定
enum SOUND_TAG
{
	eSE = 0,		// 効果音
	eBGM = 1,		// BGM("バックグラウンドミュージック"らしい)
	eEnvironment = 1 << 1,	// 環境音
	eVoice = 1 << 2,	// 声
};

//==================================================
// サウンドデータを扱う サウンドの元データクラス
//==================================================
class SoundEffect
{
public:

	// @brief コンストラクタ
	SoundEffect() {}

	// @brief デストラクタ
	~SoundEffect() { m_soundEffect = nullptr; }

	// @brief インスタンスの生成
	// @param flag フラグ設定
	std::unique_ptr<DirectX::SoundEffectInstance> CreateInstance(DirectX::SOUND_EFFECT_INSTANCE_FLAGS flag)
	{
		return (m_soundEffect)
			? m_soundEffect->CreateInstance(flag)
			: nullptr;
	}

	// @brief WAVEサウンド読み込み
	// @param filepath 音源ファイル
	bool Load(const std::string& filepath);

private:

	// サウンドエフェクト
	std::unique_ptr<DirectX::SoundEffect> m_soundEffect;

	// コピー禁止用:単一のデータはコピーできない
	SoundEffect(const SoundEffect& src) = delete;
	void operator=(const SoundEffect& src) = delete;

};

//==================================================
// サウンドの再生インスタンスクラス(鳴らす度に生成)
//==================================================
class SoundInstance : public std::enable_shared_from_this<SoundInstance>
{
public:

	// @brief コンストラクタ
	SoundInstance() {}

	// @brief 初期化
	virtual void Initialize(const std::shared_ptr<SoundEffect>& soundEffect);

	// @brief 再生
	// @param loop ループ再生？
	virtual void Play(bool loop = false);

	// @brief 停止
	void Stop() { if (m_instance) { m_instance->Stop(); } }

	// @brief 一時停止
	void Pause() { if (m_instance) { m_instance->Pause(); } }

	// @brief 再開
	void Resume() { if (m_instance) { m_instance->Resume(); } }

	// @brief ボリューム設定
	// @param vol ボリューム(1.0が100%)
	void SetVolume(float vol);

	// @brief 再生中かどうか返す
	bool IsPlaying();

protected:

	// サウンドの再生インスタンス
	std::unique_ptr<DirectX::SoundEffectInstance> m_instance;

	// 再生サウンドの元データ
	std::shared_ptr<SoundEffect> m_soundData;

	// コピー禁止用
	SoundInstance(const SoundInstance& src) = delete;
	void operator=(const SoundInstance& src) = delete;

};

//==================================================
// 3Dサウンド再生用のインスタンス
//==================================================
class SoundInstance3D : public SoundInstance
{
public:

	// @brief コンストラクタ
	SoundInstance3D() {}

	// @brief 初期化
	// @param soundEffect インスタンス生成ソース
	void Initialize(const std::shared_ptr<SoundEffect>& soundEffect) override;

	// @brief 再生
	// @param loop ループ再生？
	void Play(bool loop = false) override;

	// @brief 座標設定
	// @param position 座標
	void SetPos(const float3& position);

	// @brief 減衰倍率設定
	// @param val 1:通常 (範囲:FLT_MIN～FLT_MAX)
	void SetCurveDistanceScaler(float val);

protected:

	// エミッター 主に3Dサウンドソースの定義
	DirectX::AudioEmitter m_emitter;

	// コピー禁止用
	SoundInstance3D(const SoundInstance3D& src) = delete;
	void operator=(const SoundInstance3D& src) = delete;
};

//==================================================
// ゲーム内の音を管理するクラス
//==================================================
class AudioManager
{
public:

	// @brief 初期化
	void Initialize();

	// @brief 解放
	void Finalize();

	// @brief 更新
	// @param position リスナーの座標
	// @param dir リスナーの方向
	void Update(const float3& position, const float3& dir);

	// @brief 再生
	// @param filepath 音源ファイル名
	// @param loop ループ再生？
	bool Play(const std::string& filepath, bool loop = false);

	// @brief 再生
	// @param filepath 音源ファイル名
	// @param position エミッター座標
	// @param loop ループ再生？
	bool Play3D(const std::string& filepath, const float3& position, bool loop = false);

	// @brief 再生リストに追加
	// @param sound 追加する音源インスタンス
	void AddPlayList(const std::shared_ptr<SoundInstance>& sound)
	{
		if (!sound.get())
			return;
		m_playList[(UINT)(sound.get())] = sound;
	}

	// @brief 再生リスト停止
	void StopAllSound();

	//--------------------------------------------------
	// 取得
	//--------------------------------------------------

	// @brief 単一のインスタンスを返す
	// @return シングルトン・インスタンス
	static AudioManager& GetInstance()
	{
		static AudioManager instance;
		return instance;
	}

	// @brief オーディオエンジンを返す
	// @return AudioEngine
	std::unique_ptr<DirectX::AudioEngine>& GetAudioEngine() { return m_audioEngine; }

	// @brief オーディオリスナーを返す
	// @return AudioListener
	DirectX::AudioListener& GetListener() { return m_listener; }

	// @brief ユーザー設定の音量を返す
	// @return ユーザー設定の音量
	const float GetUserSettingVolume() const { return m_userVolume; }

	// @brief 再生リストのサイズ(SoundInstanceの数)を返す
	// @return 再生リストのサイズ
	int GetPlayListSize() const { return static_cast<int>(m_playList.size()); }

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------

	// @brief マスター音量設定
	// @param val 設定する音量
	void SetUserSettingVolume(float val) {
		m_userVolume = val;
		m_audioEngine->SetMasterVolume(m_userVolume);
	}

private:

	// オーディオエンジン
	std::unique_ptr<DirectX::AudioEngine> m_audioEngine;

	// オーディオリスナー
	DirectX::AudioListener m_listener;

	// 再生リスト
	std::map<UINT, std::shared_ptr<SoundInstance>> m_playList;

	// ユーザー設定の音量 ※マスター音量に設定
	float m_userVolume;

public:

	// ボリュームメータ
	float g_peakLevels[2];	// ピークメータ ※瞬間最大値
	float g_RMSLevels[2];	// RMSメータ ※平均値

private:

	// @brief コンストラクタ
	AudioManager();

	// ボリュームメータ(APO)の作成
	void InitializeVolumeMeterAPO();

	//
	void UpdateVolumeMeter();

};

//--------------------------------------------------
// #defines: インスタンスの取得
//--------------------------------------------------
#define AUDIO AudioManager::GetInstance()
