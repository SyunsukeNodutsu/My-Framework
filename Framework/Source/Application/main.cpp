#include "main.h"

//ウィンドウ
Window* Application::g_window = 0;
//デバイス
GraphicsDevice* Application::g_graphicsDevice = 0;
EffekseerDevice* Application::g_effectDevice = 0;
AudioDevice* Application::g_audioDevice = 0;
InputDevice* Application::g_inputDevice = 0;
FpsTimer* Application::g_fpsTimer = 0;
//メインシステム
GameSystem* Application::g_gameSystem = 0;
ImGuiSystem* Application::g_imGuiSystem = 0;

//スレッド
HANDLE Application::m_hPerSceneRenderDeferredThread[m_numPerSceneRenderThreads] = { 0 };
HANDLE Application::m_hBeginPerSceneRenderDeferredEvent[m_numPerSceneRenderThreads] = { 0 };
HANDLE Application::m_hEndPerSceneRenderDeferredEvent[m_numPerSceneRenderThreads] = { 0 };
int Application::m_perSceneThreadInstanceData[m_numPerSceneRenderThreads] = { 0 };
ID3D11DeviceContext* Application::m_contextDeferred[m_numPerSceneRenderThreads] = { nullptr };
ID3D11CommandList* Application::m_commandList[m_numPerSceneRenderThreads] = { nullptr };

bool Application::m_bClearStateUponBeginCommandList = false;
bool Application::m_bClearStateUponFinishCommandList = false;
bool Application::m_bClearStateUponExecuteCommandList = false;

//-----------------------------------------------------------------------------
// メインエントリ
//-----------------------------------------------------------------------------
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	//アプリケーション実行前の初期化
	
	//不使用な引数をコンパイラに伝えてWarningを抑制
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#if _DEBUG
	//メモリリークの検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//Windows10 CreatorsUpdateで入ったモニタ単位でDPIが違う環境への対応
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	//COMの初期化
	Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
	if (FAILED(initialize)) {
		MessageBoxA(nullptr, "COM initialization failed.", "Failed", MB_OK);
		return -1;
	}

	//DirectXMathの計算に対応しているCPUか確認
	if (!DirectX::XMVerifyCPUSupport()) {
		MessageBoxA(nullptr, "DirectXMath is not supported.", "Failed", MB_OK);
		return -1;
	}

	//アプリケーション実行
	Application* application = new Application();
	ApplicationChilled::SetApplication(application);

	application->Execute();

	delete application;
	
	//COM解放
	CoUninitialize();

	return 0;
}

//-----------------------------------------------------------------------------
// アプリケーション初期設定
//-----------------------------------------------------------------------------
bool Application::Initialize(int width, int height)
{
	//--------------------------------------------------
	//ウィンドウ初期化
	//--------------------------------------------------

	//ウィンドウ作成
	g_window = new Window();
	if (!g_window->Create(width, height, "My framework", "Window")) {
		MessageBoxA(nullptr, "Create window failed.", "Failed", MB_OK);
		return false;
	}

	//--------------------------------------------------
	//フレームワーク・デバイス初期化
	//--------------------------------------------------

	//各種デバイス生成
	g_graphicsDevice	= new GraphicsDevice();
	g_effectDevice		= new EffekseerDevice();
	g_audioDevice		= new AudioDevice();
	g_inputDevice		= new InputDevice();
	g_fpsTimer			= new FpsTimer();
	//メインシステム生成
	g_gameSystem		= new GameSystem();
	g_imGuiSystem		= new ImGuiSystem();

	//デバイスセット
	GraphicsDeviceChild::SetGraphicsDevice(g_graphicsDevice);
	EffekseerDeviceChild::SetEffekseerDevice(g_effectDevice);
	AudioDeviceChild::SetAudioDevice(g_audioDevice);

	//描画デバイス Direct3D
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

	//エフェクトデバイス
	g_effectDevice->Initialize();

	//オーディオデバイス
	g_audioDevice->Initialize();
	g_audioDevice->SetMasterVolume(0.08f);
	
	//入力デバイス
	g_inputDevice->Initialize(g_window->GetWndHandle(), InputMode::eRawInput);

	//その他

	//レンダラー(定数バッファとか)
	RENDERER.Initialize();

	//シェーダー
	SHADER.Initialize();

	//アプリケーション

	//ゲーム
	g_gameSystem->Initialize();

	//imGui(プロファイラー)
	g_imGuiSystem->Initialize(g_graphicsDevice->g_cpDevice.Get(), g_graphicsDevice->g_cpContext.Get());

	//--------------------------------------------------
	//スレッド
	//--------------------------------------------------
#ifdef DEBUG
	// これらのチェックは、D3D状態の暗黙の前提であるキャリーオーバーを回避するために重要である。
	// デバイスのコンテキストをまたいで  マルチスレッドレンダリングにおける非常に一般的なエラーの原因である
	// あるコンテキストで何らかの状態を設定し、不注意にその状態に依存することです。
	// 別のコンテキスト  これらのフラグをすべて true に設定することで、そのようなエラーをすべて明らかにすることができます。
	// (自明ではないパフォーマンスコストで)。
	//
	// 名前にはもう少し意味があります。 フラグは、以下の場合に状態をクリアすることを強制する。
	//
	// 1) 問題のアクションを実際に実行した (例: FinishCommandList を呼び出した)。
	// 2) フレーム内で、そのアクションが実行されたかもしれない任意の時点に到達したとき
	// 例えば、DEVICECONTEXT_IMMEDIATEを使用しているが、そうでなければ
	// FinishCommandList を呼び出したはずです)。
	//
	// この使い方は、様々な経路で一貫した動作を保証するものです。
	//
	m_bClearStateUponBeginCommandList = true;
	m_bClearStateUponFinishCommandList = true;
	m_bClearStateUponExecuteCommandList = true;
#endif

	for (int i = 0; i < m_numPerSceneRenderThreads; i++)
	{
		m_perSceneThreadInstanceData[i] = i;

		m_hBeginPerSceneRenderDeferredEvent[i] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		m_hEndPerSceneRenderDeferredEvent[i] = CreateEvent(nullptr, FALSE, FALSE, nullptr);

		//遅延コンテキスト作成
		g_graphicsDevice->g_cpDevice->CreateDeferredContext(0, &m_contextDeferred[i]);

		//作成
		m_hPerSceneRenderDeferredThread[i] = (HANDLE)_beginthreadex(
			nullptr,
			0,
			PerSceneRenderDeferredProc,// thread関数
			&m_perSceneThreadInstanceData[i],// thread関数への引数
			CREATE_SUSPENDED,// 作成option
			nullptr// thread ID
		);

		ResumeThread(m_hPerSceneRenderDeferredThread[i]);
	}

	return true;
}

//-----------------------------------------------------------------------------
// アプリケーション終了
//-----------------------------------------------------------------------------
void Application::Release()
{
	//スレッド
	for (int i = 0; i < m_numPerSceneRenderThreads; i++)
	{
		CloseHandle(m_hPerSceneRenderDeferredThread[i]);
		CloseHandle(m_hEndPerSceneRenderDeferredEvent[i]);
		CloseHandle(m_hBeginPerSceneRenderDeferredEvent[i]);
		SafeRelease(m_contextDeferred[i]);
	}

	//アプリケーション
	g_imGuiSystem->Finalize();
	g_gameSystem->Finalize();

	//デバイス
	g_inputDevice->Finalize();
	g_audioDevice->Finalize();
	g_effectDevice->Finalize();
	g_graphicsDevice->Finalize();

	//ウィンドウ
	g_window->Release();

	//解放
	delete g_imGuiSystem;
	delete g_gameSystem;

	delete g_graphicsDevice;
	delete g_effectDevice;
	delete g_audioDevice;
	delete g_inputDevice;
	delete g_fpsTimer;
	delete g_window;
}

//-----------------------------------------------------------------------------
// スレッド関数
//-----------------------------------------------------------------------------
inline unsigned int __stdcall Application::PerSceneRenderDeferredProc(LPVOID lparam)
{
	//スレッドのローカルデータを取得
	const int instance = *(int*)lparam;
	ID3D11DeviceContext* pd3dDeferredContext = m_contextDeferred[instance];
	ID3D11CommandList*& pd3dCommandList = m_commandList[instance];

	for (;;)
	{
		//メインスレッドからの準備完了の合図を待機
		WaitForSingleObject(m_hBeginPerSceneRenderDeferredEvent[instance], INFINITE);

		if (m_bClearStateUponBeginCommandList)
			pd3dDeferredContext->ClearState();

		//ここで描画
		//例) RenderDirect(pd3dDeferredContext);

		//グラフィックコマンドを記録
		pd3dDeferredContext->FinishCommandList(!m_bClearStateUponFinishCommandList, &pd3dCommandList);

		//メインスレッドのコマンドリストが終了したことを送信
		SetEvent(m_hEndPerSceneRenderDeferredEvent[instance]);
	}

	return 0;
}

//-----------------------------------------------------------------------------
// アプリケーション実行
//-----------------------------------------------------------------------------
void Application::Execute()
{
	// 初期設定(ウィンドウ作成、Direct3D初期化など)
	if (Initialize(1600, 900) == false) return;

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

		//カメラ行列の取得
		const auto& cameraMatrix = g_gameSystem->IsLoadingDone() ? g_gameSystem->g_cameraSystem.GetCamera()->GetCameraMatrix() : mfloat4x4::Identity;
		//サウンド更新
		g_audioDevice->Update(cameraMatrix);

		//----------------------------------------
		// ゲーム処理
		//----------------------------------------
		
		// 更新
		g_gameSystem->Update();
		g_effectDevice->Update();

		// 描画
		g_graphicsDevice->Begin();
		{
			//-------------------------------
			// すべてのワーカースレッドにシグナルを送り 完了を待つ
			for (int iInstance = 0; iInstance < m_numPerSceneRenderThreads; ++iInstance)
			{
				//シーンのキックオフに向けたシグナルの準備
				SetEvent(m_hBeginPerSceneRenderDeferredEvent[iInstance]);
			}

			//完成を待機
			WaitForMultipleObjects(
				m_numPerSceneRenderThreads,
				m_hEndPerSceneRenderDeferredEvent,
				TRUE,
				INFINITE
			);

			for (int iInstance = 0; iInstance < m_numPerSceneRenderThreads; ++iInstance)
			{
				g_graphicsDevice->g_cpContext->ExecuteCommandList(
					m_commandList[iInstance],
					!m_bClearStateUponExecuteCommandList
				);
				SafeRelease(m_commandList[iInstance]);
			}
			//-------------------------------

			//3D想定
			g_gameSystem->Draw();

			//エフェクト
			g_effectDevice->Draw();

			//2D想定
			g_gameSystem->Draw2D();

			//ImGui 描画
			g_imGuiSystem->Begin();
			g_imGuiSystem->DrawImGui();
			g_imGuiSystem->End();
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
