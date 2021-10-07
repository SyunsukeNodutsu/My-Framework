#include "Easing.h"
#include "EasingType.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Easing::Easing()
	: m_easingType(EASING_TYPE::eLinear)
	, m_timer()
	, m_isEmit(false)
	, m_startX(0.0f)
	, m_endX(0.0f)
	, m_easingTime(0.0f)
	, m_progress(0.0f)
	, m_nowTime(0.0f)
{
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void Easing::Update()
{
	if (!m_isEmit) return;

	// 終了
	if (m_nowTime >= m_easingTime) {
		m_progress = m_endX;
		m_isEmit = false;
		return;
	}

	// タイプごとにイージング実行
	switch (m_easingType)
	{
	case EASING_TYPE::eLinear:		m_progress = Linear(m_nowTime, m_easingTime, m_startX, m_endX);			break;
	case EASING_TYPE::eQuadIn:		m_progress = QuadIn(m_nowTime, m_easingTime, m_startX, m_endX);			break;
	case EASING_TYPE::eQuadOut:		m_progress = QuadOut(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eQuadInOut:	m_progress = QuadInOut(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eCubicIn:		m_progress = CubicIn(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eCubicOut:	m_progress = CubicOut(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eCubicInOut:	m_progress = CubicInOut(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eQuartIn:		m_progress = QuartIn(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eQuartOut:	m_progress = QuartOut(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eQuartInOut:	m_progress = QuartInOut(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eQuintIn:		m_progress = QuintIn(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eQuintOut:	m_progress = QuintOut(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eQuintInOut:	m_progress = QuintInOut(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eSineIn:		m_progress = SineIn(m_nowTime, m_easingTime, m_startX, m_endX);			break;
	case EASING_TYPE::eSineOut:		m_progress = SineOut(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eSineInOut:	m_progress = SineInOut(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eExpIn:		m_progress = ExpIn(m_nowTime, m_easingTime, m_startX, m_endX);			break;
	case EASING_TYPE::eExpOut:		m_progress = ExpOut(m_nowTime, m_easingTime, m_startX, m_endX);			break;
	case EASING_TYPE::eExpInOut:	m_progress = ExpInOut(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eCircIn:		m_progress = CircIn(m_nowTime, m_easingTime, m_startX, m_endX);			break;
	case EASING_TYPE::eCircOut:		m_progress = CircOut(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eCircInOut:	m_progress = CircInOut(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eElasticIn:	m_progress = ElasticIn(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eElasticOut:	m_progress = ElasticOut(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eElasticInOut:m_progress = ElasticInOut(m_nowTime, m_easingTime, m_startX, m_endX);	break;
	case EASING_TYPE::eBackIn:		m_progress = BackIn(m_nowTime, m_easingTime, m_startX, m_endX);			break;
	case EASING_TYPE::eBackOut:		m_progress = BackOut(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eBackInOut:	m_progress = BackInOut(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eBounceIn:	m_progress = BounceIn(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eBounceOut:	m_progress = BounceOut(m_nowTime, m_easingTime, m_startX, m_endX);		break;
	case EASING_TYPE::eBounceInOut: m_progress = BounceInOut(m_nowTime, m_easingTime, m_startX, m_endX);	break;
	}

	// 時間経過
	m_nowTime = static_cast<float>(m_timer.GetElapsedSeconds());
}

//-----------------------------------------------------------------------------
// 発生
//-----------------------------------------------------------------------------
void Easing::Emit(EASING_TYPE a_type, float a_timeLength, float a_startX, float a_endX)
{
	if (m_isEmit) return;

	m_easingType = a_type;
	m_startX = a_startX;
	m_endX = a_endX;
	m_easingTime = a_timeLength;

	// 計測開始
	m_timer.Record();

	m_isEmit = true;
}
