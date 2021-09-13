#include "Window.h"

// imGui
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Window::Window()
	: m_hWnd(nullptr)
	, m_windowInfo()
	, m_className()
	, m_titleName()
{
}

//-----------------------------------------------------------------------------
// 作成
//-----------------------------------------------------------------------------
bool Window::Create(int clientWidth, int clientHeight, const std::string& titleName, const std::string& className)
{
	m_titleName = sjis_to_wide(titleName);
	m_className = sjis_to_wide(className);

	// インスタンスハンドル取得
	HINSTANCE hInstance = GetModuleHandle(0);

	//--------------------------------------------------
	// ウィンドウクラスの登録
	//--------------------------------------------------
	WNDCLASSEX wc = {};
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= &Window::callWindowProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm			= LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor			= LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= nullptr;
	wc.lpszClassName	= m_className.c_str();

	//ウィンドウクラスの登録
	if (!RegisterClassEx(&wc))
		return false;

	//--------------------------------------------------
	// アプリケーション初期化の実行
	//--------------------------------------------------
	m_hWnd = CreateWindowW(
		m_className.c_str(), m_titleName.c_str(),
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
		CW_USEDEFAULT, 0, clientWidth, clientHeight,
		nullptr, nullptr, hInstance,
		this// MDIチャイルドウィンドウを作成する際に指定 ここがnullptrだとcallbackウィンドウプロシージャが呼ばれない
	);

	if (m_hWnd == nullptr)
		return false;

	// クライアントのサイズを設定
	SetClientSize(clientWidth, clientHeight);

	// 反映
	SetForegroundWindow(m_hWnd);
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	// timeGetTime関数の精度を1msに設定
	//timeBeginPeriod(1);

	// WindowInfoを取得しておく
	m_windowInfo.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(m_hWnd, &m_windowInfo);

	SetDesktopCenterWindow(m_hWnd);

	return true;
}

//-----------------------------------------------------------------------------
// 解放
//-----------------------------------------------------------------------------
void Window::Release()
{
	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = nullptr;
	}
}

//-----------------------------------------------------------------------------
// ウィンドウメッセージ処理
//-----------------------------------------------------------------------------
bool Window::ProcessMessage()
{
	// 入力系 リフレッシュ
	RAW_INPUT.Refresh();

	// メッセージ取得
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return false;

		// メッセージ処理
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

//-----------------------------------------------------------------------------
// ウィンドウ関数(Static関数)
//-----------------------------------------------------------------------------
LRESULT CALLBACK Window::callWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// ウィンドウプロパティから、GameWindowクラスのインスタンスを取得
	Window* pThis = (Window*)GetProp(hWnd, L"GameWindowInstance");

	// nullptrの場合は、デフォルト処理を実行
	if (pThis == nullptr)
	{
		switch (message)
		{
		case WM_CREATE:
		{
			// CreateWindow()で渡したパラメータを取得
			CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
			Window* window = (Window*)createStruct->lpCreateParams;

			// ウィンドウプロパティにこのクラスのインスタンスアドレスを埋め込んでおく
			// 次回から、pThis->WindowProcの方へ処理が流れていく
			SetProp(hWnd, L"GameWindowInstance", window);
		}
		return 0;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

	// インスタンス側のWindow関数を実行する
	return pThis->WindowProc(hWnd, message, wParam, lParam);
}

//-----------------------------------------------------------------------------
// ウィンドウ関数
//-----------------------------------------------------------------------------
LRESULT Window::WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	// ImGuiにイベント通知
	if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wparam, lparam))
		return true;

	switch (message)
	{
	// 生入力情報取得
	case WM_INPUT:
	{
		RAW_INPUT.ParseMessage((void*)lparam);
	}
	break;

	// システムがウィンドウを描画するように要求
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		// TODO: hdcを使用する描画コードをここに追加
		EndPaint(hwnd, &ps);
	}
	break;

	case WM_SIZE:
	{
		// TODO: 無意味かも 検証
		if (wparam == SIZE_MAXIMIZED)
		{
			auto info = GetWinInfo();
			auto rcClient = info.rcClient;// クライアントサイズ
			SetClientSize((rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top));
		}
	}
	break;

	// Xボタンが押された
	case WM_CLOSE:
	{
		Release();
	}
	break;

	// ウィンドウ破棄直前
	case WM_DESTROY:
	{
		RemoveProp(hwnd, L"GameWindowInstance");
		PostQuitMessage(0);
	}
	break;

	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
	return 0;
}

//-----------------------------------------------------------------------------
// ウィンドウのクライアントのサイズを指定サイズに設定
//-----------------------------------------------------------------------------
void Window::SetClientSize(int width, int height)
{
	RECT rcWindow, rcClient;

	// ウィンドウのRECT取得
	GetWindowRect(m_hWnd, &rcWindow);
	// クライアント領域のRECT取得
	GetClientRect(m_hWnd, &rcClient);

	int nWidth = width + (rcWindow.right - rcWindow.left) - (rcClient.right - rcClient.left);
	int nHeight = height + (rcWindow.bottom - rcWindow.top) - (rcClient.bottom - rcClient.top);

	// ウィンドウの余白を考えて、クライアントのサイズを指定サイズにする
	MoveWindow(m_hWnd, rcWindow.left, rcWindow.top, nWidth, nHeight, TRUE);
}

//-----------------------------------------------------------------------------
// ウィンドウをデスクトップの中心に移動
//-----------------------------------------------------------------------------
bool Window::SetDesktopCenterWindow(HWND hWnd) const
{
	if (hWnd == nullptr)
		return false;

	// デスクトップ サイズ取得
	RECT desktopRect = {};
	SystemParametersInfo(SPI_GETWORKAREA, 0, &desktopRect, 0);

	// 矩形取得
	RECT windowRect = m_windowInfo.rcWindow;
	auto width = windowRect.right - windowRect.left;
	auto height = windowRect.bottom - windowRect.top;
	// 座標確定
	INT posX = (((desktopRect.right - desktopRect.left) - width) / 2 + desktopRect.left);
	INT posY = (((desktopRect.bottom - desktopRect.top) - height) / 2 + desktopRect.top);

	return SetWindowPos(hWnd, NULL, posX, posY, 0, 0, (SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER));
}
