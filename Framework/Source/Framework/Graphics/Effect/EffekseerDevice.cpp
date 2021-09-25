#include "EffekseerDevice.h"
#include "../../../Application/main.h"

static Effekseer::Matrix44 ToE4x4(mfloat4x4 matrix)
{
	Effekseer::Matrix44 result = {};
	for (int height = 0; height < 4; height++)
	{
		for (int width = 0; width < 4; width++)
		{
			result.Values[height][width] = matrix.m[height][width];
		}
	}
	return result;
}

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
	//Effekseer作成
	g_renderer = EffekseerRendererDX11::Renderer::Create(
		g_graphicsDevice->g_cpDevice.Get(), g_graphicsDevice->g_cpContext.Get(), MAX_EFFECT
	);

	// エフェクトのマネージャーの作成
	g_manager = Effekseer::Manager::Create(MAX_EFFECT);

	// 描画モジュールの設定
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

	// Effekseerはデフォルトが右手座標系なので、左手座標系に修正する
	g_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

	// 投影行列を設定(CameraComponentの設定を引き継ぎ)
	g_renderer->SetProjectionMatrix(::Effekseer::Matrix44().PerspectiveFovLH(
		60 * ToRadians, 16 / 9, 0.01f, 5000.0f));

	// カメラ行列を入れておかないと落ちるので、適当な値を代入
	g_renderer->SetCameraMatrix(
		Effekseer::Matrix44().LookAtLH(Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 0.0f, 1.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	APP.g_imGuiSystem->AddLog("INFO: EffekseerDevice Initialized.");

	return true;
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void EffekseerDevice::Finalize()
{
	g_manager->Destroy();
	g_renderer->Destroy();
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void EffekseerDevice::Update(float deltaTime)
{
	if (g_manager == nullptr) return;
	if (g_renderer == nullptr) return;

	// カメラ行列を設定
	auto camera = RENDERER.Getcb9().Get().m_view_matrix;
	auto proj = RENDERER.Getcb9().Get().m_proj_matrix;
	g_renderer->SetCameraMatrix(ToE4x4(camera));
	g_renderer->SetProjectionMatrix(ToE4x4(proj));

	// マネージャーの更新
	g_manager->Update();

	// インスタンス一覧更新
	for (auto& pair : m_instanceMap)
		pair.second->Update(m_speed * deltaTime);
	
	// エフェクトの描画開始処理
	g_renderer->BeginRendering();

	// エフェクトの描画処理
	g_manager->Draw();

	// エフェクトの描画終了処理
	g_renderer->EndRendering();
}

bool EffekseerDevice::Play(const std::u16string& filepath, float3& position)
{
	//再生するエフェクトを検索
	const auto& instance = FindEffect(filepath);
	if (instance == nullptr) { return false; }

	//指定された座標で再生
	instance->Play(position);
	return true;
}

std::shared_ptr<EffectWork> EffekseerDevice::LoadEffect(const std::u16string& filepath)
{
	const auto& effect = std::make_shared<EffectData>();
	bool isLoad = effect->Load(filepath);
	if (isLoad == false)
		return nullptr;

	const auto& instance = std::make_shared<EffectWork>();
	instance->Initialize(effect);

	m_instanceMap.emplace(filepath, instance);
	return instance;
}

std::shared_ptr<EffectWork> EffekseerDevice::FindEffect(const std::u16string& filepath)
{
	auto pair = m_instanceMap.find(filepath);
	if (pair == m_instanceMap.end())
		return LoadEffect(filepath);// 新規読み込み
	else
		return pair->second;// 使いまわし
}

void EffekseerDevice::StopAll()
{
	for (auto& pair : m_instanceMap)
		pair.second->Stop();

	g_manager->StopAllEffects();
}
