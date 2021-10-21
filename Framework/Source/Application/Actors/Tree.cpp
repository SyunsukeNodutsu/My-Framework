#include "Tree.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Tree::Tree()
{
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
