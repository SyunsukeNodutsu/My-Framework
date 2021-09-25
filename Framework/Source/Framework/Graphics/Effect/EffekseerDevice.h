//-----------------------------------------------------------------------------
// File: EffekseerDevice.h
//
// エフェクシアの管理
//-----------------------------------------------------------------------------
#pragma once
#include "../GraphicsDeviceChild.h"

// エフェクシア管理クラス
class EffekseerDevice : public GraphicsDeviceChild
{
public:

	// @brief コンストラクタ
	EffekseerDevice();

	// @brief 初期化
	// @return 成功...true
	bool Initialize();

	// @brief 終了
	void Finalize();

	//--------------------------------------------------
	// static
	//--------------------------------------------------

	// 最大描画(スプライト)数
	static constexpr int MAX_EFFECT = 8000;

public:

	EffekseerRendererDX11::Renderer* g_renderer;
	Effekseer::Manager* g_manager;

};
