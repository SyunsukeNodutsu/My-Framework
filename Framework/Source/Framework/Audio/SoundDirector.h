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

	// @brief ゲームサウンドの作成
	// @return ゲームサウンド
	std::shared_ptr<SoundWork> CreateSoundWork(const std::string& filepath, bool loop);

	// @brief 3Dゲームサウンドの作成
	// @return 3Dゲームサウンド
	std::shared_ptr<SoundWork3D> CreateSoundWork3D(const std::string& filepath, bool loop);

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
	void AddSoundList(std::shared_ptr<SoundWork> sound);

private:

	// 管理サウンドリスト
	std::list<std::shared_ptr<SoundWork>> m_spSoundList;

};

//--------------------------------------------------
// #defines: インスタンスの取得
//--------------------------------------------------
#define SOUND_DIRECTOR SoundDirector::GetInstance()
