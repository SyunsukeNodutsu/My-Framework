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
bool DirectInputDevice::Initialize(HWND hwnd, DWORD priorityLevel)
{
    // インターフェースの作成 DirectInputサブシステムに登録
    if (FAILED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION,
        IID_IDirectInput8, (VOID**)&m_pInputInterface, nullptr))) {
        DebugLog("インターフェースの作成に失敗.");
        return false;
    }

    //--------------------------------------------------
    // マウス
    //--------------------------------------------------
    {
        if (FAILED(m_pInputInterface->CreateDevice(GUID_SysMouse, &m_pMouseDevice, nullptr))) {
            DebugLog("システムマウスの取得に失敗.");
            return false;
        }

        // TODO: カスタムフォーマットを設定する 相対座標->絶対座標
        if (FAILED(m_pMouseDevice->SetDataFormat(&c_dfDIMouse))) {
            DebugLog("データフォーマットの設定に失敗.");
            return false;
        }

        if (FAILED(m_pMouseDevice->SetCooperativeLevel(hwnd, priorityLevel))) {
            DebugLog("協調レベルの設定に失敗.");
            return false;
        }
    }

    //--------------------------------------------------
    // キーボード
    //--------------------------------------------------
    {
        if (FAILED(m_pInputInterface->CreateDevice(GUID_SysKeyboard, &m_pKeyboardDevice, nullptr))) {
            DebugLog("システムキーボードの取得に失敗.");
            return false;
        }

        if (FAILED(m_pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard))) {
            DebugLog("データフォーマットの設定に失敗.");
            return false;
        }

        if (FAILED(m_pKeyboardDevice->SetCooperativeLevel(hwnd, priorityLevel))) {
            DebugLog("協調レベルの設定に失敗.");
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
    // アクセス権を解放
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
