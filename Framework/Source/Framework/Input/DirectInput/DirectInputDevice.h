//-----------------------------------------------------------------------------
// File: DirectInputDevice.h
//
// DirectInputAPIによる入力管理
//-----------------------------------------------------------------------------
#pragma once

// DirectInputAPIによる入力管理クラス
class DirectInputDevice
{
public:

	// @brief コンストラクタ
	DirectInputDevice();

	// @brief 初期化
	// @param hwnd ウィンドウのハンドル
	// @return 成功...true
	bool Initialize(HWND hwnd);

	// @brief 終了
	void Finalize();

	// @brief 更新
	// @param hwnd ウィンドウのハンドル
	void Update(HWND hwnd);

private:

	// インターフェース
	LPDIRECTINPUT8 m_pInputInterface;

	// 各入力デバイス
	LPDIRECTINPUTDEVICE8 m_pMouseDevice;
	LPDIRECTINPUTDEVICE8 m_pKeyboardDevice;

};
