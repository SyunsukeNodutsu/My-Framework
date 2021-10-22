#include "main.h"

Window* Application::g_window = 0;
GraphicsDevice* Application::g_graphicsDevice = 0;
EffekseerDevice* Application::g_effectDevice = 0;
AudioDevice* Application::g_audioDevice = 0;
RawInputDevice* Application::g_rawInputDevice = 0;
DirectInputDevice* Application::g_directInputDevice = 0;
FpsTimer* Application::g_fpsTimer = 0;

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
	g_window = new Window();
	if (!g_window->Create(width, height, "My framework", "Window")) {
		MessageBoxA(nullptr, "Create window failed.", "Failed", MB_OK);
		return false;
	}

	//--------------------------------------------------
	// 各種デバイス
	//--------------------------------------------------

	g_graphicsDevice	= new GraphicsDevice();
	g_effectDevice		= new EffekseerDevice();
	g_audioDevice		= new AudioDevice();
	g_rawInputDevice	= new RawInputDevice();
	g_directInputDevice = new DirectInputDevice();
	g_fpsTimer			= new FpsTimer();

	g_gameSystem	= std::make_shared<GameSystem>();
	g_imGuiSystem	= std::make_shared<ImGuiSystem>();

	// デバイスセット
	GraphicsDeviceChild::SetGraphicsDevice(g_graphicsDevice);
	EffekseerDeviceChild::SetEffekseerDevice(g_effectDevice);
	AudioDeviceChild::SetAudioDevice(g_audioDevice);

	// 描画デバイス Direct3D
	MY_DIRECT3D_DESC desc = {};
	desc.m_bufferCount	= 2;
	desc.m_width		= width;// 1280-1920-2560
	desc.m_height		= height; //  720-1080-1440
	desc.m_refreshRate	= 0;
	desc.m_windowed		= true;
	desc.m_useHDR		= false;
	desc.m_useMSAA		= true;
	desc.m_debugMode	= false;
	desc.m_hwnd			= g_window->GetWndHandle();
	g_graphicsDevice->Initialize(desc);

	// エフェクトデバイス
	g_effectDevice->Initialize();

	// オーディオデバイス
	g_audioDevice->Initialize();
	g_audioDevice->SetMasterVolume(0.2f);
	
	// 入力デバイス
	g_rawInputDevice->Initialize();
	g_directInputDevice->Initialize(g_window->GetWndHandle());

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
	// アプリケーション
	g_imGuiSystem->Finalize();
	g_gameSystem->Finalize();

	// デバイス
	g_directInputDevice->Finalize();
	g_rawInputDevice->Finalize();
	g_audioDevice->Finalize();
	g_effectDevice->Finalize();
	g_graphicsDevice->Finalize();

	// ウィンドウ
	g_window->Release();

	delete g_graphicsDevice;
	delete g_effectDevice;
	delete g_audioDevice;
	delete g_directInputDevice;
	delete g_rawInputDevice;
	delete g_fpsTimer;

	delete g_window;
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
		if (m_endFlag) break;

		g_fpsTimer->Tick();

		//----------------------------------------
		// ウィンドウ関係の処理
		//----------------------------------------

		// ウィンドウのメッセージを処理する
		if (!g_window->ProcessMessage())
			break;

		// ウィンドウが破棄されてるならループ終了
		if (!g_window->IsCreated())
			break;

		//----------------------------------------
		// ゲームサウンド処理
		//----------------------------------------

		// カメラ行列の取得
		const auto& cameraMatrix = g_gameSystem->g_cameraSystem.GetCamera()->GetCameraMatrix();
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

			// エフェクト描画
			g_effectDevice->Update(static_cast<float>(g_fpsTimer->GetDeltaTime()));

			// 2D想定 描画は最も最後
			g_gameSystem->Draw2D();

			// ImGui 描画
			g_imGuiSystem->Begin();
			g_imGuiSystem->DrawImGui();
			g_imGuiSystem->End();
		}
		g_graphicsDevice->End();

		// 描画後更新
		g_gameSystem->LateUpdate();

		//----------------------------------------
		// FPS制御
		//----------------------------------------

		// TODO: どうしても最大画面時のPresentの不具合が治らなければここで
	}

	//==================================================
	// 
	// アプリケーション解放
	// 
	//==================================================
	Release();
}
