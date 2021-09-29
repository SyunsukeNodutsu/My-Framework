//-----------------------------------------------------------------------------
// File: GameSystem.h
//
// 描画のワークフローを管理
// パイプラインへのアクセスをアプリケーションに提供
//-----------------------------------------------------------------------------
#pragma once
#include "../Graphics/GraphicsDeviceChild.h"

// 描画パイプラインクラス
class GraphicsPipeline : public GraphicsDeviceChild
{
public:

	// @brief コンストラクタ
	GraphicsPipeline();

	// @brief 描画
	void Render();

private:


};
