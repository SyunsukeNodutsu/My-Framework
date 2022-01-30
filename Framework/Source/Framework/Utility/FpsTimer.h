//-----------------------------------------------------------------------------
// File: FpsTimer.h
//
// パフォーマンスカウンタを使用してゲームのFpsを管理
// フレームレートに依存せずに一定の速度を保って動かす
//-----------------------------------------------------------------------------
#pragma once

// FPS計測クラス
class FpsTimer
{
public:

	// @brief コンストラクタ
	FpsTimer();

	// @brief デストラクタ
	~FpsTimer() = default;

	// @brief 前フレームからのデルタタイムを計算
	void Tick();

	// @brief 経過時間リセット
	void ResetDeltaTime();

	//--------------------------------------------------
	// 取得
	//--------------------------------------------------

	// @brief 前フレームからのデルタタイムを秒単位で返す
	// @param modeRaw スケーリングを無視するかどうか
	// @return 乗算するとフレームに関係なく正確に動かせる値
	double GetDeltaTime(bool modeRaw = false) const {
		return TicksToSeconds(m_deltaTicks) * (modeRaw ? 1 : m_scaling);
	}

	// @brief このタイマの総経過時間を秒単位で返す
	// @return 経過秒数
	double GetTotalTime() const { return TicksToSeconds(m_totalTicks); }

	// @brief このタイマの総フレーム数を返す
	// @return 総フレーム数
	std::uint32_t GetTotalFrameCount() const { return m_totalFrameCount; }

	// @brief 最新のFPS値を返す
	// @return FPS値
	std::uint32_t GetFPS() const { return m_fps; }

	// @brief ゲーム内時間のスケーリング値を返す
	// @return スケーリング値
	float GetTimeScale() const { return m_scaling; }

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------

	// @brief 時間のスケーリングを設定
	// @param scale 設定するスケーリング値
	void SetTimeScale(float scale) { m_scaling = scale; }

	//--------------------------------------------------
	// static
	//--------------------------------------------------

	// 1秒を意味する整数値
	static constexpr std::uint64_t TicksPerSecond = 10'000'000;

	// @brief ティックを秒に変換
	// @param ticks 変換するティック
	// @return 秒数
	static double TicksToSeconds(const std::uint64_t ticks) {
		return static_cast<double>(ticks) / TicksPerSecond;
	}

	// @brief 秒をティック数に変換
	// @param ticks 変換する時間
	// @return ティック数
	static std::uint64_t SecondsToTicks(const double seconds) {
		return static_cast<std::uint64_t>(seconds * TicksPerSecond);
	}

public:

	static std::uint64_t DeltaTime;

private:

	LARGE_INTEGER	m_frequency;		// 高解像度タイマの周波数
	LARGE_INTEGER	m_lastTime;			// 前回計測時間

	std::uint64_t	m_maxDelta;			// 許容する最大のデルタタイム
	std::uint64_t	m_deltaTicks;		// 前フレームからのデルタティック
	std::uint64_t	m_totalTicks;		// このタイマの総経過ティック

	std::uint32_t	m_totalFrameCount;	// 総フレーム数
	std::uint32_t	m_frameThisCount;	// 現在の1秒のフレーム数を数える

	std::uint32_t	m_fps;				// 前の1秒のフレームレート
	std::uint64_t	m_secondCounter;	// 1秒を計測

	float			m_scaling;			// 時間の経過をスケーリング

};
