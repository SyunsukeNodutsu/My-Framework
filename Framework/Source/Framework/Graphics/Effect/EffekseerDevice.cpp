#include "EffekseerDevice.h"
#include "../../../Application/main.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
EffekseerDevice::EffekseerDevice()
	: g_renderer(nullptr)
	, g_manager(nullptr)
	, m_instanceMap()
	, m_speed(60.0f)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool EffekseerDevice::Initialize()
{
	if (g_graphicsDevice == nullptr) return false;
	if (g_graphicsDevice->g_cpDevice == nullptr) return false;
	if (g_graphicsDevice->g_cpContext == nullptr) return false;

	//レンダラ作成
	g_renderer = EffekseerRendererDX11::Renderer::Create(g_graphicsDevice->g_cpDevice.Get(), g_graphicsDevice->g_cpContext.Get(), MAX_EFFECT);

	//エフェクトのマネージャーの作成
	g_manager = Effekseer::Manager::Create(8000);

	//描画モジュールの設定
	g_manager->SetSpriteRenderer(g_renderer->CreateSpriteRenderer());
	g_manager->SetRibbonRenderer(g_renderer->CreateRibbonRenderer());
	g_manager->SetRingRenderer(g_renderer->CreateRingRenderer());
	g_manager->SetTrackRenderer(g_renderer->CreateTrackRenderer());
	g_manager->SetModelRenderer(g_renderer->CreateModelRenderer());

	// テクスチャ、モデル、マテリアルローダーの設定する。
	// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
	g_manager->SetTextureLoader(g_renderer->CreateTextureLoader());
	g_manager->SetModelLoader(g_renderer->CreateModelLoader());
	g_manager->SetMaterialLoader(g_renderer->CreateMaterialLoader());
	g_manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

	//左手座標系に修正
	//g_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

	//TODO: サウンドデータを使用するかも

	//初期投影行列を設定
	auto& proj = Effekseer::Matrix44().PerspectiveFovLH(60 * ToRadians, 16 / 9, 0.01f, 5000.0f);
	g_renderer->SetProjectionMatrix(proj);

	//初期カメラ行列を設定
	g_renderer->SetCameraMatrix(Effekseer::Matrix44().LookAtLH(Effekseer::Vector3D(0.0f, 0.0f, 0.0f), Effekseer::Vector3D(0.0f, 0.0f, 1.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	ApplicationChilled::GetApplication()->g_imGuiSystem->AddLog("INFO: EffekseerDevice Initialized.");

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

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void EffekseerDevice::Update()
{
	if (g_manager == nullptr) return;
	if (g_renderer == nullptr) return;

	auto time = static_cast<float>(ApplicationChilled::GetApplication()->g_fpsTimer->GetTotalTime());
	auto dtime = static_cast<float>(ApplicationChilled::GetApplication()->g_fpsTimer->GetDeltaTime());

	//カメラ行列を設定
	auto& camera = RENDERER.Getcb9().Get().m_view_matrix;
	auto& proj = RENDERER.Getcb9().Get().m_proj_matrix;
	g_renderer->SetCameraMatrix(ToE4x4(camera));
	g_renderer->SetProjectionMatrix(ToE4x4(proj));

	//マネージャーの更新
	g_manager->Update(dtime);

	//時間更新
	//g_renderer->SetTime(time);
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void EffekseerDevice::Draw()
{
	//エフェクトの描画開始処理
	g_renderer->BeginRendering();

	//エフェクトの描画処理
	g_manager->Draw();

	//エフェクトの描画終了処理
	g_renderer->EndRendering();
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool EffekseerDevice::Play(const std::u16string& filepath, const float3& position, const float magnification)
{
	//エフェクト生成
	const auto& instance = std::make_shared<EffekseerEffect>();
	if (instance)
	{
		//初期化/MAPに追加
		instance->Initialize(filepath, magnification);
		m_instanceMap.emplace(filepath, instance);
	}

	//指定された座標で再生
	instance->Play(position);

	return true;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void EffekseerDevice::StopAll()
{
	for (auto& pair : m_instanceMap)
		pair.second->Stop();

	g_manager->StopAllEffects();
}
