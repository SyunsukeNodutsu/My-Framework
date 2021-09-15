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
		//AddGameObject("Tank");
	}

	for (auto& object : m_spObjectList)
		object->Initialize();

	// 音
	//AUDIO.Play("Resource/Audio/さよならの支度.wav", true);
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

	SHADER.GetSpriteShader().End();
}
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))
//-----------------------------------------------------------------------------
// imGui描画
//-----------------------------------------------------------------------------
void GameSystem::DrawImGui()
{
	ImGui::ShowDemoWindow();

	ImGuiWindowFlags window_flags = 0;
	//ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

	//-----------------------------------
	ImGui::Begin("Layers Monitor", nullptr, window_flags | ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Save", "Ctrl+S")) {}
			if (ImGui::MenuItem("Quit", "Alt+F4")) { APP.End(); }
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	
	ImGui::Text("Dear imGui version. (%s)", IMGUI_VERSION);
	ImGui::Spacing();

	ImGuiColorEditFlags cflags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview;
	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::ColorEdit4("Text", (float*)&style.Colors[ImGuiCol_Text], cflags);
	ImGui::ColorEdit4("WindowBg", (float*)&style.Colors[ImGuiCol_WindowBg], cflags);

	ImGui::End();

	//-----------------------------------
	ImGui::Begin("Input Monitor", nullptr, window_flags);
	ImGui::End();

	//-----------------------------------
	ImGui::Begin("Profiler Monitor", nullptr, window_flags);
	ImGui::Text(Cpuid::m_brand.c_str());
	ImGui::Text(wide_to_sjis(D3D.GetAdapterName()).c_str());
	ImGui::Text(std::string("Fps: " + std::to_string(m_fpsTimer.GetFPS())).c_str());
	ImGui::Text(std::string("DeltaTime: " + std::to_string(m_fpsTimer.GetDeltaTime())).c_str());
	ImGui::Text(std::string("TotalTime: " + std::to_string(m_fpsTimer.GetTotalTime())).c_str());

	// Plot/Graph widgets are not very good.
	// Consider using a third-party library such as ImPlot: https://github.com/epezent/implot
	// (see others https://github.com/ocornut/imgui/wiki/Useful-Extensions)
	if (ImGui::TreeNode("Plots Widgets"))
	{
		static bool animate = true;
		ImGui::Checkbox("Animate", &animate);

		// Plot as lines and plot as histogram
		static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
		ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));
		ImGui::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));

		// Fill an array of contiguous float values to plot
		// Tip: If your float aren't contiguous but part of a structure, you can pass a pointer to your first float
		// and the sizeof() of your structure in the "stride" parameter.
		static float values[90] = {};
		static int values_offset = 0;
		static double refresh_time = 0.0;
		if (!animate || refresh_time == 0.0)
			refresh_time = ImGui::GetTime();
		while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
		{
			static float phase = 0.0f;
			values[values_offset] = cosf(phase);
			values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
			phase += 0.10f * values_offset;
			refresh_time += 1.0f / 60.0f;
		}

		// Plots can display overlay texts
		// (in this example, we will display an average value)
		{
			float average = 0.0f;
			for (int n = 0; n < IM_ARRAYSIZE(values); n++)
				average += values[n];
			average /= (float)IM_ARRAYSIZE(values);
			char overlay[32];
			//sprintf(overlay, "avg %f", average);
			ImGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));
		}

		// Use functions to generate output
		// FIXME: This is rather awkward because current plot API only pass in indices.
		// We probably want an API passing floats and user provide sample rate/count.
		struct Funcs
		{
			static float Sin(void*, int i) { return sinf(i * 0.1f); }
			static float Saw(void*, int i) { return (i & 1) ? 1.0f : -1.0f; }
		};
		static int func_type = 0, display_count = 70;
		ImGui::Separator();
		ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
		ImGui::Combo("func", &func_type, "Sin\0Saw\0");
		ImGui::SameLine();
		ImGui::SliderInt("Sample count", &display_count, 1, 400);
		float (*func)(void*, int) = (func_type == 0) ? Funcs::Sin : Funcs::Saw;
		ImGui::PlotLines("Lines", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
		ImGui::PlotHistogram("Histogram", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
		ImGui::Separator();

		// Animate a simple progress bar
		static float progress = 0.0f, progress_dir = 1.0f;
		if (animate)
		{
			progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
			if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
			if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
		}

		// Typically we would use ImVec2(-1.0f,0.0f) or ImVec2(-FLT_MIN,0.0f) to use all available width,
		// or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
		ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::Text("Progress Bar");

		float progress_saturated = IM_CLAMP(progress, 0.0f, 1.0f);
		char buf[32];
		//sprintf(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);
		ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);
		ImGui::TreePop();
	}

	ImGui::End();

	//-----------------------------------
	ImGui::Begin("Log Monitor", nullptr, window_flags);
	ImGui::End();

	//-----------------------------------
	ImGui::Begin("Audio Monitor", nullptr, window_flags);
	//ImGui::Text(std::string("Volume: " + std::to_string(AUDIO.GetUserSettingVolume())).c_str());
	//ImGui::Text(std::string("PlayListSize: " + std::to_string(AUDIO.GetPlayListSize())).c_str());
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
