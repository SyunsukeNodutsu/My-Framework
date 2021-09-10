#include "Tree.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Tree::Tree()
{
	LoadModel("Resource/Model/Tree/Tree.gltf");
	m_transform.SetPosition(float3(-2, 0, 0));
	m_transform.SetScale(float3(0.5f));
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void Tree::Draw(float deltaTime)
{
	// 木の葉っぱ描画用に 裏面カリングをOFFに
	D3D.GetRenderer().SetRasterize(RS_CullMode::eCullNone, RS_FillMode::eSolid);
	D3D.GetRenderer().SetBlend(BlendMode::eBlendNone);

	GameObject::Draw(deltaTime);

	D3D.GetRenderer().SetRasterize(RS_CullMode::eBack, RS_FillMode::eSolid);
	D3D.GetRenderer().SetBlend(BlendMode::eAlpha);
}
