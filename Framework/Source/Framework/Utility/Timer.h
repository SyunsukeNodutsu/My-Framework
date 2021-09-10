//-----------------------------------------------------------------------------
// File: Timer.h
//
// chronoによる時間計測
//-----------------------------------------------------------------------------
#pragma once

// 時間計測タイマー
struct CommonTimer
{
	// @brief コンストラクタ レコードの開始
	CommonTimer() { Record(); }

	// @brief 基準となるタイムスタンプの記録
	inline void Record() {
		m_timeStamp = std::chrono::high_resolution_clock::now();
	}

	// @brief 最後にRecord()が呼ばれてからの経過時間(秒)を返す
	// @return 経過時間(秒)
	inline double GetElapsedSeconds() {
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now - m_timeStamp);
		return time_span.count();
	}

	// @brief 最後にRecord()が呼ばれてからの経過時間(ミリ秒)を返す
	// @return 経過時間(ミリ秒)
	inline double GetElapsedMilliseconds() {
		return GetElapsedSeconds() * 1000.0;
	}

private:

	std::chrono::high_resolution_clock::time_point m_timeStamp;

};
