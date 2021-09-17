//-----------------------------------------------------------------------------
// File: main.h
//
// エントリポイント メインループ
//-----------------------------------------------------------------------------
#pragma once

// アプリケーションクラス
class Application
{
public:

	// @brief アプリケーション実行
	void Execute();

	//--------------------------------------------------
	// 取得・設定
	//--------------------------------------------------

	// @brief インスタンスを返す
	// @return シングルトン インスタンス
	static Application& GetInstance()
	{
		static Application instance;
		return instance;
	}

	// @brief アプリケーションを終了させる
	void End() { m_endFlag = true; }

	//--------------------------------------------------
	// グローバルデータ
	//--------------------------------------------------

	// ウィンドウ
	Window g_window;

	// オーディオデバイス
	AudioDevice g_audioDevice;

private:

	bool m_endFlag = false; // ゲーム終了フラグ

	std::shared_ptr<SoundWork> m_spSoundWork;

private:

	// @brief アプリケーション初期化
	// @param width
	// @param height
	bool Initialize(int width, int height);

	// @brief アプリケーション解放
	void Release();
};

//--------------------------------------------------
// #defines: インスタンスの取得
//--------------------------------------------------
#define APP Application::GetInstance()
