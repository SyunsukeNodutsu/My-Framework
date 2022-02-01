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
//	 InputDevice    |                Sound3D
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

public:

	//ウィンドウ ※"static inline"でメモリ確保できる
	static Window* g_window;

	//各種デバイス
	static GraphicsDevice* g_graphicsDevice;//描画デバイス
	static EffekseerDevice* g_effectDevice;	//エフェクトデバイス
	static AudioDevice* g_audioDevice;		//オーディオデバイス
	static InputDevice* g_inputDevice;		//入力デバイス

	//TODO: ウィンドウが持った方がいいかも
	static FpsTimer* g_fpsTimer;			//fps計測

	//メインシステム
	static GameSystem* g_gameSystem;		//ゲーム管理
	static ImGuiSystem* g_imGuiSystem;		//プロファイラ デバッグ

private:

	//スレッドごとの値(static)
	static const int m_numPerSceneRenderThreads = 4;//レンダリングスレッド数
	static HANDLE m_hPerSceneRenderDeferredThread[m_numPerSceneRenderThreads];//スレッドハンドル
	static HANDLE m_hBeginPerSceneRenderDeferredEvent[m_numPerSceneRenderThreads];//Beginイベント
	static HANDLE m_hEndPerSceneRenderDeferredEvent[m_numPerSceneRenderThreads];//Endイベント
	static int m_perSceneThreadInstanceData[m_numPerSceneRenderThreads];//
	static ID3D11DeviceContext* m_contextDeferred[m_numPerSceneRenderThreads];
	static ID3D11CommandList* m_commandList[m_numPerSceneRenderThreads];

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

	//
	static inline unsigned int WINAPI PerSceneRenderDeferredProc(LPVOID lparam);

};
