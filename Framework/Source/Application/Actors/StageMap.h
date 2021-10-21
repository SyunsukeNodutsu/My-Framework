//-----------------------------------------------------------------------------
// File: StageMap.h
//
// ステージ 地形
//-----------------------------------------------------------------------------
#pragma once
#include "Actor.h"

// ステージ 地形クラス
class StageMap : public Actor
{
public:

	// @brief 初期化
	void Initialize() override
	{
		g_tag = ACTOR_TAG::eUntagged;

		g_name = "StageMap";

		LoadModel("Resource/Model/StageMap/StageMap.gltf");
		m_transform.SetScale(float3(6));
	}

};
