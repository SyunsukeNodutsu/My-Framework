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
	if (!g_window.Create(width, height, "Framework test.", "Window")) {
		MessageBoxA(nullptr, "Create window failed.", "Failed", MB_OK);
		return false;
	}

	//--------------------------------------------------
	// Direct3D初期化
	//--------------------------------------------------

	// フルスクリーン確認
	bool isFullScreen = false;
	auto result = IDNO;// MessageBoxA(g_window.GetWndHandle(), "フルスクリーンにしますか？", "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
	if (result == IDYES)
		isFullScreen = true;

	// デバイスのデバッグモードを有効にする
	bool deviceDebugMode = false;
#ifdef _DEBUG
	deviceDebugMode = true;
#endif

	// 解像度はさすがにフルHD以上
	constexpr int resolution_width = 1536;	// 1280-1920-2560
	constexpr int resolution_height = 864; //  720-1080-1440

	// Direct3D初期化
	std::string errorMessage = {};
	bool done = D3D.Create(
		g_window.GetWndHandle(), false, resolution_width, resolution_height,
		false, deviceDebugMode, errorMessage
	);
	if (!done) {
		MessageBoxA(g_window.GetWndHandle(), errorMessage.c_str(), "Failed", MB_OK | MB_ICONSTOP);
		return false;
	}

	// フルスクリーン
	if (isFullScreen)
		D3D.GetSwapChain()->SetFullscreenState(TRUE, 0);

	//--------------------------------------------------
	// ImGui初期化
	//--------------------------------------------------
	IMGUISYSTEM.Initialize();

	//--------------------------------------------------
	// その他 初期化
	//--------------------------------------------------

	// シェーダー
	SHADER.Initialize();

	// 入力 RawInputAPI
	RAW_INPUT.Initialize();

	// GameSystem初期化
	GAMESYSTEM.Initialize();

	return true;
}

//-----------------------------------------------------------------------------
// アプリケーション終了
//-----------------------------------------------------------------------------
void Application::Release()
{
	IMGUISYSTEM.Finalize();
	GAMESYSTEM.Finalize();

	D3D.Release();
	RAW_INPUT.Finalize();

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

	while (1)
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

		// カメラ座標
		float3 camera_pos = float3(0, 0, 0);
		// カメラZ軸方向
		float3 camera_dir = float3(0, 0, 1);

		//AUDIO.Update(camera_pos, camera_dir);

		//----------------------------------------
		// ゲーム処理
		//----------------------------------------
		
		// 更新
		GAMESYSTEM.Update();

		// 描画
		D3D.Begin();
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
		D3D.End();

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
