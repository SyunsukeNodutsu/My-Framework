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
void InputDevice::Initialize(HWND hend, InputMode mode)
{
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

}

//-----------------------------------------------------------------------------
//キーボード入力取得
//-----------------------------------------------------------------------------
bool InputDevice::IsKeyDown(int keyCode) const
{
	return false;
}
bool InputDevice::IsKeyPressed(int keyCode) const
{
	return false;
}
bool InputDevice::IsKeyReleased(int keyCode) const
{
	return false;
}
double InputDevice::GetKeyDownTime(int keyCode) const
{
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
	return float2();
}
int InputDevice::GetMouseWheelDelta() const
{
	return 0;
}

void InputDevice::SetMousePos(float2 position, bool abs)
{
	m_rawInputDevice.g_spMouse->SetAt(position, m_hwnd);
}
