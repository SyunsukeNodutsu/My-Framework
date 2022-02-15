//-----------------------------------------------------------------------------
// File: main.h
// 
// ■フレームワーク全体像(2022/02/04)
// 
// Application(main.h)------AudioDevice
//   |    |  |                   |
// Window |  |             AudioDeviceChild
//		  |  |                |        |
//		  |  ┗-----┓  SoundDirector Sound
//		  |         |                  |
//	 InputDevice    |                Sound3D
//	  |       |     |
// Keyboard   Mouse |
//					|
//			  GraphicsDevice                         Model---Animation
//					|                                |   |
//			GraphicsDeviceChild---------------------Mesh |
//			 |      |       |  ┗-------┓               |
//		   Buffer Texture Shader   |  EffectDevice   Material
//			 |              |      |         |
//	   ConstantBuffer -- 各Shader  |   EffectDeviceChild
//			 |                     |
//			 ┗------------------Renderer
// 
// ■使用ライブラリ
// 外部 ---
// DirectXTK
// DirectXTex
// Dear imGui
// Effekseer(1.62a)
// 学内配布 ---
// json
// gltfLoader
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

	//@brief 編集モードを設定する
	//@param onoff 編集モードにするかどうか
	void SetEditorMode(bool onoff) { m_editorMode = onoff; }

	//@brief ブラーの値を設定する
	void SetBlurValue(float2 blurValue) { m_blurValue = blurValue; }

	//@brief ImGuiDockingWindowの際にテクスチャとして使用するRTを返す
	Texture* GetScreenRT() { return  m_spScreenRT.get(); }

	//@brief 高輝度抽出テクスチャを返す
	Texture* GetHeightBrightRT() { return  m_spHeightBrightTex.get(); }

public:

	//ウィンドウ ※"static inline"でメモリ確保できる
	static Window* g_window;

	//各種デバイス
	static GraphicsDevice* g_graphicsDevice;//描画デバイス
	static EffekseerDevice* g_effectDevice;	//エフェクトデバイス
	static AudioDevice* g_audioDevice;		//オーディオデバイス
	static InputDevice* g_inputDevice;		//入力デバイス
	static FpsTimer* g_fpsTimer;			//fps計測

	/*
	TODO: こっちのがいい(static)
	const auto& horizontal = Input::GetAxis("Horizontal");
	const auto& deltaTime = Time.deltaTime;
	*/

	//メインシステム
	static GameSystem* g_gameSystem;		//ゲーム管理
	static ImGuiSystem* g_imGuiSystem;		//プロファイラ デバッグ

private:

	bool m_endFlag = false;//ゲーム終了フラグ
	bool m_editorMode = false;//ImGuiによる編集モード

	//ポストプロセス用
	std::shared_ptr<Texture> m_spScreenRT;//加工用レンダーターゲット
	std::shared_ptr<Texture> m_spScreenZ;
	std::shared_ptr<Texture> m_spHeightBrightTex;//ブルーム用
	BlurTexture m_blurTex;//ブラー用
	float2 m_blurValue;

private:

	//@brief アプリケーション初期化
	//@param width ウィンドウ ビューポート幅
	//@param height ウィンドウ ビューポート高さ
	//@return 成功...true 失敗...false
	bool Initialize(int width, int height);

	//@brief アプリケーション解放
	void Release();

};
