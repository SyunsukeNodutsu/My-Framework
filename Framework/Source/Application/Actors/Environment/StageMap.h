﻿//-----------------------------------------------------------------------------
// File: StageMap.h
//
// ステージ 地形
//-----------------------------------------------------------------------------
#pragma once

// ステージ 地形クラス
class StageMap : public Actor
{
public:

	// @brief 初期化
	void Initialize() override
	{
		g_tag |= ACTOR_TAG::eGround;
	}

};
