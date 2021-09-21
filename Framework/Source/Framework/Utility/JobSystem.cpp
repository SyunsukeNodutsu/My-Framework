#include "JobSystem.h"
#include <future>

namespace JobSystem
{
	// 仕事内容
	struct Job
	{
		context* ctx;
		std::function<void(JobArgs)> task;
		//----------------------
		// 共有しないのでいずれ消去
		uint32_t groupID;
		uint32_t groupJobOffset;
		uint32_t groupJobEnd;
		//----------------------
	};

	uint32_t						numThreads = 0;	// スレッド数
	ThreadSafeRingBuffer<Job, 128>	jobQueue;		// Job用リングバッファ

	// 特定のイベントもしくは条件を満たすまでスレッドの実行を待機
	std::condition_variable			wakeCondition;

	// スレッド間で使用する共有リソースを排他制御
	// lock...リソースのロックを取得 unlock...リソースのロックを手放す
	std::mutex						threadMtx;

	std::vector<std::thread> workers;

	//--------------------------------------------------
	// func
	//--------------------------------------------------

	// @brief ジョブキューから次のアイテムを実行
	// @return 成功...true 利用可能なジョブがなかった場合...false
	inline bool work()
	{
		Job job;
		if (!jobQueue.pop_front(job))
			return false;

		// JobArgs設定
		JobArgs args = {};
		args.groupID = job.groupID;

		for (uint32_t i = job.groupJobOffset; i < job.groupJobEnd; ++i)
		{
			args.jobIndex			= i;
			args.groupIndex			= i - job.groupJobOffset;
			args.isFirstJobInGroup	= (i == job.groupJobOffset);
			args.isLastJobInGroup	= (i == job.groupJobEnd - 1);
		}
		job.task(args);
		job.ctx->counter.fetch_sub(1);
		return true;
	}

	void Initialize()
	{
		// このシステムのハードウェアスレッドの数を取得
		uint32_t numCores = std::thread::hardware_concurrency();

		// 実際に必要なスレッドの数を計算
		constexpr uint32_t mainThread = 1;
		numThreads = std::max(static_cast<uint32_t>(1), numCores - mainThread);

		for (uint32_t threadID = 0; threadID < numThreads; ++threadID)
		{
			workers.push_back(std::thread([] {
				while (true)
				{
					if (!work())
					{
						// 仕事がない スレッドをスリープ状態にする
						std::unique_lock<std::mutex> lock(threadMtx);
						wakeCondition.wait(lock);
					}
				}
			}));

#ifdef _WIN32
			// Windows特有のスレッド設定
			HANDLE handle = (HANDLE)workers[threadID].native_handle();

			// 各スレッドを専用のコアに配置
			DWORD_PTR affinityMask = 1ull << threadID;
			DWORD_PTR affinity_result = SetThreadAffinityMask(handle, affinityMask);
			assert(affinity_result > 0);

			// スレッドの優先度を上げる
			BOOL priority_result = SetThreadPriority(handle, THREAD_PRIORITY_HIGHEST);
			assert(priority_result != 0);

			// スレッドに名前を設定
			std::wstring wthreadname = L"JobSystem_" + std::to_wstring(threadID);
			HRESULT hr = SetThreadDescription(handle, wthreadname.c_str());
			assert(SUCCEEDED(hr));
#endif // _WIN32

			// スレッド管理を手放す
			workers[threadID].detach();
		}

		DebugLog(("JobSystem初期化完了: [" + std::to_string(numCores) + " cores] [" + std::to_string(numThreads) + " threads]\n").c_str());
	}
	void Release()
	{
		// 全スレッド終了
		for (auto& th : workers) {
			if (th.joinable())
				th.join();
		}
	}
	void Execute(context& ctx, const std::function<void(JobArgs)>& task)
	{
		// コンテキストの状態が更新されます
		ctx.counter.fetch_add(1);

		Job job = {};
		job.ctx					= &ctx;
		job.task				= task;
		job.groupID				= 0;
		job.groupJobOffset		= 0;
		job.groupJobEnd			= 1;

		// 新しいジョブが正常にプッシュされるまで プッシュを試みます
		while (!jobQueue.push_back(job)) {
			wakeCondition.notify_all();
			work();
		}

		// 待機している全てのスレッドを起こす
		wakeCondition.notify_one();
	}
	void Wait(const context& ctx)
	{
		// 待機している全てのスレッドを起こす
		wakeCondition.notify_all();

		// 待機することで 可能であれば他の仕事をすることで
		// 現在のスレッドを有効に活用することもできます
		while (IsBusy(ctx))
			work();
	}

	uint32_t GetThreadCount() {
		return numThreads;
	}
	bool IsBusy(const context& ctx)
	{
		// コンテキストラベルが0より大きい場合は
		// まだやるべきことがあることを意味します
		return ctx.counter.load() > 0;
	}
}
