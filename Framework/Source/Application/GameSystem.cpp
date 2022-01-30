#include "GameSystem.h"
#include "main.h"

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
	m_spLoadingTex[0] = std::make_shared<Texture>();
	m_spLoadingTex[1] = std::make_shared<Texture>();
	m_spLoadingTex[2] = std::make_shared<Texture>();

	m_spLoadingTex[0]->Create("Resource/Texture/Load01.png");
	m_spLoadingTex[1]->Create("Resource/Texture/Load02.png");
	m_spLoadingTex[2]->Create("Resource/Texture/Load03.png");

	//m_testThread = (HANDLE)_beginthreadex(
	//	nullptr,
	//	0,
	//	BeginTimeCountThread,// thread関数
	//	nullptr,// thread関数への引数
	//	CREATE_SUSPENDED,
	//	nullptr
	//);

	//非同期読み込みテスト
	std::thread asyncLoad([=] { LoadScene("Resource/Jsons/TitleProcess.json"); });
	//std::thread asyncLoad([=] { LoadScene("Resource/Jsons/GameProcess.json"); });
	asyncLoad.detach();
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void GameSystem::Finalize()
{
	Serialize("Resource/Jsons/Serialize.json");

	for (auto& object : m_spActorList)
		object->Finalize();

	m_spActorList.clear();
	m_debugLines.clear();

	g_sceneFilepath.clear();

	//CloseHandle(m_testThread);
}

//-----------------------------------------------------------------------------
// シーンのシリアル処理
//-----------------------------------------------------------------------------
void GameSystem::Serialize(const std::string& filepath)
{
	json11::Json::object serial = {};
	json11::Json::array objectArray = {};

	// 一括シリアル化
	for (auto& object : m_spActorList)
		object->Serialize(objectArray);

	serial["actor_list"] = objectArray;

	// jsonファイル取得 ※初回は作成
	std::ofstream ofs(filepath);
	if (!ofs) return;

	// json objectをjsonに変換 -> シリアライズ
	constexpr bool debug = false;
	std::string strJson = json11::Json(serial).dump(debug);

	// 指定されたjsonファイルへ文字列を書き込み
	ofs.write(strJson.c_str(), strJson.size());
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void GameSystem::Update()
{
	if (!GetLockFlag()) return;

	const float& deltaTime = static_cast<float>(ApplicationChilled::GetApplication()->g_fpsTimer->GetDeltaTime());
	const float& totalTime = static_cast<float>(ApplicationChilled::GetApplication()->g_fpsTimer->GetTotalTime());

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

		// 除外
		if ((*itr)->g_enable == false)
		{
			(*itr)->OnDestroy();
			itr = m_spActorList.erase(itr);
		}
		else ++itr;
	}

	// 遷移予約の確認 -> 予約ありの場合 シーン遷移
	if (m_isRequestChangeScene)
		ExecChangeScene();

	g_cameraSystem.Update(deltaTime);
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void GameSystem::LateUpdate()
{
	if (!GetLockFlag()) return;

	const float& deltaTime = static_cast<float>(ApplicationChilled::GetApplication()->g_fpsTimer->GetDeltaTime());

	for (auto& object : m_spActorList)
		object->LateUpdate(deltaTime);
}

//-----------------------------------------------------------------------------
// 3D描画
//-----------------------------------------------------------------------------
void GameSystem::Draw()
{
	const float& deltaTime = static_cast<float>(ApplicationChilled::GetApplication()->g_fpsTimer->GetDeltaTime());

	// カメラ情報をGPUに転送
	g_cameraSystem.SetToDevice();

	if (!GetLockFlag()) return;

	//--------------------------------------------------
	// シャドウマップ描画
	//--------------------------------------------------
	/*for (int i = 0; i < 3; i++)
	{
		SHADER.GetShadowMapShader().Begin(i);

		for (auto& object : m_spActorList)
			object->DrawShadowMap(deltaTime);
	}
	SHADER.GetShadowMapShader().End();*/
	
	//--------------------------------------------------
	// 通常3D描画
	//--------------------------------------------------
	{
		//RENDERER.SetResources(SHADER.GetShadowMapShader().GetShadowMap(0).get(), 10);
		//RENDERER.SetResources(SHADER.GetShadowMapShader().GetShadowMap(1).get(), 11);
		//RENDERER.SetResources(SHADER.GetShadowMapShader().GetShadowMap(2).get(), 12);

		SHADER.GetModelShader().Begin();

		for (auto& object : m_spActorList)
			object->Draw(deltaTime);

		//RENDERER.SetNullResources(10);
		//RENDERER.SetNullResources(11);
		//RENDERER.SetNullResources(12);
	}
}

//-----------------------------------------------------------------------------
// 2D描画
//-----------------------------------------------------------------------------
void GameSystem::Draw2D()
{
	const float& deltaTime = static_cast<float>(ApplicationChilled::GetApplication()->g_fpsTimer->GetDeltaTime());

	//--------------------------------------------------
	// Effect描画
	//--------------------------------------------------
	if (GetLockFlag())
	{
		SHADER.GetEffectShader().Begin();

		// TODO: ここで3DモデルのEffect描画

		// RENDERERの詳細パラメータを設定
		RENDERER.SetResources(ApplicationChilled::GetApplication()->g_graphicsDevice->GetWhiteTex().get());
		RENDERER.SetDepthStencil(false, false);
		RENDERER.Getcb8().Work().m_world_matrix = mfloat4x4::Identity;
		RENDERER.Getcb8().Write();

		// DebugLines
		if (m_debugLines.size() >= 1)
		{
			ApplicationChilled::GetApplication()->g_graphicsDevice->DrawVertices(
				ApplicationChilled::GetApplication()->g_graphicsDevice->g_cpContext.Get(),
				D3D_PRIMITIVE_TOPOLOGY_LINELIST, (UINT)m_debugLines.size(), &m_debugLines[0], sizeof(EffectShader::Vertex));
			m_debugLines.clear();
		}

		// DepthStencilを元に戻す
		RENDERER.SetDepthStencil(true, true);
	}

	//--------------------------------------------------
	// Sprite描画
	//--------------------------------------------------
	{
		SHADER.GetSpriteShader().Begin(ApplicationChilled::GetApplication()->g_graphicsDevice->g_cpContext.Get(), true, true);

		if (!GetLockFlag())
		{
			static int index = 0;

			static float numTime = 0;
			numTime += static_cast<float>(ApplicationChilled::GetApplication()->g_fpsTimer->GetDeltaTime());

			if (numTime >= 0.5f)
			{
				numTime = 0.0f;
				index++;
				if (index >= 3) index = 0;
			}

			SHADER.GetSpriteShader().DrawTexture(m_spLoadingTex[index].get(), float2(0, 0));
		}
		else
		{
			for (auto& object : m_spActorList)
				object->DrawSprite(deltaTime);
		}

		SHADER.GetSpriteShader().End(ApplicationChilled::GetApplication()->g_graphicsDevice->g_cpContext.Get());
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
	ver.m_uv	= float2::Zero;

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
	SetLockFlag(false);
	std::thread([=]
		{
			LoadScene(g_sceneFilepath);
			//LoadScene("Resource/Jsons/GameProcess.json");
		}
	).detach();
}

//-----------------------------------------------------------------------------
// シーンの読み込み
//-----------------------------------------------------------------------------
bool GameSystem::LoadScene(const std::string& filepath)
{
	Reset();
	
	json11::Json jsonObject = RES_FAC.GetJsonData(filepath);

	//ログを投げる
	if (jsonObject.is_null())
	{
		std::string post = "ERROR: Failed to load jsonfile. path: ";
		post += filepath;
		ApplicationChilled::GetApplication()->g_imGuiSystem->AddLog(post);
		return false;
	}
	else
	{
		std::string post = "INFO: Load jsonfile. path: ";
		post += filepath;
		ApplicationChilled::GetApplication()->g_imGuiSystem->AddLog(post);
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

	//auto sleepTime = std::chrono::seconds(1);
	//std::this_thread::sleep_for(sleepTime);

	SetLockFlag(true);

	m_isRequestChangeScene = false;

	return true;
}

unsigned int __stdcall GameSystem::BeginTimeCountThread(void* usercontext)
{
	HRESULT hr = S_FALSE;

	for (;;)
	{
		// ワークキューのエントリーを待つ
		//WaitForSingleObject(m_testThread, INFINITE);

		//ここで各更新
	}

	return 0;
}
