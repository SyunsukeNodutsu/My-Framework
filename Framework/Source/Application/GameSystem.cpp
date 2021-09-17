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
	}

	for (auto& object : m_spObjectList)
		object->Initialize();

	//// 音
	//AUDIO.SetUserSettingVolume(0.3f);
	//AUDIO.Play("", true);
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void GameSystem::Finalize()
{
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

	//--------------------------------------------------
	// 2D画像描画
	//--------------------------------------------------

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
