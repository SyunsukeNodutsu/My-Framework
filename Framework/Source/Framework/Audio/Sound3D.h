//-----------------------------------------------------------------------------
// File: Sound3D.h
//
// 3Dサウンド操作クラス
//-----------------------------------------------------------------------------
#pragma once
#include "Sound.h"

// 3Dサウンド操作クラス
class SoundWork3D : public SoundWork
{
public:

	// @brief コンストラクタ
	SoundWork3D();

private:

	X3DAUDIO_EMITTER emitter;
	float3 vEmitterPos;
	X3DAUDIO_CONE emitterCone;

};
