//-----------------------------------------------------------------------------
// File: Keyboard.h
//
// RawInputAPIによるキーボード管理
//-----------------------------------------------------------------------------
#pragma once
#include "KeyCode.h"

// キーボード管理クラス
class Keyboard
{
public:

	// @brief 初期化 デバイス登録
	// @param hwnd アプリケーションのウィンドウハンドル
	void Initialize(HWND hwnd);

	// @brief 終了 デバイス登録解除
	// @param hwnd アプリケーションのウィンドウハンドル
	void Finalize(HWND hwnd);

	// @brief 次の入力判定の準備
	void Refresh();

	// @brief キーボード入力情報を解析する
	// @param keyboard 解析するキーボードデータ
	void ParseKeyboardData(RAWKEYBOARD keyboard);

	//--------------------------------------------------
	// キー判定
	//--------------------------------------------------

	// @brief キーが押されている間を返す
	// @param keyCode 判定したいキーコード
	// @return 押されている...true 押されていない...false
	bool IsDown(int keyCode) const { return m_isDown[keyCode]; }

	// @brief キーが押された瞬間を返す
	// @param keyCode 判定したいキーコード
	// @return 押した瞬間...true それ以外...false
	bool IsPressed(int keyCode) const { return m_isPressed[keyCode]; }

	// @brief キーが離された瞬間を返す
	// @param keyCode 判定したいキーコード
	// @return 離された瞬間...true それ以外...false
	bool IsReleased(int keyCode) const { return m_isReleased[keyCode]; }

	// @brief キーが押されている間の秒数を返す
	// @param keyCode 判定したいキーコード
	// @return キーが押されている間の秒数
	double GetPushTime(int keyCode) const { return m_pushTime[keyCode]; }

private:

	// キーボードデバイス
	RAWINPUTDEVICE m_deviceKeyboard;

	// 入力判定用 配列 ※TODO: 調査 無駄な気がする、bit演算でどうにか...
	// std::array<UINT, 256> m_state; m_state[0] |= 0x0; <-こんな感じにしたい
	// コードは 1 ～ 254 の範囲の値なので256確保
	std::array<bool, 256> m_isDown;		// 押している
	std::array<bool, 256> m_isPressed;	// 押した瞬間
	std::array<bool, 256> m_isReleased;	// 離した瞬間

	// 1度押したか 押した/離した瞬間を判定する
	std::array<bool, 256> m_isPush;

	// 押されている秒数
	std::array<double, 256> m_pushTime;

};
