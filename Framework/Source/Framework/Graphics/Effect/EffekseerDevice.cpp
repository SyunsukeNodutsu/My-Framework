#include "EffekseerDevice.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
EffekseerDevice::EffekseerDevice()
	: g_renderer()
	, g_manager()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool EffekseerDevice::Initialize()
{
	// エフェクトのレンダラーの作成
	/*g_renderer = ::EffekseerRendererDX11::Renderer::Create(
		g_graphicsDevice->g_cpDevice.Get(), g_graphicsDevice->g_cpContext.Get(), MAX_EFFECT
	);*/

	// エフェクトのマネージャーの作成
	//g_manager = ::Effekseer::Manager::Create(MAX_EFFECT);

	// 描画モジュールの設定
	/*g_manager->SetSpriteRenderer(g_renderer->CreateSpriteRenderer());
	g_manager->SetRibbonRenderer(g_renderer->CreateRibbonRenderer());
	g_manager->SetRingRenderer(g_renderer->CreateRingRenderer());
	g_manager->SetTrackRenderer(g_renderer->CreateTrackRenderer());
	g_manager->SetModelRenderer(g_renderer->CreateModelRenderer());*/

	return true;
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void EffekseerDevice::Finalize()
{
	g_manager.Reset();
	g_renderer.Reset();
}
