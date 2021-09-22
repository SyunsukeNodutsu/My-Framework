#include "Tank.h"

//-----------------------------------------------------------------------------
// インスタンス生成直後
//-----------------------------------------------------------------------------
void Tank::Awake()
{
	m_name = "T43";

	LoadModel("Resource/Model/T43/T43_Full.gltf");
	m_transform.SetPosition(float3(8, 0, 0));

	m_tire.LoadModel("Resource/Model/T43/T43_Tire.gltf");
	m_tire.GetTransform().SetPosition(m_transform.GetPosition() + float3(1, 0, 0));
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void Tank::Draw(float deltaTime)
{
	RENDERER.SetDitherEnable(true);
	Actor::Draw(deltaTime);
	RENDERER.SetDitherEnable(false);

	m_tire.Draw(deltaTime);
}
