//-----------------------------------------------------------------------------
// File: JobSystem.h
//
// 並行タスクの実行を管理 ワーカースレッドパターン
//
// わしの理想
// 1.環境に合わせてスレッドをあらかじめ作成
// 2.仕事を追加していき jobsystemが仕事を適切にスレッドに割り当てる
//-----------------------------------------------------------------------------
#pragma once

// 1つのタスクを共有？
struct JobArgs
{
	uint32_t jobIndex;		// ディスパッチに関連するジョブのインデックス	※HLSLの SV_DispatchThreadID のようなもの
	uint32_t groupID;		// ディスパッチに対するグループインデックス		※HLSLの SV_GroupID のようなもの
	uint32_t groupIndex;	// グループに対するジョブの相対的なインデックス ※HLSLの SV_GroupIndex のようなもの
	bool isFirstJobInGroup;	// 現在の仕事は グループの中で最初のものか？
	bool isLastJobInGroup;	// 現在の仕事は グループの中で最後の仕事か？
};

// 並行タスクの実行を管理
namespace JobSystem
{
	// 同期可能な単一のワークロード
	struct context
	{
		std::atomic<uint32_t> counter{ 0 };
	};

	// @brief 使用できるスレッドを調査し 割り当てる準備を行う
	void Initialize();

	// @brief 解放
	void Release();

	// @brief タスクを非同期に実行 ※アイドル状態のスレッドがこれを実行
	// @param ctx コンテキスト
	// @param task jobArgsをパラメータとして受け取る
	void Execute(context& ctx, const std::function<void(JobArgs)>& task);

	// @brief すべてのスレッドがアイドル状態になるまで待つ
	// @param ctx コンテキスト
	void Wait(const context& ctx);

	//--------------------------------------------------
	// 取得
	//--------------------------------------------------

	// @brief スレッド数を返す
	// @return スレッド(論理プロセッサ)数
	uint32_t GetThreadCount();

	// @brief どのスレッドも現在動作しているかどうかを返す
	// @param ctx コンテキスト
	// @return 動作中...true ビジー...false
	bool IsBusy(const context& ctx);

}
