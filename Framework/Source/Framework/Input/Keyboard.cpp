#include "Keyboard.h"

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void Keyboard::Initialize(HWND hwnd)
{
    if (hwnd == nullptr)
        return;

    m_deviceKeyboard.usUsagePage = 0x01;
    m_deviceKeyboard.usUsage     = 0x06;
    m_deviceKeyboard.dwFlags     = RIDEV_INPUTSINK;
    m_deviceKeyboard.hwndTarget  = hwnd;

    if (RegisterRawInputDevices(&m_deviceKeyboard, 1, sizeof(RAWINPUTDEVICE)) == FALSE)
        MessageBoxA(nullptr, "Register KeyboardDevices failed.", "Failed", MB_OK);

    m_isDown.fill(false);
    m_isPressed.fill(false);
    m_isReleased.fill(false);
    m_isPush.fill(false);
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void Keyboard::Finalize(HWND hwnd)
{
    if (hwnd == nullptr)
        return;

    m_deviceKeyboard.usUsagePage = 0x01;
    m_deviceKeyboard.usUsage     = 0x06;
    m_deviceKeyboard.dwFlags     = RIDEV_REMOVE;// 登録解除
    m_deviceKeyboard.hwndTarget  = hwnd;

    if (RegisterRawInputDevices(&m_deviceKeyboard, 1, sizeof(RAWINPUTDEVICE)) == FALSE)
        MessageBoxA(nullptr, "Register KeyboardDevices failed.", "Failed", MB_OK);
}

//-----------------------------------------------------------------------------
// 次の入力判定の準備
//-----------------------------------------------------------------------------
void Keyboard::Refresh()
{
    m_isPressed.fill(false);
    m_isReleased.fill(false);
}

//-----------------------------------------------------------------------------
// キーボード情報解析
//-----------------------------------------------------------------------------
void Keyboard::ParseKeyboardData(RAWKEYBOARD keyboard)
{
    // 押されている
    m_isDown[keyboard.VKey] = (keyboard.Flags & RI_KEY_BREAK) == 0;

    switch (keyboard.Message)
    {
        // キーを離した
    case WM_KEYUP:
    {
        m_isReleased[keyboard.VKey] = true;
        m_isPressed[keyboard.VKey] = false;
        m_isPush[keyboard.VKey] = false;
    }
    break;

    // キーを押した
    case WM_KEYDOWN:
    {
        if (!m_isPush[keyboard.VKey])
        {
            m_isPressed[keyboard.VKey] = true;
            m_isPush[keyboard.VKey] = true;
        }
    }
    break;
    }
}
