//-----------------------------------------------------------------------------
// File: ImGuiSystem.h
//
// ゲームのimGuiデバッグ管理
//-----------------------------------------------------------------------------
#pragma once

//imGuiデバッグ管理クラス
class ImGuiSystem
{
public:

	//@brief コンストラクタ
	ImGuiSystem();

	//@brief 初期化
	//@param device Direct3Dデバイス
	//@param context Direct3Dデバイスコンテキスト
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);

	//@brief 終了
	void Finalize();

	//@brief 描画開始
	void Begin();

	//@brief 描画終了
	void End();

	//@brief imGui描画
	void DrawImGui();

	//取得/設定 ===========================================
	
	//@brief 有効化を設定
	//@param onoff 設定する値
	void SetEnable(bool onoff) { m_enable = onoff; }

	//@brief LogMonitorにログを追加 ※Logの追加はFPSの低下につながります
	//@param log 追加するログ
	//@param sinfo ログにに付与するソースロケーション
	void AddLog(std::string_view log);
	//void AddLog(std::string_view log, std::source_location sinfo);

	//@brief LogMonitorのクリア
	void ClearLog() { m_logBuffer.clear(); }

private:

	ImGuiTextBuffer m_logBuffer;	//ログ用のImGui専用バッファー
	bool m_enable;					//有効？
	bool m_addLog;					//ログを追加
	bool m_createInifile;			//終了時に.iniファイルを吐き出す
	bool m_showEachMonitor;			//詳細なモニターを表示
	bool m_showDemoMonitor;			//DemoWindowを表示

private:

	//各モニター ==========================================
	//ゲームのシーン
	void SceneMonitor(ImGuiWindowFlags wflags);
	//Shaderデバッグ
	void ShaderDebugMonitor(ImGuiWindowFlags wflags);
	//デバッグログ
	void LogMonitor(ImGuiWindowFlags wflags);
	//ゲームオーディオ
	void AudioMonitor(ImGuiWindowFlags wflags);
	//プロファイラー分析
	void ProfilerMonitor(ImGuiWindowFlags wflags);

	//@brief imGuiのPlotLinesをラップ ※TODO: なんかFPSめっちゃ下がる
	//@param string プロットのラベル名
	//@param val ラインに表示する値
	void PlotLinesEx(const std::string& string, float val);
};
