//-----------------------------------------------------------------------------
// File: RawInput.h
//
// RawInputAPIによる入力管理
//
// TODO: テスト用にシングルトン実装 あとで見直す
// TODO: 即値とか範囲外アクセス確認とかいろいろ足りない^^
//-----------------------------------------------------------------------------
#pragma once
#include "Keyboard.h"
#include "Mouse.h"

// RawInputによる入力管理クラス
class RawInput
{
public:

	// @brief デストラクタ
	~RawInput() = default;

	// @brief 初期化
	void Initialize();

	// @brief 終了
	void Finalize();

	// @brief 次の入力判定の準備
	void Refresh();

	//--------------------------------------------------
	// 取得
	//--------------------------------------------------

	// @brief インスタンスを返す
	// @return シングルトン・インスタンス
	static RawInput& GetInstance() { static RawInput instance; return instance; }

	// @brief キーボード管理クラスを返す
	// @return キーボード管理クラス
	std::shared_ptr<Keyboard> GetKeyboard() { return m_spKeyboard; }

	// @brief マウス管理クラスを返す
	// @return マウス管理クラス
	std::shared_ptr<Mouse> GetMouse() { return m_spMouse; }

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------

	// @brief Windowsメッセージを解析する
	// @param lparam 解析するmessage情報
	void ParseMessage(void* lparam);

private:

	std::shared_ptr<Keyboard>	m_spKeyboard;
	std::shared_ptr<Mouse>		m_spMouse;

private:

	// @brief コンストラクタ
	RawInput();

};

//--------------------------------------------------
// #defines: インスタンスの取得
//--------------------------------------------------
#define RAW_INPUT RawInput::GetInstance()
