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
	RENDERER.Getcb8().Work().m_dither_enable = true;
	RENDERER.Getcb8().Write();
	Actor::Draw(deltaTime);
	RENDERER.Getcb8().Work().m_dither_enable = false;
	RENDERER.Getcb8().Write();

	m_tire.Draw(deltaTime);
}
