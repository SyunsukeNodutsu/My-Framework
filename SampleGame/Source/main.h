//-----------------------------------------------------------------------------
// File: main.h
// 
// 別プロジェクトに移動中
//-----------------------------------------------------------------------------
#pragma once

class GameSystem;
class ImGuiSystem;

//アプリケーションクラス
class Application
{
public:

	//@brief アプリケーション実行
	void Execute();

	//@brief アプリケーションを終了させる
	void End() { m_endFlag = true; }

public:

	//ウィンドウ ※"static inline"でメモリ確保できる
	static Window* g_window;

	//各種デバイス
	static GraphicsDevice* g_graphicsDevice;//描画デバイス
	static EffekseerDevice* g_effectDevice;//エフェクトデバイス
	static AudioDevice* g_audioDevice;//オーディオデバイス
	static InputDevice* g_inputDevice;//入力デバイス

	//TODO: ウィンドウが持った方がいいかも
	static FpsTimer* g_fpsTimer;//fps計測

	//メインシステム
	static GameSystem* g_gameSystem;//ゲーム管理
	static ImGuiSystem* g_imGuiSystem;//プロファイラ デバッグ

private:

	//ゲーム終了フラグ
	bool m_endFlag = false;

private:

	//@brief アプリケーション初期化
	//@param width ウィンドウ ビューポート幅
	//@param height ウィンドウ ビューポート高さ
	//@return 成功...true 失敗...false
	bool Initialize(int width, int height);

	//@brief アプリケーション解放
	void Release();

};
