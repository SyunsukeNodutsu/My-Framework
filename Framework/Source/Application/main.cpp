#include "main.h"
#include "GameSystem.h"
#include "ImGuiSystem.h"

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
	if (!g_window.Create(width, height, "高ぶってきたからTwitterで政治的ツイートするわ。", "Window")) {
		MessageBoxA(nullptr, "Create window failed.", "Failed", MB_OK);
		return false;
	}

	//--------------------------------------------------
	// Direct3D初期化
	//--------------------------------------------------
	MY_DIRECT3D_DESC desc = {};
	desc.m_bufferCount	= 2;
	desc.m_width		= 1536;// 1280-1920-2560
	desc.m_height		= 864; //  720-1080-1440
	desc.m_refreshRate	= 0;
	desc.m_windowed		= true;
	desc.m_useHDR		= false;
	desc.m_useMSAA		= true;
	desc.m_deferredRendering = false;
	desc.m_hwnd			= g_window.GetWndHandle();

	GraphicsDeviceChild::SetGraphicsDevice(&g_graphicsDevice);// テクスチャ作ってるから先に設定
	g_graphicsDevice.Initialize(desc);

	//--------------------------------------------------
	// ゲームオーディオ
	//--------------------------------------------------
	g_audioDevice.Initialize();
	g_audioDevice.SetMasterVolume(0.4f);
	AudioDeviceChild::SetAudioDevice(&g_audioDevice);
	
	//--------------------------------------------------
	// その他
	//--------------------------------------------------

	RENDERER.Initialize();

	// シェーダー
	SHADER.Initialize();
	// 入力 RawInputAPI
	RAW_INPUT.Initialize();

	GAMESYSTEM.Initialize();
	IMGUISYSTEM.Initialize(g_graphicsDevice.g_cpDevice.Get(), g_graphicsDevice.g_cpContext.Get());

	return true;
}

//-----------------------------------------------------------------------------
// アプリケーション終了
//-----------------------------------------------------------------------------
void Application::Release()
{
	// TODO: 終了の順番要調査 特にRAW INPUT

	// アプリケーション
	IMGUISYSTEM.Finalize();
	GAMESYSTEM.Finalize();

	// デバイス
	g_audioDevice.Finalize();
	g_graphicsDevice.Finalize();

	RAW_INPUT.Finalize();

	// ウィンドウ
	g_window.Release();
}

//-----------------------------------------------------------------------------
// アプリケーション実行
//-----------------------------------------------------------------------------
void Application::Execute()
{
	// 初期設定(ウィンドウ作成、Direct3D初期化など)
	if (Initialize(1536, 864) == false)
		return;

	//==================================================
	// 
	// ゲームループ
	// 
	//==================================================

	while (4545)
	{
		if (m_endFlag)
			break;

		//----------------------------------------
		// ウィンドウ関係の処理
		//----------------------------------------

		// ウィンドウのメッセージを処理する
		g_window.ProcessMessage();

		// ウィンドウが破棄されてるならループ終了
		if (!g_window.IsCreated())
			break;

		//----------------------------------------
		// ゲームサウンド処理
		//----------------------------------------

		// カメラ行列の取得
		auto cameraMatrix = GAMESYSTEM.GetCamera()->GetCameraMatrix();
		// サウンド更新
		g_audioDevice.Update(cameraMatrix);

		//----------------------------------------
		// ゲーム処理
		//----------------------------------------
		
		// 更新
		GAMESYSTEM.Update();

		// 描画
		g_graphicsDevice.Begin();
		{
			// 3D想定
			GAMESYSTEM.Draw();
			// 2D想定
			GAMESYSTEM.Draw2D();

			// ImGui 描画
			IMGUISYSTEM.Begin();
			IMGUISYSTEM.DrawImGui();
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
		g_graphicsDevice.End();

		// 描画後更新
		GAMESYSTEM.LateUpdate();
	}

	//==================================================
	// 
	// アプリケーション解放
	// 
	//==================================================
	Release();
}
