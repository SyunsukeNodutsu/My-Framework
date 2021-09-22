﻿#include "main.h"

std::shared_ptr<Window> Application::g_window = 0;
std::shared_ptr<GraphicsDevice> Application::g_graphicsDevice = 0;
std::shared_ptr<AudioDevice> Application::g_audioDevice = 0;
std::shared_ptr<RawInputDevice> Application::g_rawInputDevice = 0;
std::shared_ptr<FpsTimer> Application::g_fpsTimer = 0;

std::shared_ptr<GameSystem> Application::g_gameSystem = 0;
std::shared_ptr<ImGuiSystem> Application::g_imGuiSystem = 0;

//-----------------------------------------------------------------------------
// メインエントリ
//-----------------------------------------------------------------------------
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	//--------------------------------------------------
	// アプリケーション実行前の初期化
	//--------------------------------------------------

	// 不使用な引数をコンパイラに伝えてWarningを抑制
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#if _DEBUG
	// メモリリークの検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// Windows10 CreatorsUpdateで入ったモニタ単位でDPIが違う環境への対応
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	// COMの初期化
	Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
	if (FAILED(initialize)) {
		MessageBoxA(nullptr, "COM initialization failed.", "Failed", MB_OK);
		return -1;
	}

	// DirectXMathの計算に対応しているCPUか確認
	if (!DirectX::XMVerifyCPUSupport()) {
		MessageBoxA(nullptr, "DirectXMath is not supported.", "Failed", MB_OK);
		return -1;
	}

	//--------------------------------------------------
	// アプリケーション実行
	//--------------------------------------------------
	APP.Execute();

	//--------------------------------------------------
	// 終了
	//--------------------------------------------------

	// COM解放
	CoUninitialize();

	return 0;
}

//-----------------------------------------------------------------------------
// アプリケーション初期設定
//-----------------------------------------------------------------------------
bool Application::Initialize(int width, int height)
{
	//--------------------------------------------------
	// ウィンドウ初期化
	//--------------------------------------------------

	// ウィンドウ作成
	g_window = std::make_shared<Window>();
	if (!g_window->Create(width, height, "xxx Game", "Window")) {
		MessageBoxA(nullptr, "Create window failed.", "Failed", MB_OK);
		return false;
	}

	//--------------------------------------------------
	// 各種デバイス
	//--------------------------------------------------

	g_graphicsDevice	= std::make_shared<GraphicsDevice>();
	g_audioDevice		= std::make_shared<AudioDevice>();
	g_rawInputDevice	= std::make_shared<RawInputDevice>();
	g_fpsTimer			= std::make_shared<FpsTimer>();
	g_gameSystem		= std::make_shared<GameSystem>();
	g_imGuiSystem		= std::make_shared<ImGuiSystem>();
	GraphicsDeviceChild::SetGraphicsDevice(g_graphicsDevice.get());
	AudioDeviceChild::SetAudioDevice(g_audioDevice.get());

	// 描画デバイス Direct3D
	MY_DIRECT3D_DESC desc = {};
	desc.m_bufferCount	= 2;
	desc.m_width		= width;// 1280-1920-2560
	desc.m_height		= height; //  720-1080-1440
	desc.m_refreshRate	= 0;
	desc.m_windowed		= true;
	desc.m_useHDR		= false;
	desc.m_useMSAA		= true;
	desc.m_deferredRendering = false;
	desc.m_hwnd			= g_window->GetWndHandle();

	g_graphicsDevice->Initialize(desc);

	// オーディオデバイス
	g_audioDevice->Initialize();
	g_audioDevice->SetMasterVolume(0.4f);
	
	// 入力デバイス
	g_rawInputDevice->Initialize();

	//--------------------------------------------------
	// その他
	//--------------------------------------------------

	// レンダラー(定数バッファとか)
	RENDERER.Initialize();

	// シェーダー
	SHADER.Initialize();
	
	//--------------------------------------------------
	// アプリケーション
	//--------------------------------------------------

	// ゲーム
	g_gameSystem->Initialize();

	// imGui(プロファイル)
	g_imGuiSystem->Initialize(g_graphicsDevice->g_cpDevice.Get(), g_graphicsDevice->g_cpContext.Get());

	return true;
}

//-----------------------------------------------------------------------------
// アプリケーション終了
//-----------------------------------------------------------------------------
void Application::Release()
{
	// TODO: 終了の順番要調査 特にRAW INPUT

	// アプリケーション
	g_imGuiSystem->Finalize();
	g_gameSystem->Finalize();

	// デバイス
	g_rawInputDevice->Finalize();
	g_audioDevice->Finalize();
	g_graphicsDevice->Finalize();

	// ウィンドウ
	g_window->Release();
}

//-----------------------------------------------------------------------------
// アプリケーション実行
//-----------------------------------------------------------------------------
void Application::Execute()
{
	// 初期設定(ウィンドウ作成、Direct3D初期化など)
	if (Initialize(1600, 900) == false)
		return;

	//==================================================
	// 
	// ゲームループ
	// 
	//==================================================

	while (1)
	{
		if (m_endFlag)
			break;

		g_fpsTimer->Tick();

		//----------------------------------------
		// ウィンドウ関係の処理
		//----------------------------------------

		// ウィンドウのメッセージを処理する
		if (g_window->ProcessMessage() == false)
			break;

		// ウィンドウが破棄されてるならループ終了
		if (!g_window->IsCreated())
			break;

		//----------------------------------------
		// ゲームサウンド処理
		//----------------------------------------

		// カメラ行列の取得
		auto cameraMatrix = g_gameSystem->GetCamera()->GetCameraMatrix();
		// サウンド更新
		g_audioDevice->Update(cameraMatrix);

		//----------------------------------------
		// ゲーム処理
		//----------------------------------------
		
		// 更新
		g_gameSystem->Update();

		// 描画
		g_graphicsDevice->Begin();
		{
			// 3D想定
			g_gameSystem->Draw();
			// 2D想定
			g_gameSystem->Draw2D();

			// ImGui 描画
			g_imGuiSystem->Begin();
			g_imGuiSystem->DrawImGui();
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
		g_graphicsDevice->End();

		// 描画後更新
		g_gameSystem->LateUpdate();
	}

	//==================================================
	// 
	// アプリケーション解放
	// 
	//==================================================
	Release();
}
