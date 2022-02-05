#include "CesiumMan.h"

//-----------------------------------------------------------------------------
//初期化
//-----------------------------------------------------------------------------
void CesiumMan::Initialize()
{
	g_name = "CesiumMan";

	LoadModel("Resource/Model/CesiumMan/CesiumMan3.gltf");

	m_transform.SetScale(10);

	//アニメーション設定
	auto data = m_modelWork.GetAnimation(1);
	m_animator.SetAnimation(data);
}

//-----------------------------------------------------------------------------
//更新
//-----------------------------------------------------------------------------
void CesiumMan::Update(float deltaTime)
{
	float animationSpeed = 60.0f;
	m_animator.AdvanceTime(m_modelWork.WorkNodes(), animationSpeed * deltaTime);
	m_modelWork.CalcNodeMatrices();
}

//-----------------------------------------------------------------------------
//描画
//-----------------------------------------------------------------------------
void CesiumMan::Draw(float deltaTime)
{
	RENDERER.Getcb8().Work().m_dither_enable = true;
	RENDERER.Getcb8().Write();
	Actor::Draw(deltaTime);
	RENDERER.Getcb8().Work().m_dither_enable = false;
	RENDERER.Getcb8().Write();
}
