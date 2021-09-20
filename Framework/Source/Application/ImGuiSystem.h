﻿//-----------------------------------------------------------------------------
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
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);

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

	// @brief 有効化を設定
	// @param onoff 設定する値
	void SetEnable(bool onoff) { m_enable = onoff; }

	// @brief LogMonitorにログを追加
	template<class... Args>
	inline void AddLog(const char* fmt, Args ...args)
	{
		std::string string = fmt;
		string += "\n";
		m_logBuffer.append(string.c_str(), args...);
		m_addLog = true;
	}

private:

	ImGuiTextBuffer m_logBuffer;
	bool m_enable;			// 有効？
	bool m_addLog;			// ログを追加
	bool m_createInifile;	// 終了時に.iniファイルを吐き出す
	bool m_showEachMonitor;	// 詳細なモニターを表示
	bool m_showDemoMonitor;	// DemoWindowを表示

private:

	// 各モニター
	
	// imGuiのレイヤー編集
	void LayersMonitor(ImGuiWindowFlags wflags);
	// Shaderデバッグ
	void ShaderDebugMonitor(ImGuiWindowFlags wflags);
	// デバッグログ
	void LogMonitor(ImGuiWindowFlags wflags);
	// ゲームオーディオ
	void AudioMonitor(ImGuiWindowFlags wflags);
	// プロファイラー分析
	void ProfilerMonitor(ImGuiWindowFlags wflags);

	//
	void PlotLinesEx(std::string string, float val);
};

//--------------------------------------------------
// #defines: インスタンスの取得
//--------------------------------------------------
#define IMGUISYSTEM ImGuiSystem::GetInstance()
