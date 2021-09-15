#pragma once
#include "GameObject.h"

// ステージ 地形
class StageMap : public GameObject
{
public:

	//
	void Awake() override
	{
		m_name = "StageMap";

		LoadModel("Resource/Model/StageMap/StageMap.gltf");
		m_transform.SetScale(float3(6));
	}

private:


};
