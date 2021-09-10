//-----------------------------------------------------------------------------
// File: Easing.h
//
// t         ... 経過時間
// totalTime ... 目標時間
// min       ... 初期座標
// max       ... 終点座標
// https://github.com/pixelflag/EasingTest
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Linear
//-----------------------------------------------------------------------------

float Linear(float t, float totaltime, float min, float max)
{
	return (max - min) * t / totaltime + min;
}

//-----------------------------------------------------------------------------
// Quad
//-----------------------------------------------------------------------------

float QuadIn(float t, float totaltime, float min, float max)
{
	max -= min;
	t /= totaltime;
	return max * t * t + min;
}

float QuadOut(float t, float totaltime, float min, float max)
{
	max -= min;
	t /= totaltime;
	return -max * t * (t - 2) + min;
}

float QuadInOut(float t, float totaltime, float min, float max)
{
	max -= min;
	t /= totaltime * 0.5f;
	if (t < 1) return max * 0.5f * t * t + min;

	t = t - 1;
	return -max * 0.5f * (t * (t - 2) - 1) + min;
}

//-----------------------------------------------------------------------------
// Cubic
//-----------------------------------------------------------------------------

float CubicIn(float t, float totaltime, float min, float max)
{
	max -= min;
	t /= totaltime;
	return max * t * t * t + min;
}

float CubicOut(float t, float totaltime, float min, float max)
{
	max -= min;
	t = t / totaltime - 1;
	return max * (t * t * t + 1) + min;
}

float CubicInOut(float t, float totaltime, float min, float max)
{
	max -= min;
	t /= totaltime * 0.5f;
	if (t < 1) return max * 0.5f * t * t * t + min;

	t = t - 2;
	return max * 0.5f * (t * t * t + 2) + min;
}

//-----------------------------------------------------------------------------
// QuartIn
//-----------------------------------------------------------------------------

float QuartIn(float t, float totaltime, float min, float max)
{
	max -= min;
	t /= totaltime;
	return max * t * t * t * t + min;
}

float QuartOut(float t, float totaltime, float min, float max)
{
	max -= min;
	t = t / totaltime - 1;
	return -max * (t * t * t * t - 1) + min;
}

float QuartInOut(float t, float totaltime, float min, float max)
{
	max -= min;
	t /= totaltime / 2;
	if (t < 1) return max / 2 * t * t * t * t + min;

	t = t - 2;
	return -max / 2 * (t * t * t * t - 2) + min;
}

//-----------------------------------------------------------------------------
// Quint
//-----------------------------------------------------------------------------

float QuintIn(float t, float totaltime, float min, float max)
{
	max -= min;
	t /= totaltime;
	return max * t * t * t * t * t + min;
}

float QuintOut(float t, float totaltime, float min, float max)
{
	max -= min;
	t = t / totaltime - 1;
	return max * (t * t * t * t * t + 1) + min;
}

float QuintInOut(float t, float totaltime, float min, float max)
{
	max -= min;
	t /= totaltime / 2;
	if (t < 1) return max / 2 * t * t * t * t * t + min;

	t = t - 2;
	return max / 2 * (t * t * t * t * t + 2) + min;
}

//-----------------------------------------------------------------------------
// Sine
//-----------------------------------------------------------------------------

float SineIn(float t, float totaltime, float min, float max)
{
	max -= min;
	return -max * cosf(t * (static_cast<float>(PI) * 90 / 180) / totaltime) + max + min;
}

float SineOut(float t, float totaltime, float min, float max)
{
	max -= min;
	return max * sinf(t * (static_cast<float>(PI) * 90 / 180) / totaltime) + min;
}

float SineInOut(float t, float totaltime, float min, float max)
{
	max -= min;
	return -max / 2 * (cosf(t * static_cast<float>(PI) / totaltime) - 1) + min;
}

//-----------------------------------------------------------------------------
// Exp
//-----------------------------------------------------------------------------

float ExpIn(float t, float totaltime, float min, float max)
{
	max -= min;
	return t == 0.0 ? min : max * powf(2, 10 * (t / totaltime - 1)) + min;
}

float ExpOut(float t, float totaltime, float min, float max)
{
	max -= min;
	return t == totaltime ? max + min : max * (-powf(2, -10 * t / totaltime) + 1) + min;
}

float ExpInOut(float t, float totaltime, float min, float max)
{
	if (t == 0.0f) return min;
	if (t == totaltime) return max;
	max -= min;
	t /= totaltime / 2;

	if (t < 1) return max / 2 * powf(2, 10 * (t - 1)) + min;

	t = t - 1;
	return max / 2 * (-powf(2, -10 * t) + 2) + min;

}

//-----------------------------------------------------------------------------
// Circ
//-----------------------------------------------------------------------------

float CircIn(float t, float totaltime, float min, float max)
{
	max -= min;
	t /= totaltime;
	return -max * (sqrtf(1 - t * t) - 1) + min;
}

float CircOut(float t, float totaltime, float min, float max)
{
	max -= min;
	t = t / totaltime - 1;
	return max * sqrtf(1 - t * t) + min;
}

float CircInOut(float t, float totaltime, float min, float max)
{
	max -= min;
	t /= totaltime / 2;
	if (t < 1) return -max / 2 * (sqrtf(1 - t * t) - 1) + min;

	t = t - 2;
	return max / 2 * (sqrtf(1 - t * t) + 1) + min;
}

//-----------------------------------------------------------------------------
// Elastic
//-----------------------------------------------------------------------------

float ElasticIn(float t, float totaltime, float min, float max)
{
	max -= min;
	t /= totaltime;

	float s = 1.70158f;
	float p = totaltime * 0.3f;
	float a = max;

	if (t == 0) return min;
	if (t == 1) return min + max;

	if (a < abs(max))
	{
		a = max;
		s = p / 4;
	}
	else
	{
		s = p / (2 * static_cast<float>(PI)) * asinf(max / a);
	}

	t = t - 1;
	return -(a * powf(2, 10 * t) * sinf((t * totaltime - s) * (2 * static_cast<float>(PI)) / p)) + min;
}

float ElasticOut(float t, float totaltime, float min, float max)
{
	max -= min;
	t /= totaltime;

	float s = 1.70158f;
	float p = totaltime * 0.3f; ;
	float a = max;

	if (t == 0) return min;
	if (t == 1) return min + max;

	if (a < abs(max))
	{
		a = max;
		s = p / 4;
	}
	else
	{
		s = p / (2 * static_cast<float>(PI)) * asinf(max / a);
	}

	return a * powf(2, -10 * t) * sinf((t * totaltime - s) * (2 * static_cast<float>(PI)) / p) + max + min;
}

float ElasticInOut(float t, float totaltime, float min, float max)
{
	max -= min;
	t /= totaltime / 2;

	float s = 1.70158f;
	float p = totaltime * (0.3f * 1.5f);
	float a = max;

	if (t == 0) return min;
	if (t == 2) return min + max;

	if (a < abs(max))
	{
		a = max;
		s = p / 4;
	}
	else
	{
		s = p / (2 * static_cast<float>(PI)) * asinf(max / a);
	}

	if (t < 1)
	{
		return -0.5f * (a * powf(2, 10 * (t -= 1)) * sinf((t * totaltime - s) * (2 * static_cast<float>(PI)) / p)) + min;
	}

	t = t - 1;
	return a * powf(2, -10 * t) * sinf((t * totaltime - s) * (2 * static_cast<float>(PI)) / p) * 0.5f + max + min;
}

//-----------------------------------------------------------------------------
// Back
//-----------------------------------------------------------------------------

float BackIn(float t, float totaltime, float min, float max, float s = 1.7f)
{
	max -= min;
	t /= totaltime;
	return max * t * t * ((s + 1) * t - s) + min;
}

float BackOut(float t, float totaltime, float min, float max, float s = 1.7f)
{
	max -= min;
	t = t / totaltime - 1;
	return max * (t * t * ((s + 1) * t + s) + 1) + min;
}

float BackInOut(float t, float totaltime, float min, float max, float s = 1.7f)
{
	max -= min;
	s *= 1.525f;
	t /= totaltime / 2;
	if (t < 1) return max / 2 * (t * t * ((s + 1) * t - s)) + min;

	t = t - 2;
	return max / 2 * (t * t * ((s + 1) * t + s) + 2) + min;
}

//-----------------------------------------------------------------------------
// BounceInOut
//-----------------------------------------------------------------------------

float BounceOut(float t, float totaltime, float min, float max)
{
	max -= min;
	t /= totaltime;

	if (t < 1.0f / 2.75f)
	{
		return max * (7.5625f * t * t) + min;
	}
	else if (t < 2.0f / 2.75f)
	{
		t -= 1.5f / 2.75f;
		return max * (7.5625f * t * t + 0.75f) + min;
	}
	else if (t < 2.5f / 2.75f)
	{
		t -= 2.25f / 2.75f;
		return max * (7.5625f * t * t + 0.9375f) + min;
	}
	else
	{
		t -= 2.625f / 2.75f;
		return max * (7.5625f * t * t + 0.984375f) + min;
	}
}

float BounceIn(float t, float totaltime, float min, float max)
{
	max -= min;
	return max - BounceOut(totaltime - t, totaltime, 0, max) + min;
}

float BounceInOut(float t, float totaltime, float min, float max)
{
	if (t < totaltime / 2)
	{
		return BounceIn(t * 2, totaltime, 0, max - min) * 0.5f + min;
	}
	else
	{
		return BounceOut(t * 2 - totaltime, totaltime, 0, max - min) * 0.5f + min + (max - min) * 0.5f;
	}
}
