#pragma once
#include "GameObject.h"

// 戦車クラス
class Tank : public GameObject
{
public:

	// @brief インスタンス生成直後
	void Awake() override
	{
		LoadModel("Resource/Model/T43/T43_Full.gltf");
		m_transform.SetPosition(float3(8, 0, 0));
	}

	// @brief 描画
	void Draw(float deltaTime) override
	{
		D3D.GetRenderer().SetDitherEnable(true);
		GameObject::Draw(deltaTime);
		D3D.GetRenderer().SetDitherEnable(false);
	}

private:


};
