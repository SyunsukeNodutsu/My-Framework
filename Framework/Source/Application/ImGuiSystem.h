//-----------------------------------------------------------------------------
// File: ImGuiSystem.h
//
// ゲームのimGuiデバッグ管理
//-----------------------------------------------------------------------------
#pragma once

// imGuiデバッグ管理クラス
class ImGuiSystem
{
public:

	// @brief コンストラクタ
	ImGuiSystem();

	// @brief 初期化
	void Initialize();

	// @brief 終了
	void Finalize();

	// @brief 描画開始
	void Begin();

	// @brief imGui描画
	void DrawImGui();

	//--------------------------------------------------
	// 取得・設定
	//--------------------------------------------------

	// @brief 単一のインスタンスを返す
	// @return シングルトン・インスタンス
	static ImGuiSystem& GetInstance() {
		static ImGuiSystem instance; return instance;
	}

	// @brief LogMonitorにログを追加
	void AddLog(const std::string& log);

private:


};

//--------------------------------------------------
// #defines: インスタンスの取得
//--------------------------------------------------
#define IMGUISYSTEM ImGuiSystem::GetInstance()
