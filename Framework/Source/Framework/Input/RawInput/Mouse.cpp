#include "Mouse.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Mouse::Mouse()
    : m_deviceMouse()
{
    memset(&m_state, 0, sizeof(State));
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void Mouse::Initialize(HWND hwnd)
{
    m_owner = hwnd;

    m_deviceMouse.usUsagePage   = 0x01;
    m_deviceMouse.usUsage       = 0x02;
    m_deviceMouse.dwFlags       = RIDEV_INPUTSINK;
    m_deviceMouse.hwndTarget    = hwnd;// nullptrでもいけるっぽい

    if (RegisterRawInputDevices(&m_deviceMouse, 1, sizeof(RAWINPUTDEVICE)) == FALSE)
        MessageBoxA(nullptr, "WARNING: マウスデバイスの登録に失敗しました.", "Failed", MB_OK);
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void Mouse::Finalize(HWND hwnd)
{
    m_deviceMouse.usUsagePage   = 0x01;
    m_deviceMouse.usUsage       = 0x02;
    m_deviceMouse.dwFlags       = RIDEV_REMOVE;
    m_deviceMouse.hwndTarget    = hwnd;

    if (RegisterRawInputDevices(&m_deviceMouse, 1, sizeof(RAWINPUTDEVICE)) == FALSE)
        DebugLog("WARNING: マウスデバイスの登録解除に失敗しました.\n");
}

//-----------------------------------------------------------------------------
// リフレッシュ (メッセージ更新の前に呼び出し)
//-----------------------------------------------------------------------------
void Mouse::Refresh()
{
#ifndef UPDATE_BUTTON_STATE
#define UPDATE_BUTTON_STATE(field) _mStates.field = static_cast<ButtonState>( ( !!m_state.field ) | ( ( !!m_state.field ^ !!m_lastState.field ) << 1 ) );
#endif

    UPDATE_BUTTON_STATE(mLeftButton);
    UPDATE_BUTTON_STATE(mRightButton);
    UPDATE_BUTTON_STATE(mMiddleButton);
    UPDATE_BUTTON_STATE(mX1Button);
    UPDATE_BUTTON_STATE(mX2Button);

    m_lastState = m_state;

    SetMouseWheelDelta(0);
}

//-----------------------------------------------------------------------------
// マウス情報解析
//-----------------------------------------------------------------------------
void Mouse::ParseMouseData(RAWMOUSE mouse, HWND hwnd)
{
    // ボタン
    SetState(mouse.ulButtons);

    // 座標
    POINT tmpPos; GetCursorPos(&tmpPos);
    ScreenToClient(hwnd, &tmpPos);
    m_mousePos.x = static_cast<float>(tmpPos.x);
    m_mousePos.y = static_cast<float>(tmpPos.y);
}

//-----------------------------------------------------------------------------
// マウス座標設定
//-----------------------------------------------------------------------------
void Mouse::SetAt(float2 position, HWND hwnd, bool abs)
{
    // ウィンドウがフォーカスされていない場合は何もしない
    if (GetFocus() != hwnd)
        return;

    POINT pt = { static_cast<LONG>(position.x), static_cast<LONG>(position.y) };

    if (!abs) {
        m_mousePos.x = position.x;
        m_mousePos.y = position.y;
    }
    else {
        POINT pt2 = { 0, 0 };
        ClientToScreen(hwnd, &pt2);
        m_mousePos.x = position.x - pt2.x;
        m_mousePos.y = position.y - pt2.y;
    }

    // TODO:
    //m_delta.x = 0;
    //m_delta.y = 0;

    if (!abs) ClientToScreen(hwnd, &pt);
    SetCursorPos(pt.x, pt.y);
}

//-----------------------------------------------------------------------------
// 押されている間を返す
//-----------------------------------------------------------------------------
bool Mouse::IsDown(MouseButton mkey)
{
    if (!ApplicationChilled::GetApplication()->g_inputDevice->MouseInTheClient(m_mousePos)) return false;
    return _mStates.KeyState(mkey) == ButtonState::HELD;
}

//-----------------------------------------------------------------------------
// 押された瞬間を返す
//-----------------------------------------------------------------------------
bool Mouse::IsPressed(MouseButton mkey)
{
    if (!ApplicationChilled::GetApplication()->g_inputDevice->MouseInTheClient(m_mousePos)) return false;
    return _mStates.KeyState(mkey) == ButtonState::PRESSED;
}

//-----------------------------------------------------------------------------
// 離された瞬間を返す
//-----------------------------------------------------------------------------
bool Mouse::IsReleased(MouseButton mkey)
{
    if (!ApplicationChilled::GetApplication()->g_inputDevice->MouseInTheClient(m_mousePos)) return false;
    return _mStates.KeyState(mkey) == ButtonState::RELEASED;
}

//-----------------------------------------------------------------------------
// マウス座標を返す
//-----------------------------------------------------------------------------
float2 Mouse::GetMousePos(bool haste)
{
    if (haste)
    {
        // 座標
        POINT tmpPos; GetCursorPos(&tmpPos);
        ScreenToClient(m_owner, &tmpPos);
        m_mousePos.x = static_cast<float>(tmpPos.x);
        m_mousePos.y = static_cast<float>(tmpPos.y);
        return m_mousePos;
    }
    return m_mousePos;
}

//-----------------------------------------------------------------------------
// 状態の設定 (save bit)
//-----------------------------------------------------------------------------
void Mouse::SetState(WPARAM wparam, bool down)
{
    if (wparam < 0 || wparam > Count) return;

    auto ptr = reinterpret_cast<bool*>(&m_state);
    unsigned int bf = 1u << (wparam & 0x1f); // 1u << (w & 0x1f);

    if (down) m_state.val |= bf;
    else m_state.val &= ~bf;
}

//-----------------------------------------------------------------------------
// 状態の設定 (mouse button state)
//-----------------------------------------------------------------------------
void Mouse::SetState(ULONG flags)
{
    // https://docs.microsoft.com/en-us/windows/desktop/api/winuser/ns-winuser-tagrawmouse
    if (flags & RI_MOUSE_LEFT_BUTTON_DOWN) SetState(Left, true);
    if (flags & RI_MOUSE_LEFT_BUTTON_UP) SetState(Left, false);

    if (flags & RI_MOUSE_RIGHT_BUTTON_DOWN) SetState(Right, true);
    if (flags & RI_MOUSE_RIGHT_BUTTON_UP) SetState(Right, false);

    if (flags & RI_MOUSE_MIDDLE_BUTTON_DOWN) SetState(Middle, true);
    if (flags & RI_MOUSE_MIDDLE_BUTTON_UP) SetState(Middle, false);

    // TODO: 修正.もしかしたらLMBの後に呼ばれているかも
    if (flags & RI_MOUSE_BUTTON_1_DOWN) SetState(X1, true);
    if (flags & RI_MOUSE_BUTTON_1_UP) SetState(X1, false);

    if (flags & RI_MOUSE_BUTTON_2_DOWN) SetState(X2, true);
    if (flags & RI_MOUSE_BUTTON_2_UP) SetState(X2, false);
}
