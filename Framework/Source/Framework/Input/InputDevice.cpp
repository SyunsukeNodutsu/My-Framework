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

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// マウスがウィンドウのクライアント領域上に存在
//-----------------------------------------------------------------------------
bool InputDevice::MouseInTheClient(float2 mpos, WINDOWINFO* pwinfo) const
{
	if (pwinfo == nullptr)
	{
		auto winfo = ApplicationChilled::GetApplication()->g_window->GetWinInfo();
		pwinfo = &winfo;
	}

	if (mpos.x < 0 || mpos.x > pwinfo->rcClient.right - pwinfo->rcClient.left) return false;
	if (mpos.y < 0 || mpos.y > pwinfo->rcClient.bottom - pwinfo->rcClient.top) return false;

	return true;
}

//-----------------------------------------------------------------------------
// 水平入力を返す
//-----------------------------------------------------------------------------
float InputDevice::GetHorizontal() const
{
	float2 result;
	if (IsKeyDown(KeyCode::A)) result.x -= 1.0f;
	if (IsKeyDown(KeyCode::D)) result.x += 1.0f;
	result.Normalize();

	return result.x;
}

//-----------------------------------------------------------------------------
// 垂直入力を返す
//-----------------------------------------------------------------------------
float InputDevice::GetVertical() const
{
	float2 result;
	if (IsKeyDown(KeyCode::W)) result.x += 1.0f;
	if (IsKeyDown(KeyCode::S)) result.x -= 1.0f;
	result.Normalize();

	return result.x;
}
