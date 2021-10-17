#include "Sky.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Sky::Sky()
	: m_rotateSpeed(0.2f)
{
}

//-----------------------------------------------------------------------------
// 生成直後
//-----------------------------------------------------------------------------
void Sky::Awake()
{
	m_name = "Sky";

	LoadModel("Resource/Model/Sky/Sky.gltf");
	m_transform.SetScale(float3(3000.0f));
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void Sky::Update(float deltaTime)
{
	mfloat4x4 matrix = m_transform.GetWorldMatrix();
	matrix *= mfloat4x4::CreateRotationY(m_rotateSpeed * ToRadians * deltaTime);
	m_transform.SetWorldMatrix(matrix);
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void Sky::Draw(float deltaTime)
{
	RENDERER.Getcb10().Work().m_enable = false;
	RENDERER.Getcb10().Write();
	Actor::Draw(deltaTime);
	RENDERER.Getcb10().Work().m_enable = true;
	RENDERER.Getcb10().Write();
}
