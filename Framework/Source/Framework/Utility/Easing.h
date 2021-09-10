//-----------------------------------------------------------------------------
// File: Easing.h
//
// イージング管理 緩急をつけるあれ
//-----------------------------------------------------------------------------
#pragma once
#include "Timer.h"

enum class EASING_TYPE;

// イージングクラス ※1つの値のみに対応.目標をX座標として考える
class Easing
{
public:

	// @brief コンストラクタ
	Easing();

	// @brief 更新 ※注意.必ずEmitの後に記述
	void Update();

	// @brief 発生
	// @param type Easingの種類
	// @param timeLength
	// @param startX
	// @param endX
	void Emit(EASING_TYPE type, float timeLength, float startX, float endX);

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------

	// @brief 進行具合を返す
	// @return 進行具合(0～1)
	float GetProgress() { return m_progress; }

	// @brief 完了しているか返す
	// @return 完了...true
	bool IsEmit() { return m_isEmit; }

private:

	EASING_TYPE		m_easingType;	// イージングの種類
	CommonTimer		m_timer;		// 時間計測用タイマー
	bool			m_isEmit;		// 発生しているか
	float			m_startX;		// 移動開始座標
	float			m_endX;			// 移動終了座標
	float			m_easingTime;	// アニメーションさせる時間
	float			m_progress;		// 移動量
	float			m_nowTime;		// 現在の時間

};

#pragma region イージングの種類
enum class EASING_TYPE
{
	eLinear,
	eConstant,
	eQuadIn,
	eQuadOut,
	eQuadInOut,
	eCubicIn,
	eCubicOut,
	eCubicInOut,
	eQuartIn,
	eQuartOut,
	eQuartInOut,
	eQuintIn,
	eQuintOut,
	eQuintInOut,
	eSineIn,
	eSineOut,
	eSineInOut,
	eExpIn,
	eExpOut,
	eExpInOut,
	eCircIn,
	eCircOut,
	eCircInOut,
	eElasticIn,
	eElasticOut,
	eElasticInOut,
	eBackIn,
	eBackOut,
	eBackInOut,
	eBounceIn,
	eBounceOut,
	eBounceInOut,
};
#pragma endregion
