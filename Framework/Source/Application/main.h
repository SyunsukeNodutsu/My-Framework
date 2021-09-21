//-----------------------------------------------------------------------------
// File: main.h
//
// アプリケーション
// TODO: visual stdio側 でプロジェクトを分けたい
//-----------------------------------------------------------------------------
#pragma once
#include "GameSystem.h"
#include "ImGuiSystem.h"

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
	// @return 単一シングルトン・インスタンス
	static Application& GetInstance() {
		static Application instance; return instance;
	}

	// @brief アプリケーションを終了させる
	void End() { m_endFlag = true; }

	//--------------------------------------------------
	// グローバルデータ
	//--------------------------------------------------

	// ウィンドウ
	Window g_window;

	// 各種デバイス
	
	// 描画デバイス
	GraphicsDevice g_graphicsDevice;
	// オーディオデバイス
	AudioDevice	g_audioDevice;
	// 入力デバイス
	RawInputDevice g_rawInputDevice;

	// fps計測
	FpsTimer g_fpsTimer;

	// ゲーム

	// ゲーム管理
	GameSystem g_gameSystem;
	// プロファイラ デバッグ
	ImGuiSystem g_imGuiSystem;

private:

	// ゲーム終了フラグ
	bool m_endFlag = false;

private:

	// @brief アプリケーション初期化
	// @param width ウィンドウ ビューポート幅
	// @param height ウィンドウ ビューポート高さ
	// @return 成功...true 失敗...false
	bool Initialize(int width, int height);

	// @brief アプリケーション解放
	void Release();
};

//--------------------------------------------------
// #defines: インスタンスの取得
//--------------------------------------------------
#define APP Application::GetInstance()
