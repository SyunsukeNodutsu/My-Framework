#include "GameSystem.h"
#include "GameObjects/GameObject.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GameSystem::GameSystem()
	: g_fpsTimer()
	, m_spCamera(nullptr)
	, m_spObjectList()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void GameSystem::Initialize()
{
	// テスト用テクスチャ
	spTexture = std::make_shared<Texture>();
	spTexture->Create("Resource/Texture/screen_toggle.png");

	// 大照明設定
	D3D.GetRenderer().SetDirectionalLightDir(float3(1, -1, 0));
	// ディザリング設定
	D3D.GetRenderer().SetDitherEnable(false);

	//--------------------------------------------------
	// GameObjectList初期化
	// TODO: 外部ファイルへ
	//--------------------------------------------------
	{
		AddGameObject("Human");
		AddGameObject("Sky");
		AddGameObject("StageMap");
		//AddGameObject("Tank");

		auto object = std::make_shared<GameObject>();
		object->LoadModel("Resource/Model/DebugSphere.gltf");
		m_spObjectList.push_back(object);
	}

	for (auto& object : m_spObjectList)
		object->Initialize();

	// BGM再生
	m_spSoundWork = std::make_shared<SoundWork>();
	m_spSoundWork->Load("Resource/Audio/MusicMono.wav", false);
	m_spSoundWork->Play(1000);
	m_spSoundWork->SetPan(-1);

	m_spSoundWork3D = std::make_shared<SoundWork3D>();
	m_spSoundWork3D->Load("Resource/Audio/heli.wav", true);
	float3 pos = float3(0, 0, 0);
	m_spSoundWork3D->Play3D(pos);
	m_spSoundWork3D->SetVolume(0);
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void GameSystem::Finalize()
{
	m_spSoundWork->Release();
	m_spSoundWork3D->Release();

	for (auto& object : m_spObjectList)
		object->Finalize();
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void GameSystem::Update()
{
	// 前フレームからの経過時間を計算/取得
	g_fpsTimer.Tick();
	const float deltaTime = static_cast<float>(g_fpsTimer.GetDeltaTime());
	const float totalTime = static_cast<float>(g_fpsTimer.GetTotalTime());

	// 時間系を設定/送信
	D3D.GetRenderer().SetTime(totalTime, deltaTime);

	// GameObjectリスト更新
	for (auto itr = m_spObjectList.begin(); itr != m_spObjectList.end();)
	{
		if ((*itr) == nullptr)
			continue;

		// 更新
		(*itr)->Update(deltaTime);

		// 除外？
		if ((*itr)->IsEnable() == false)
			itr = m_spObjectList.erase(itr);
		else
			++itr;
	}

	m_spSoundWork3D->Update();
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void GameSystem::LateUpdate()
{
	const float deltaTime = static_cast<float>(g_fpsTimer.GetDeltaTime());

	for (auto& object : m_spObjectList)
		object->LateUpdate(deltaTime);
}

//-----------------------------------------------------------------------------
// 3D描画
//-----------------------------------------------------------------------------
void GameSystem::Draw()
{
	const float deltaTime = static_cast<float>(g_fpsTimer.GetDeltaTime());

	// カメラ情報をGPUに転送
	if (m_spCamera)
		m_spCamera->SetToShader();

	SHADER.GetModelShader().Begin();

	for (auto& object : m_spObjectList)
		object->Draw(deltaTime);
}

//-----------------------------------------------------------------------------
// 2D描画
//-----------------------------------------------------------------------------
void GameSystem::Draw2D()
{
	const float deltaTime = static_cast<float>(g_fpsTimer.GetDeltaTime());

	SHADER.GetSpriteShader().Begin(true, true);

	SHADER.GetSpriteShader().End();
}

//-----------------------------------------------------------------------------
// GameObjectをシーンに追加
//-----------------------------------------------------------------------------
void GameSystem::AddGameObject(const std::string& name)
{
	auto object = GenerateGameObject(name);

	// TODO: NullActor
	if (object == nullptr) return;

	m_spObjectList.push_back(object);
}
