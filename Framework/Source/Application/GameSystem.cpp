#include "GameSystem.h"
#include "main.h"

#include "Actors/Actor.h"

#include "../Framework/Audio/SoundDirector.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GameSystem::GameSystem()
	: g_cameraSystem()
	, g_sceneFilepath("")
	, m_spActorList()
	, m_debugLines()
	, m_isRequestChangeScene(false)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void GameSystem::Initialize()
{
	// シーン読み込み ※ActorList初期化
	LoadScene("Resource/Jsons/TitleProcess.json");

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

	m_spActorList.clear();
	m_debugLines.clear();

	g_sceneFilepath.clear();
}

//-----------------------------------------------------------------------------
// シーンのシリアル処理
//-----------------------------------------------------------------------------
void GameSystem::Serialize(const std::string& filepath)
{
	// 保存したい値を集約
	json11::Json::object serial = {};
	json11::Json::array objectArray = {};

	// 一括シリアル化
	for (auto& object : m_spActorList)
		object->Serialize(objectArray);

	serial["actor_list"] = objectArray;

	// jsonファイル取得(初回は作成)
	std::ofstream ofs(filepath);
	if (!ofs) return;

	// オブジェクトをjsonへ変換
	json11::Json json = serial;
	// jsonのフォーマットにあった書式に変換しながら文字列に変換
	std::string strJson = json.dumpDebug();

	// 指定されたjsonファイルへ文字列を書き込み
	ofs.write(strJson.c_str(), strJson.size());
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

	// 遷移予約確認
	if (m_isRequestChangeScene)
		ExecChangeScene();

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
// シーンの遷移を予約
//-----------------------------------------------------------------------------
void GameSystem::RequestChangeScene(const std::string& filepath)
{
	g_sceneFilepath = filepath;
	m_isRequestChangeScene = true;
}

//-----------------------------------------------------------------------------
// シーンのActorリストに新規Actorを追加
//-----------------------------------------------------------------------------
void GameSystem::AddActor(std::shared_ptr<Actor> actor)
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
	ver.m_position	= pos01;
	ver.m_color		= color;
	ver.m_uv		= float2::Zero;
	m_debugLines.push_back(ver);

	// ラインの終了頂点
	ver.m_position	= pos02;
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
// シーンの切り替え前にリセット
//-----------------------------------------------------------------------------
void GameSystem::Reset()
{
	for (auto& object : m_spActorList)
		object->Finalize();

	m_spActorList.clear();
	m_debugLines.clear();

	g_cameraSystem.ResetCameraList();
}

//-----------------------------------------------------------------------------
// シーンの切り替えを実行
//-----------------------------------------------------------------------------
void GameSystem::ExecChangeScene()
{
	LoadScene(g_sceneFilepath);
	m_isRequestChangeScene = false;
}

//-----------------------------------------------------------------------------
// シーンの読み込み
//-----------------------------------------------------------------------------
bool GameSystem::LoadScene(const std::string& filepath)
{
	Reset();

	json11::Json jsonObject = RES_FAC.GetJsonData(filepath);
	if (jsonObject.is_null()) {
		APP.g_imGuiSystem->AddLog(
			std::string("ERROR: Failed to load Json file. path: " + filepath).c_str()
		);
		return false;
	}
	else {
		APP.g_imGuiSystem->AddLog(
			std::string("INFO: Load jsonfile. path: " + filepath).c_str()
		);
	}

	auto& jsonObjectList = jsonObject["actor_list"].array_items();

	for (auto&& json : jsonObjectList)
	{
		auto newActor = GenerateActor(json["class_name"].string_value());
		if (newActor == nullptr) continue;

		// プレハブ確認/差し替え
		MergePrefab(json);
		// json objectの逆シリアル化 Actorのデータを初期化
		newActor->Deserialize(json);

		m_spActorList.push_back(newActor);
	}

	// 生成を終えた後に一括で初期化
	for (auto& object : m_spActorList)
		object->Initialize();

	return true;
}
