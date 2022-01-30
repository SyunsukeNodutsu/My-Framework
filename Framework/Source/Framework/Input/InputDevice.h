//-----------------------------------------------------------------------------
// File: InputDevice.h
//
// 入力デバイス
//-----------------------------------------------------------------------------
#pragma once
#include "BasicInput/BasicInputDevice.h"
#include "RawInput/RawInputDevice.h"
#include "XInput/XInputDevice.h"

//入力モード
enum class InputMode
{
	eBasicInput,//Win32(GetAsyncKeyState)
	eRawInput,	//RawInputAPI
	eXInput,	//XInputAPI
};

//入力デバイス
class InputDevice
{
public:

	//@brief コンストラクタ
	InputDevice();

	//@brief デストラクタ
	~InputDevice();

	//@brief 初期化
	//@param hend ウィンドウハンドル
	//@param mode 入力モード
	void Initialize(HWND hend, InputMode mode = InputMode::eBasicInput);

	//@brief 終了
	void Finalize();

	//@brief 次の入力判定の準備
	void Refresh();

	//@brief Windowsメッセージを解析する
	//@param lparam 解析するmessage情報
	void ParseMessage(void* lparam);

	//@brief マウスホイール変化量を設定する
	//@param delta ホイール変化量
	void SetMouseWheelDelta(int delta);

	//--------------------------------------------------
	//キーボード入力取得
	//--------------------------------------------------
	
	//@brief キーが押されている間を返す
	//@param keyCode 判定したいキーコード
	//@return 押されている...true 押されていない...false
	bool IsKeyDown(int keyCode) const;

	//@brief キーが押された瞬間を返す
	//@param keyCode 判定したいキーコード
	//@return 押した瞬間...true それ以外...false
	bool IsKeyPressed(int keyCode) const;

	//@brief キーが離された瞬間を返す
	//@param keyCode 判定したいキーコード
	//@return 離された瞬間...true それ以外...false
	bool IsKeyReleased(int keyCode) const;

	//@brief キーが押されている間の秒数を返す
	//@param keyCode 判定したいキーコード
	//@return キーが押されている間の秒数
	double GetKeyDownTime(int keyCode) const;

	//--------------------------------------------------
	//マウス入力取得
	//--------------------------------------------------

	//@brief キーが押されている間を返す
	//@param mouseKode 判定したいボタン
	//@return 押されている...true 押されていない...false
	bool IsMouseDown(int mouseKode) const;

	//@brief キーが押された瞬間を返す
	//@param mouseKode 判定したいボタン
	//@return 押した瞬間...true それ以外...false
	bool IsMousePressed(int mouseKode) const;

	//@brief キーが離された瞬間を返す
	//@param mouseKode 判定したいボタン
	//@return 離された瞬間...true それ以外...false
	bool IsMouseReleased(int mouseKode) const;

	//@brief マウス座標を返す
	//@return マウス座標(全体)
	float2 GetMousePos() const;

	//@brief マウスホイールの変化量を返す
	//@return マウスホイールの変化量
	int GetMouseWheelDelta() const;

	//@brief カーソルの表示切り替え
	//@param onoff 表示するか
	//@note windowを作成したスレッドでしか機能しない
	void SetCursorShow(const bool onoff) {
		if (onoff) { while (ShowCursor(TRUE) < 0) {} }
		else { while (ShowCursor(FALSE) >= 0) {} }
	}

	// マウス座標を設定
	void SetMousePos(float2 position, bool abs = false);

	//--------------------------------------------------
	//コントローラー入力取得
	//--------------------------------------------------

private:

	InputMode m_inputMode;					//入力モード
	BasicInputDevice m_basicInputDevice;	//Win32標準的な入力デバイス
	RawInputDevice m_rawInputDevice;		//RawInputAPIを使用した入力デバイス
	XInputDevice m_xInputDevice;			//XInputAPIを使用した入力デバイス

	HWND m_hwnd;							//初期化の際に取得したウィンドウハンドル

};
