//-----------------------------------------------------------------------------
// File: Watchtower.h
//
// ものみ台 監視する高台
//-----------------------------------------------------------------------------
#pragma once
#include "GameObject.h"

// ものみ台
class Watchtower : public GameObject
{
public:

	// 生成直後
	void Awake() override
	{
		LoadModel("Resource/Model/Watchtower/Watchtower.gltf");
		m_transform.SetPosition(float3(-10, 0, 0));
		m_transform.SetScale(float3(2));
	}

private:

};
