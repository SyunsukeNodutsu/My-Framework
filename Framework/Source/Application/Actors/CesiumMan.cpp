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
