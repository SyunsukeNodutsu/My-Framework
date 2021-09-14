#include "GameSystem.h"
#include "main.h"
#include "../Framework/Utility/Cpuid.h"
#include "GameObjects/GameObject.h"
#include "../Framework/Audio/AudioDeviceChild.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GameSystem::GameSystem()
	: m_fpsTimer()
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

	Cpuid::Research();

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
		AddGameObject("Tank");
	}

	for (auto& object : m_spObjectList)
		object->Initialize();

	// 音
	AUDIO.SetUserSettingVolume(0.2f);
	AUDIO.Play("Resource/Audio/さよならの支度.wav", true);
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
	m_fpsTimer.Tick();
	const float deltaTime = static_cast<float>(m_fpsTimer.GetDeltaTime());
	const float totalTime = static_cast<float>(m_fpsTimer.GetTotalTime());

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

	if (RAW_INPUT.GetKeyboard()->IsPressed(KeyCode::Up))
	{
		auto volume = AUDIO.GetUserSettingVolume();
		AUDIO.SetUserSettingVolume(volume += 0.02f);
	}
	if (RAW_INPUT.GetKeyboard()->IsPressed(KeyCode::Down))
	{
		auto volume = AUDIO.GetUserSettingVolume();
		AUDIO.SetUserSettingVolume(volume -= 0.02f);
	}

	if (RAW_INPUT.GetMouse()->IsPressed(MouseButton::Left))
	{
		float2 mpos = RAW_INPUT.GetMouse()->GetMousePos();

		BOOL isFullscreen = false;
		D3D.GetSwapChain()->GetFullscreenState(&isFullscreen, 0);
		float2 srect = isFullscreen ? float2(1920, 1080) : float2(1280, 720);

		if (mpos.x >= srect.x - 150 && mpos.x <= srect.x &&
			mpos.y >= srect.y - 50 && mpos.y <= srect.y)
			D3D.GetSwapChain()->SetFullscreenState(!isFullscreen, 0);
	}

	if (RAW_INPUT.GetKeyboard()->IsPressed(KeyCode::O))
		AUDIO.StopAllSound();
}

//-----------------------------------------------------------------------------
// 描画後更新
//-----------------------------------------------------------------------------
void GameSystem::LateUpdate()
{
	const float deltaTime = static_cast<float>(m_fpsTimer.GetDeltaTime());

	for (auto& object : m_spObjectList)
		object->LateUpdate(deltaTime);
}

//-----------------------------------------------------------------------------
// 3D描画
//-----------------------------------------------------------------------------
void GameSystem::Draw()
{
	const float deltaTime = static_cast<float>(m_fpsTimer.GetDeltaTime());

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
	const float deltaTime = static_cast<float>(m_fpsTimer.GetDeltaTime());

	//--------------------------------------------------
	// 2D画像描画
	//--------------------------------------------------

	SHADER.GetSpriteShader().Begin(true, true);

	constexpr float2 texture_port = float2(1920, 1080);
	constexpr float2 texture_rect = float2(150, 50);

	SHADER.GetSpriteShader().DrawTexture(
		spTexture.get(),
		float2(
			(texture_port.x * 0.5f) - (texture_rect.x * 0.5f),// 885
			(texture_port.y * -0.5f) - (texture_rect.y * -0.5f)// -515
		)
	);

	SHADER.GetSpriteShader().End();
}

//-----------------------------------------------------------------------------
// imGui描画
//-----------------------------------------------------------------------------
void GameSystem::DrawImGui()
{
	// 
	static bool no_titlebar		= false;
	static bool no_scrollbar	= false;
	static bool no_menu			= false;
	static bool no_move			= false;
	static bool no_resize		= false;
	static bool no_close		= false;
	static bool no_background	= false;

	ImGuiWindowFlags window_flags = 0;
	if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
	if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
	if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
	if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;

	if (!ImGui::Begin("Debug", nullptr, window_flags))
	{
		// 最適化のために ウィンドウが折りたたまれている場合は初期化
		ImGui::End();
		return;
	}

	ImGui::Text(Cpuid::m_brand.c_str());
	ImGui::Text(wide_to_sjis(D3D.GetAdapterName()).c_str());

	ImGui::Text(std::string("Fps: " + std::to_string(m_fpsTimer.GetFPS())).c_str());
	ImGui::Text(std::string("DeltaTime: " + std::to_string(m_fpsTimer.GetDeltaTime())).c_str());
	ImGui::Text(std::string("TotalTime: " + std::to_string(m_fpsTimer.GetTotalTime())).c_str());

	ImGui::Text(std::string("Volume: " + std::to_string(AUDIO.GetUserSettingVolume())).c_str());

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Quit", "Alt+F4")) { APP.End(); }

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Examples"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (ImGui::CollapsingHeader("Window options"))
	{
		if (ImGui::BeginTable("split", 2))
		{
			ImGui::TableNextColumn(); ImGui::Checkbox("No titlebar", &no_titlebar);
			ImGui::TableNextColumn(); ImGui::Checkbox("No scrollbar", &no_scrollbar);
			ImGui::TableNextColumn(); ImGui::Checkbox("No menu", &no_menu);
			ImGui::TableNextColumn(); ImGui::Checkbox("No move", &no_move);
			ImGui::TableNextColumn(); ImGui::Checkbox("No resize", &no_resize);
			ImGui::TableNextColumn(); ImGui::Checkbox("No close", &no_close);
			ImGui::TableNextColumn(); ImGui::Checkbox("No background", &no_background);
			ImGui::EndTable();
		}
	}

	ImGui::Separator();

	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::ColorEdit4("##color", (float*)&style.Colors[ImGuiCol_WindowBg], ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);

	ImGui::End();
}

//-----------------------------------------------------------------------------
// GameObjectをシーンに追加
//-----------------------------------------------------------------------------
void GameSystem::AddGameObject(const std::string& name)
{
	auto object = GenerateGameObject(name);
	if (object == nullptr)
		return;

	m_spObjectList.push_back(object);
}
