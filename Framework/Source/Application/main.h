//-----------------------------------------------------------------------------
// File: main.h
// 
// ■フレームワーク全体像(2021/09/24)
// 
// Application(main.h)------AudioDevice
//   |    |  |                   |
// Window |  |             AudioDeviceChild
//		  |  |                |        |
//		  |  ┗-----┓  SoundDirector Sound
//		  |         |                  |
//	RawInputDevice  |                Sound3D
//	  |       |     |
// Keyboard   Mouse |
//					|
//			  GraphicsDevice            Model---Animation
//					|                   |   |
//			GraphicsDeviceChild--------Mesh |
//			 |      |       |  ┗--┓       |
//		   Buffer Texture Shader   |      Material
//			 |              |      |
//	   ConstantBuffer -- 各Shader  |
//			 |                     |
//			 ┗------------------Renderer
// 
// ■使用ライブラリ
// 外部 ---
// DirectXTK
// Dear imGui
// Effekseer(1.62a)
// 学内配布 ---
// json
// gltfLoader
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
	static Window* g_window;
	// static inline Window* g_window; 「static inline」でメモリ確保できる

	// 各種デバイス
	
	// 描画デバイス
	static GraphicsDevice* g_graphicsDevice;
	//
	static EffekseerDevice* g_effectDevice;
	// オーディオデバイス
	static AudioDevice* g_audioDevice;
	// 入力デバイス
	static RawInputDevice* g_rawInputDevice;

	static DirectInputDevice* g_directInputDevice;

	// fps計測
	static FpsTimer* g_fpsTimer;

	// ゲーム

	// ゲーム管理
	static std::shared_ptr<GameSystem> g_gameSystem;
	// プロファイラ デバッグ
	static std::shared_ptr<ImGuiSystem> g_imGuiSystem;

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
