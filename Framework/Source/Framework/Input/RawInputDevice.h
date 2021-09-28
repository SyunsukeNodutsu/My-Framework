//-----------------------------------------------------------------------------
// File: RawInputDevice.h
//
// RawInputAPIによる入力管理
//-----------------------------------------------------------------------------
#pragma once
#include "Keyboard.h"
#include "Mouse.h"

// RawInputによる入力管理クラス
class RawInputDevice
{
public:

	// @brief コンストラクタ
	RawInputDevice();

	// @brief デストラクタ
	~RawInputDevice() = default;

	// @brief 初期化
	void Initialize();

	// @brief 終了
	void Finalize();

	// @brief 次の入力判定の準備
	void Refresh();

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------

	// @brief Windowsメッセージを解析する
	// @param lparam 解析するmessage情報
	void ParseMessage(void* lparam);

public:

	// キーボードデバイス
	std::shared_ptr<Keyboard>	g_spKeyboard;

	// マウスデバイス
	std::shared_ptr<Mouse>		g_spMouse;

};
