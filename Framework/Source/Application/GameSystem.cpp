#include "GameSystem.h"
#include "Actors/Actor.h"
#include "main.h"

#include "../Framework/Audio/SoundDirector.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GameSystem::GameSystem()
	: m_spCamera(nullptr)
	, m_spActorList()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void GameSystem::Initialize()
{
	// 大照明設定
	RENDERER.SetDirectionalLightDir(float3(1, -1, 0));
	// ディザリング設定
	RENDERER.SetDitherEnable(false);

	//--------------------------------------------------
	// ActorList初期化
	// TODO: 外部ファイルへ
	//--------------------------------------------------
	{
		AddActor("Human");
		AddActor("Sky");
		AddActor("StageMap");
		AddActor("Tank");

		auto object = std::make_shared<Actor>();
		object->LoadModel("Resource/Model/DebugSphere.gltf");
		m_spActorList.push_back(object);
	}

	for (auto& object : m_spActorList)
		object->Initialize();

	// BGM再生
	auto sound = SOUND_DIRECTOR.CreateSoundWork("Resource/Audio/New Happy Day by fennec beats.wav", true, true);
	if (sound)
	{
		//sound->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 0.08f);
		sound->Play();
	}

	auto sound3d = SOUND_DIRECTOR.CreateSoundWork3D("Resource/Audio/heli.wav", true);
	if (sound3d)
	{
		float3 pos = float3(0, 0, 0);
		sound3d->Play3D(pos);
		sound3d->SetVolume(0.0f);
	}
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void GameSystem::Finalize()
{
	for (auto& object : m_spActorList)
		object->Finalize();
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void GameSystem::Update()
{
	// 前フレームからの経過時間を計算/取得
	const float deltaTime = static_cast<float>(APP.g_fpsTimer->GetDeltaTime());
	const float totalTime = static_cast<float>(APP.g_fpsTimer->GetTotalTime());

	// 時間系を設定/送信
	RENDERER.SetTime(totalTime, deltaTime);

	// Actorリスト更新
	for (auto itr = m_spActorList.begin(); itr != m_spActorList.end();)
	{
		if ((*itr) == nullptr)
			continue;

		// 更新
		(*itr)->Update(deltaTime);

		// 除外？
		if ((*itr)->IsEnable() == false)
			itr = m_spActorList.erase(itr);
		else
			++itr;
	}
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void GameSystem::LateUpdate()
{
	const float deltaTime = static_cast<float>(APP.g_fpsTimer->GetDeltaTime());

	for (auto& object : m_spActorList)
		object->LateUpdate(deltaTime);
}

//-----------------------------------------------------------------------------
// 3D描画
//-----------------------------------------------------------------------------
void GameSystem::Draw()
{
	const float deltaTime = static_cast<float>(APP.g_fpsTimer->GetDeltaTime());

	// カメラ情報をGPUに転送
	if (m_spCamera)
		m_spCamera->SetToShader();

	SHADER.GetModelShader().Begin();

	for (auto& object : m_spActorList)
		object->Draw(deltaTime);
}

//-----------------------------------------------------------------------------
// 2D描画
//-----------------------------------------------------------------------------
void GameSystem::Draw2D()
{
	const float deltaTime = static_cast<float>(APP.g_fpsTimer->GetDeltaTime());

	SHADER.GetSpriteShader().Begin(true, true);

	SHADER.GetSpriteShader().End();
}

//-----------------------------------------------------------------------------
// Actorをシーンに追加
//-----------------------------------------------------------------------------
void GameSystem::AddActor(const std::string& name)
{
	auto object = GenerateActor(name);

	// TODO: NullActor
	if (object == nullptr) return;

	m_spActorList.push_back(object);
}
