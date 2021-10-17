//-----------------------------------------------------------------------------
// File: SoundDirector.h
//
// ゲームサウンドの全体管理
// 再生リスト, エフェクター, 一括操作(停止, フェード) など
//-----------------------------------------------------------------------------
#pragma once
#include "AudioDeviceChild.h"

// ゲームサウンドの全体管理
class SoundDirector : public AudioDeviceChild
{
public:

	// @brief コンストラクタ
	SoundDirector();

	// @brief 終了
	void Finalize();

	// @brief 更新 ※主に再生リストの更新
	void Update();

	// @brief 再生リスト全停止
	void AllStop();

	// @brief 再生
	// @param filepath ファイルのパス
	// @param delay ディレイ
	// @param volume 音量
	// @param loop ループ再生？
	// @param useFilter フィルター機能使用？
	// @return 成功...true
	bool Play(const std::string& filepath, DWORD delay = 0, float volume = 1.0f, bool loop = false, bool useFilter = false);

	// @brief 3D再生
	// @param filepath ファイルのパス
	// @param pos 発生座標
	// @param delay ディレイ
	// @param volume 音量
	// @param loop ループ再生？
	// @param useFilter フィルター機能使用？
	// @return 成功...true
	bool Play3D(const std::string& filepath, const float3 pos, DWORD delay = 0, float volume = 1.0f, bool loop = false, bool useFilter = false);

	// @brief ゲームサウンドの作成
	// @param filepath ファイルのパス
	// @param loop ループ再生？
	// @param useFilter フィルター機能使用？
	// @return ゲームサウンド
	std::shared_ptr<SoundWork> CreateSoundWork(const std::string& filepath, bool loop = false, bool useFilter = false);

	// @brief 3Dゲームサウンドの作成
	// @param filepath ファイルのパス
	// @param loop ループ再生？
	// @param useFilter フィルター機能使用？
	// @return 3Dゲームサウンド
	std::shared_ptr<SoundWork3D> CreateSoundWork3D(const std::string& filepath, bool loop = false, bool useFilter = false);

	//--------------------------------------------------
	// 取得・設定
	//--------------------------------------------------

	// @brief 単一のインスタンスを返す
	// @return 単一のシングルトン・インスタンス
	static SoundDirector& GetInstance() {
		static SoundDirector instance; return instance;
	}

	// @brief サウンドリストのサイズ(サウンド数)を返す
	// @return サウンドリストに登録されているサウンドの数
	int GetSoundListSize() const {
		return static_cast<int>(m_spSoundList.size());
	}

	// @brief サウンドリストを返す(読み取り専用)
	// @return サウンドリスト
	const std::list<std::shared_ptr<SoundWork>>& GetSoundList() const {
		return m_spSoundList;
	}

	// @brief 再生リストに登録
	// @param sound 追加するサウンド
	void AddSoundList(std::shared_ptr<SoundWork> sound);

private:

	// 管理サウンドリスト
	std::list<std::shared_ptr<SoundWork>> m_spSoundList;

};

//--------------------------------------------------
// #defines: インスタンスの取得
//--------------------------------------------------
#define SOUND_DIRECTOR SoundDirector::GetInstance()
