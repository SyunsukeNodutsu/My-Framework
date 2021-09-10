﻿//-----------------------------------------------------------------------------
// File: Window.h
//
// ウィンドウ管理クラス
//-----------------------------------------------------------------------------
#pragma once

// ウィンドウクラス
class Window
{
public:

	// @brief コンストラクタ
	Window();

	// @brief デストラクタ
	~Window() { Release(); }

	// @brief ウィンドウ作成
	bool Create(int clientWidth, int clientHeight, const std::string& titleName, const std::string& windowClassName);

	// @brief 解放
	void Release();

	// @brief ウィンドウメッセージ処理
	// @return 終了メッセージが来た...false
	bool ProcessMessage();

	//--------------------------------------------------
	// 取得
	//--------------------------------------------------

	// @brief ウィンドウハンドルを返す
	// @return ウィンドウハンドル
	HWND GetWndHandle() const { return m_hWnd; }

	// @brief ウィンドウ情報を返す
	// @return ウィンドウ情報
	WINDOWINFO GetWinInfo() {
		GetWindowInfo(m_hWnd, &m_windowInfo);
		return m_windowInfo;
	}

	// @brief ウィンドウが存在すかを返す
	// @return 存在...true 存在しない...false
	bool IsCreated() const { return m_hWnd ? true : false; }

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------

	// @brief クライアントサイズの設定
	void SetClientSize(int width, int height);

	// @brief ウィンドウをデスクトップの中心に移動
	// @param hWnd 移動させるウィンドウのウィンドウハンドル
	// @return 成功...true 失敗...false
	bool SetDesktopCenterWindow(HWND hWnd) const;

private:

	HWND			m_hWnd;			// ウィンドウハンドル
	WINDOWINFO		m_windowInfo;	// ウィンドウ情報
	std::wstring	m_className;	// ウィンドウクラス名
	std::wstring	m_titleName;	// ウィンドウのタイトルバー名

private:

	static LRESULT CALLBACK callWindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

};
