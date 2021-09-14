//-----------------------------------------------------------------------------
// File: Sky.h
//
// 球体の空
//-----------------------------------------------------------------------------
#pragma once
#include "GameObject.h"

// スカイスフィアクラス
class Sky : public GameObject
{
public:

	// @brief 生成直後
	void Awake() override
	{
		LoadModel("Resource/Model/Sky/Sky.gltf");
		m_transform.SetScale(float3(1000.0f));
	}

	// @brief 更新
	void Update(float deltaTime) override
	{
		static mfloat4x4 matrix = m_transform.GetWorldMatrix();
		matrix *= mfloat4x4::CreateRotationY(0.2f * ToRadians * deltaTime);
		m_transform.SetWorldMatrix(matrix);
	}

	// @brief 描画
	void Draw(float deltaTime)
	{
		D3D.GetRenderer().SetLightEnable(false);
		GameObject::Draw(deltaTime);
		D3D.GetRenderer().SetLightEnable(true);
	}

private:

};
