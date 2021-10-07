#include "Tree.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Tree::Tree()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void Tree::Initialize()
{
	LoadModel("Resource/Model/Tree/02/Tree02.gltf");
	//LoadModel("Resource/Model/Tree/01/Tree01.gltf");

	m_transform.SetPosition(float3(0, -0.08f, 10));
	m_transform.SetScale(float3(0.5f));
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void Tree::Draw(float deltaTime)
{
	RENDERER.SetRasterize(RS_CullMode::eCullNone, RS_FillMode::eSolid);
	RENDERER.Getcb8().Work().m_dither_enable = true;
	Actor::Draw(deltaTime);
	RENDERER.SetRasterize(RS_CullMode::eBack, RS_FillMode::eSolid);
	RENDERER.Getcb8().Work().m_dither_enable = false;
}
