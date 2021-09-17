#include "RawInput.h"
#include "../../Application/main.h"
#include "../../Application/ImGuiSystem.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
RawInput::RawInput()
    : m_spKeyboard(nullptr)
    , m_spMouse(nullptr)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void RawInput::Initialize()
{
    m_spKeyboard = std::make_shared<Keyboard>();
    m_spKeyboard->Initialize(APP.g_window.GetWndHandle());

    m_spMouse = std::make_shared<Mouse>();
    m_spMouse->Initialize(APP.g_window.GetWndHandle());

    IMGUISYSTEM.AddLog("INFO: RawInputDevice initialized.");
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void RawInput::Finalize()
{
    m_spKeyboard->Finalize(APP.g_window.GetWndHandle());
    m_spMouse->Finalize(APP.g_window.GetWndHandle());
}

//-----------------------------------------------------------------------------
// 次回の入力判定の準備
//-----------------------------------------------------------------------------
void RawInput::Refresh()
{
    m_spKeyboard->Refresh();
    m_spMouse->Refresh();
}

//-----------------------------------------------------------------------------
// メッセージ解析
//-----------------------------------------------------------------------------
void RawInput::ParseMessage(void* lparam)
{
    if (lparam == nullptr)
        return;

    // TODO: castちょっとよくない
    auto hRawInput = reinterpret_cast<HRAWINPUT>(lparam);

    // バッファサイズを取得
    UINT uSize;
    if (GetRawInputData(hRawInput, RID_INPUT, NULL, &uSize, sizeof(RAWINPUTHEADER)) != 0) {
        assert(0 && "エラー：バッファサイズの取得に失敗.");
        return;
    }

    // 入力データを取得
    // GetRawInputData()を2回呼ぶことで入力データを取得できる
    auto lpbyData = new BYTE[uSize];
    if (GetRawInputData(hRawInput, RID_INPUT, lpbyData, &uSize, sizeof(RAWINPUTHEADER)) != uSize) {
        delete[] lpbyData;
        assert(0 && "エラー：入力データの取得に失敗.");
        return;
    }

    //--------------------------------------------------
    // マウス/キーボードの入力情報を取得
    //--------------------------------------------------

    // 入力データ 参照
    auto lprawinput = reinterpret_cast<RAWINPUT*>(lpbyData);

    // キーボードの場合
    if (lprawinput->header.dwType == RIM_TYPEKEYBOARD)
        m_spKeyboard->ParseKeyboardData(lprawinput->data.keyboard);

    // マウスの場合
    else if (lprawinput->header.dwType == RIM_TYPEMOUSE)
        m_spMouse->ParseMouseData(lprawinput->data.mouse, APP.g_window.GetWndHandle());

    // 入力データ解放
    delete[] lpbyData;
}
