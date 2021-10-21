#include "GameSystem.h"
#include "main.h"

#include "Actors/Actor.h"

#include "../Framework/Audio/SoundDirector.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GameSystem::GameSystem()
	: g_cameraSystem()
	, m_spActorList()
	, m_debugLines()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void GameSystem::Initialize()
{
	//--------------------------------------------------
	// ActorList初期化
	// TODO: 外部ファイルへ
	//--------------------------------------------------
	{
		//AddActor("Human");
		AddActor("Sky");
		AddActor("StageMap");
		AddActor("Tank");
		AddActor("Tree");
	}

	for (auto& object : m_spActorList)
		object->Initialize();

	// BGM再生
	// 音量操作や停止、フィルターなどをかける場合は インスタンスを取得
	auto sound = SOUND_DIRECTOR.CreateSoundWork("Resource/Audio/BGM/Germany.wav", true, true);
	if (sound)
	{
		sound->Play(1000);
		sound->SetVolume(0.36f);
	}

	// 8分木マネージャ初期化
	int level = 6;
	float3 min = float3(-300);// TODO: 適当な空間定義
	float3 max = float3(300);
	m_actorOctreeManager.Initialize(level, min, max);
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void GameSystem::Finalize()
{
	for (auto& object : m_spActorList)
		object->Finalize();

	m_debugLines.clear();
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void GameSystem::Update()
{
	// 前フレームからの経過時間を計算/取得
	const float& deltaTime = static_cast<float>(APP.g_fpsTimer->GetDeltaTime());
	const float& totalTime = static_cast<float>(APP.g_fpsTimer->GetTotalTime());

	// 時間系を設定/送信
	RENDERER.Getcb12().Work().m_deltaTime = deltaTime;
	RENDERER.Getcb12().Work().m_totalTime = totalTime;
	RENDERER.Getcb12().Write();

	// Actorリスト更新
	for (auto itr = m_spActorList.begin(); itr != m_spActorList.end();)
	{
		if ((*itr) == nullptr) continue;

		// 更新
		(*itr)->Update(deltaTime);

		// 除外？
		if ((*itr)->g_enable == false)
			itr = m_spActorList.erase(itr);
		else
			++itr;
	}

	// カメラシステム更新
	g_cameraSystem.Update(deltaTime);
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void GameSystem::LateUpdate()
{
	const float& deltaTime = static_cast<float>(APP.g_fpsTimer->GetDeltaTime());

	for (auto& object : m_spActorList)
		object->LateUpdate(deltaTime);
}

//-----------------------------------------------------------------------------
// 3D描画
//-----------------------------------------------------------------------------
void GameSystem::Draw()
{
	const float& deltaTime = static_cast<float>(APP.g_fpsTimer->GetDeltaTime());

	// カメラ情報をGPUに転送
	g_cameraSystem.SetToDevice();

	// シャドウマップ描画
	for (int i = 0; i < 3; i++)
	{
		SHADER.GetShadowMapShader().Begin(i);

		for (auto& object : m_spActorList)
			object->DrawShadowMap(deltaTime);
	}
	SHADER.GetShadowMapShader().End();
	
	// 通常3D描画
	{
		RENDERER.SetResources(SHADER.GetShadowMapShader().GetShadowMap(0).get(), 10);
		RENDERER.SetResources(SHADER.GetShadowMapShader().GetShadowMap(1).get(), 11);
		RENDERER.SetResources(SHADER.GetShadowMapShader().GetShadowMap(2).get(), 12);

		SHADER.GetModelShader().Begin();

		for (auto& object : m_spActorList)
			object->Draw(deltaTime);

		RENDERER.SetNullResources(10);
		RENDERER.SetNullResources(11);
		RENDERER.SetNullResources(12);
	}
}

//-----------------------------------------------------------------------------
// 2D描画
//-----------------------------------------------------------------------------
void GameSystem::Draw2D()
{
	const float& deltaTime = static_cast<float>(APP.g_fpsTimer->GetDeltaTime());

	// Effect描画
	{
		SHADER.GetEffectShader().Begin();

		// TODO: ここで3DモデルのEffect描画

		// RENDERERの詳細パラメータを設定
		RENDERER.SetResources(APP.g_graphicsDevice->GetWhiteTex().get());
		RENDERER.SetDepthStencil(false, false);
		RENDERER.Getcb8().Work().m_world_matrix = mfloat4x4::Identity;
		RENDERER.Getcb8().Write();

		// DebugLines
		if (m_debugLines.size() >= 1)
		{
			APP.g_graphicsDevice->DrawVertices(D3D_PRIMITIVE_TOPOLOGY_LINELIST, m_debugLines.size(), &m_debugLines[0], sizeof(EffectShader::Vertex));
			m_debugLines.clear();
		}

		// DepthStencilを元に戻す
		RENDERER.SetDepthStencil(true, true);
	}

	// Sprite描画
	{
		SHADER.GetSpriteShader().Begin(true, true);

		for (auto& object : m_spActorList)
			object->DrawSprite(deltaTime);

		SHADER.GetSpriteShader().End();
	}
}

//-----------------------------------------------------------------------------
// シーンのActorリストに新規Actorを追加
//-----------------------------------------------------------------------------
void GameSystem::AddActorList(std::shared_ptr<Actor> actor)
{
	if (!actor) return;

	m_spActorList.push_back(actor);
}

//-----------------------------------------------------------------------------
// デバッグ用の3D線を追加
//-----------------------------------------------------------------------------
void GameSystem::AddDebugLine(const float3& pos01, const float3& pos02, const cfloat4x4 color)
{
	EffectShader::Vertex ver = {};

	// ラインの開始頂点
	ver.m_position = pos01;
	ver.m_color = color;
	ver.m_uv = float2::Zero;
	m_debugLines.push_back(ver);

	// ラインの終了頂点
	ver.m_position = pos02;
	m_debugLines.push_back(ver);
}

//-----------------------------------------------------------------------------
// デバッグ用の3D球を追加
//-----------------------------------------------------------------------------
void GameSystem::AddDebugSphereLine(const float3& pos, const float radius, const cfloat4x4 color)
{
	EffectShader::Vertex ver = {};
	ver.m_color = color;
	ver.m_uv = float2::Zero;

	static constexpr int detail = 32;
	for (UINT i = 0; i < detail + 1; ++i)
	{
		// XZ平面
		ver.m_position = pos;
		ver.m_position.x += cosf((float)i * (360 / detail) * ToRadians) * radius;
		ver.m_position.z += sinf((float)i * (360 / detail) * ToRadians) * radius;
		m_debugLines.push_back(ver);

		ver.m_position = pos;
		ver.m_position.x += cosf((float)(i + 1) * (360 / detail) * ToRadians) * radius;
		ver.m_position.z += sinf((float)(i + 1) * (360 / detail) * ToRadians) * radius;
		m_debugLines.push_back(ver);

		// XY平面
		ver.m_position = pos;
		ver.m_position.x += cosf((float)i * (360 / detail) * ToRadians) * radius;
		ver.m_position.y += sinf((float)i * (360 / detail) * ToRadians) * radius;
		m_debugLines.push_back(ver);

		ver.m_position = pos;
		ver.m_position.x += cosf((float)(i + 1) * (360 / detail) * ToRadians) * radius;
		ver.m_position.y += sinf((float)(i + 1) * (360 / detail) * ToRadians) * radius;
		m_debugLines.push_back(ver);

		// YZ平面
		ver.m_position = pos;
		ver.m_position.y += cosf((float)i * (360 / detail) * ToRadians) * radius;
		ver.m_position.z += sinf((float)i * (360 / detail) * ToRadians) * radius;
		m_debugLines.push_back(ver);

		ver.m_position = pos;
		ver.m_position.y += cosf((float)(i + 1) * (360 / detail) * ToRadians) * radius;
		ver.m_position.z += sinf((float)(i + 1) * (360 / detail) * ToRadians) * radius;
		m_debugLines.push_back(ver);
	}
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

//-----------------------------------------------------------------------------
// シーンの読み込み
//-----------------------------------------------------------------------------
bool GameSystem::LoadScene(const std::string& filepath)
{
	json json = RES_FAC.GetJsonData(filepath);
	if (json.is_null()) {
		return false;
	}

	// json object の一覧
	auto actorList = json::array();
	actorList = json["ActorList"].dump();

	// 生成ループ
	for (auto&& actor : actorList)
	{
		const auto& str = actor["ClassName"].dump();
		const std::shared_ptr<Actor>& newActor = GenerateActor(str);

		if (newActor == nullptr) {
			APP.g_imGuiSystem->AddLog("Failed to create actor.");
			continue;
		}

		AddActorList(newActor);
	}

	return true;
}

/*
* #pragma region Json test.
	// デシリアライズ
	std::ifstream ifs("Resource/test.json");
	if (!ifs.fail()) {
		std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		json jobj = json::parse(strJson);
		//auto pi = jobj["pi"].dump();// 文字列で取得？
		auto pi = jobj["pi"].get<float>();// floatで取得
		DebugLog(std::string("pi: " + std::to_string(pi) + "\n").c_str());
	}

	// シリアライズ
	json j;
	j["pi"] = 3.141;
	j["happy"] = true;
	j["name"] = "Niels";
	j["nothing"] = nullptr;
	j["answer"]["everything"] = 42;  // 存在しないキーを指定するとobjectが構築される
	j["list"] = { 1, 0, 2 };         // [1,0,2]
	j["object"] = { {"currency", "USD"}, {"value", 42.99} };  // {"currentcy": "USD", "value": 42.99}

	std::string s = j.dump(1);// 引数1...インデント
	std::ofstream ofs("Resource/test2.json");
	if (ofs) {
		ofs.write(s.c_str(), s.size());
	}
#pragma endregion
*/
