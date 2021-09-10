#include "SailingShip.h"

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void SailingShip::Initialize()
{
	LoadModel("Resource/Model/Ship/Ship.gltf");
	m_transform.SetPosition(float3(0, 0, 40));
	m_transform.SetAngle(float3(0, 140, 0));
}

//-----------------------------------------------------------------------------
// 3D描画
//-----------------------------------------------------------------------------
void SailingShip::Draw(float deltaTime)
{
	// 旗を裏表描画するためとりあえず今のところは
	D3D.GetRenderer().SetRasterize(RS_CullMode::eCullNone, RS_FillMode::eSolid);
	D3D.GetRenderer().SetDitherEnable(true);
	GameObject::Draw(deltaTime);
	D3D.GetRenderer().SetDitherEnable(false);
	D3D.GetRenderer().SetRasterize(RS_CullMode::eBack, RS_FillMode::eSolid);
}
