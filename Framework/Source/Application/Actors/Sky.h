//-----------------------------------------------------------------------------
// File: Sky.h
//
// 球体の空
//-----------------------------------------------------------------------------
#pragma once
#include "Actor.h"

// スカイスフィアクラス
class Sky : public Actor
{
public:

	// @brief 生成直後
	void Awake() override
	{
		m_name = "Skt";

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
		RENDERER.SetLightEnable(false);
		Actor::Draw(deltaTime);
		RENDERER.SetLightEnable(true);
	}

private:

};
