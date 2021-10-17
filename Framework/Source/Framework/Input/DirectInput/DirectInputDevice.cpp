#include "DirectInputDevice.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
DirectInputDevice::DirectInputDevice()
    : m_pInputInterface(nullptr)
    , m_pMouseDevice(nullptr)
    , m_pKeyboardDevice(nullptr)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool DirectInputDevice::Initialize(HWND hwnd)
{
    // インターフェースの作成 DirectInputサブシステムに登録
    if (FAILED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION,
        IID_IDirectInput8, (VOID**)&m_pInputInterface, nullptr))) {
        assert(0 && "インターフェースの作成に失敗.");
        return false;
    }

    //--------------------------------------------------
    // マウス
    //--------------------------------------------------
    {
        // システムマウスの取得
        if (FAILED(m_pInputInterface->CreateDevice(GUID_SysMouse, &m_pMouseDevice, nullptr))) {
            assert(0 && "システムマウスの取得に失敗.");
            return false;
        }

        // データフォーマットの設定
        // TODO: カスタムフォーマットを設定する 相対座標->絶対座標
        if (FAILED(m_pMouseDevice->SetDataFormat(&c_dfDIMouse))) {
            assert(0 && "データフォーマットの設定に失敗.");
            return false;
        }

        // 協調レベルの設定
        if (FAILED(m_pMouseDevice->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND))) {
            assert(0 && "協調レベルの設定に失敗.");
            return false;
        }
    }

    //--------------------------------------------------
    // キーボード
    //--------------------------------------------------
    {
        // システムキーボードの取得
        if (FAILED(m_pInputInterface->CreateDevice(GUID_SysKeyboard, &m_pKeyboardDevice, nullptr))) {
            assert(0 && "システムキーボードの取得に失敗.");
            return false;
        }

        // データフォーマットの設定
        if (FAILED(m_pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard))) {
            assert(0 && "データフォーマットの設定に失敗.");
            return false;
        }

        // 協調レベルの設定
        if (FAILED(m_pKeyboardDevice->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND))) {
            assert(0 && "協調レベルの設定に失敗.");
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void DirectInputDevice::Finalize()
{
    // デバイスをアンアクイジション
    if (m_pMouseDevice) m_pMouseDevice->Unacquire();
    if (m_pKeyboardDevice) m_pKeyboardDevice->Unacquire();

    // 解放
    if (m_pMouseDevice) m_pMouseDevice->Release();
    if (m_pKeyboardDevice) m_pKeyboardDevice->Release();
    if (m_pInputInterface) m_pInputInterface->Release();
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void DirectInputDevice::Update(HWND hwnd)
{

}
