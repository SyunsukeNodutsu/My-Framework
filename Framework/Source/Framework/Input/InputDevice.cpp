#include "InputDevice.h"

//-----------------------------------------------------------------------------
//コンストラクタ
//-----------------------------------------------------------------------------
InputDevice::InputDevice()
	: m_inputMode(InputMode::eBasicInput)
	, m_basicInputDevice()
	, m_rawInputDevice()
	, m_xInputDevice()
{
}

//-----------------------------------------------------------------------------
//デストラクタ
//-----------------------------------------------------------------------------
InputDevice::~InputDevice()
{
}

//-----------------------------------------------------------------------------
//初期化
//-----------------------------------------------------------------------------
void InputDevice::Initialize(HWND hwnd, InputMode mode)
{
	m_hwnd = hwnd;
	m_inputMode = mode;

	m_rawInputDevice.Initialize();
}

//-----------------------------------------------------------------------------
//終了
//-----------------------------------------------------------------------------
void InputDevice::Finalize()
{
	m_rawInputDevice.Finalize();
}

//-----------------------------------------------------------------------------
//入力判定の準備
//-----------------------------------------------------------------------------
void InputDevice::Refresh()
{
	m_rawInputDevice.Refresh();
}

//-----------------------------------------------------------------------------
//メッセージ解析
//-----------------------------------------------------------------------------
void InputDevice::ParseMessage(void* lparam)
{
	m_rawInputDevice.ParseMessage(lparam);
}

//-----------------------------------------------------------------------------
//マウスホイール変化量を設定する
//-----------------------------------------------------------------------------
void InputDevice::SetMouseWheelDelta(int delta)
{
	m_rawInputDevice.g_spMouse->SetMouseWheelDelta(delta);
}

//-----------------------------------------------------------------------------
//キーボード入力取得
//-----------------------------------------------------------------------------
bool InputDevice::IsKeyDown(int keyCode) const
{
	switch (m_inputMode)
	{
	case InputMode::eBasicInput: break;
	case InputMode::eRawInput: return m_rawInputDevice.g_spKeyboard->IsDown(keyCode); break;
	case InputMode::eXInput: break;
	default: break;
	}
	return false;
}
bool InputDevice::IsKeyPressed(int keyCode) const
{
	switch (m_inputMode)
	{
	case InputMode::eBasicInput: break;
	case InputMode::eRawInput: return m_rawInputDevice.g_spKeyboard->IsPressed(keyCode); break;
	case InputMode::eXInput: break;
	default: break;
	}
	return false;
}
bool InputDevice::IsKeyReleased(int keyCode) const
{
	switch (m_inputMode)
	{
	case InputMode::eBasicInput: break;
	case InputMode::eRawInput: return m_rawInputDevice.g_spKeyboard->IsReleased(keyCode); break;
	case InputMode::eXInput: break;
	default: break;
	}
	return false;
}
double InputDevice::GetKeyPushTime(int keyCode) const
{
	switch (m_inputMode)
	{
	case InputMode::eBasicInput: break;
	case InputMode::eRawInput: return m_rawInputDevice.g_spKeyboard->GetPushTime(keyCode); break;
	case InputMode::eXInput: break;
	default: break;
	}
	return 0.0;
}

//-----------------------------------------------------------------------------
//マウス入力取得
//-----------------------------------------------------------------------------
bool InputDevice::IsMouseDown(int mouseKode) const
{
	switch (m_inputMode)
	{
	case InputMode::eBasicInput: break;
	case InputMode::eRawInput: return m_rawInputDevice.g_spMouse->IsDown((MouseButton)mouseKode); break;
	case InputMode::eXInput: break;
	default: break;
	}
	return false;
}
bool InputDevice::IsMousePressed(int mouseKode) const
{
	switch (m_inputMode)
	{
	case InputMode::eBasicInput: break;
	case InputMode::eRawInput: return m_rawInputDevice.g_spMouse->IsPressed((MouseButton)mouseKode); break;
	case InputMode::eXInput: break;
	default: break;
	}
	return false;
}
bool InputDevice::IsMouseReleased(int mouseKode) const
{
	switch (m_inputMode)
	{
	case InputMode::eBasicInput: break;
	case InputMode::eRawInput: return m_rawInputDevice.g_spMouse->IsReleased((MouseButton)mouseKode); break;
	case InputMode::eXInput: break;
	default: break;
	}
	return false;
}
float2 InputDevice::GetMousePos() const
{
	switch (m_inputMode)
	{
	case InputMode::eBasicInput: break;
	case InputMode::eRawInput: return m_rawInputDevice.g_spMouse->GetMousePos();
	case InputMode::eXInput: break;
	default: break;
	}
	return float2();
}
int InputDevice::GetMouseWheelDelta() const
{
	switch (m_inputMode)
	{
	case InputMode::eBasicInput: break;
	case InputMode::eRawInput: return m_rawInputDevice.g_spMouse->GetMouseWheelDelta();
	case InputMode::eXInput: break;
	default: break;
	}
	return 0;
}

void InputDevice::SetMousePos(float2 position, bool abs)
{
	switch (m_inputMode)
	{
	case InputMode::eBasicInput: break;
	case InputMode::eRawInput: m_rawInputDevice.g_spMouse->SetAt(position, m_hwnd, abs); break;
	case InputMode::eXInput: break;
	default: break;
	}
}
