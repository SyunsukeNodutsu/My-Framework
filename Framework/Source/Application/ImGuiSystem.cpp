#include "ImGuiSystem.h"
#include "GameSystem.h"
#include "main.h"
#include "../Framework/Utility/Cpuid.h"
#include "../Framework/Audio/SoundDirector.h"

// stlに頼らないimGuiさんまじカッケー
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

static void HelpMarker(const char* desc);

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ImGuiSystem::ImGuiSystem()
	: m_logBuffer()
	, m_enable(true)
	, m_addLog(false)
	, m_createInifile(true)
	, m_showEachMonitor(true)
	, m_showDemoMonitor(false)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void ImGuiSystem::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	if (!device || !context) return;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// ImGuiの外観設定
	ImGui::StyleColorsClassic();

	ImGui_ImplWin32_Init(APP.g_window->GetWndHandle());
	ImGui_ImplDX11_Init(device, context);

	ImFontConfig config;
	config.MergeMode = true;

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &config, glyphRangesJapanese);

	// 初期スタイル
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.2f);

	style.ScrollbarSize = 10;

	AddLog("INFO: Dear imGui Initialized.");
	AddLog(u8"u8を付けるとUTF-8に対応.");

	Cpuid::Research();
	AddLog("INFO: CPUID Researched.");
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void ImGuiSystem::Finalize()
{
	if (!m_createInifile) {
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = NULL;
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

//-----------------------------------------------------------------------------
// 描画開始
//-----------------------------------------------------------------------------
void ImGuiSystem::Begin()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

//-----------------------------------------------------------------------------
// 描画終了
//-----------------------------------------------------------------------------
void ImGuiSystem::End()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//-----------------------------------------------------------------------------
// imGui描画
//-----------------------------------------------------------------------------
void ImGuiSystem::DrawImGui()
{
	if (!m_enable)
		return;

	// 共通のWindowフラグ
	ImGuiWindowFlags wflags = 0;
	/*ImGuiWindowFlags wflags = ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;*/

	// DemoWindow
	if (m_showDemoMonitor)
		ImGui::ShowDemoWindow();

	// シーンモニターはいつでも表示
	SceneMonitor(wflags);

	// フラグで表示のON/OFF切り替え
	if (m_showEachMonitor)
	{
		ShaderDebugMonitor(wflags);
		LogMonitor(wflags);
		AudioMonitor(wflags);
		ProfilerMonitor(wflags);

		// シャドウ
		if (ImGui::Begin("Shadow 00"))
		{
			ImGui::Image(
				(ImTextureID)SHADER.GetShadowMapShader().GetShadowMap(0)->SRV(),
				ImVec2(200, 200)
			);
		}
		ImGui::End();

		if (ImGui::Begin("Shadow 01"))
		{
			ImGui::Image(
				(ImTextureID)SHADER.GetShadowMapShader().GetShadowMap(1)->SRV(),
				ImVec2(200, 200)
			);
		}
		ImGui::End();

		if (ImGui::Begin("Shadow 02"))
		{
			ImGui::Image(
				(ImTextureID)SHADER.GetShadowMapShader().GetShadowMap(2)->SRV(),
				ImVec2(200, 200)
			);
		}
		ImGui::End();
	}
}

//-----------------------------------------------------------------------------
// ゲームのシーン
//-----------------------------------------------------------------------------
void ImGuiSystem::SceneMonitor(ImGuiWindowFlags wflags)
{
	if (!ImGui::Begin("Scene Monitor", nullptr, wflags | ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	ImGui::Checkbox("ShowEachMonitor", &m_showEachMonitor);

	// シーンのパス表示
	ImGui::Text(std::string("Scene: " + APP.g_gameSystem->g_sceneFilepath).c_str());

	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
	ImGui::Text("Actor List");
	ImGui::PopStyleColor();

	// SceneのActor一覧
	ImGui::BeginChild("##wav list", ImVec2(0, 100), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NavFlattened);

	static std::weak_ptr<Actor> wpActor;
	for (auto&& actor : APP.g_gameSystem->GetActorList())
	{
		ImGui::PushID(actor.get());

		// 選択したActor
		bool selected = (actor == wpActor.lock());
		if (ImGui::Selectable(actor->g_name.c_str(), selected))
			wpActor = actor;

		ImGui::PopID();
	}
	ImGui::EndChild();

	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
	ImGui::Text("Detailed data");
	ImGui::PopStyleColor();

	if (wpActor.lock())
	{
		auto transform = wpActor.lock()->GetTransform();
		auto pos = transform.GetPosition();
		auto angle = transform.GetAngle();
		auto scale = transform.GetScale();

		ImGui::Text(std::string("Select: " + wpActor.lock()->g_name).c_str());
		ImGui::DragFloat3("Position", &pos.x, 0.1f);
		ImGui::DragFloat3("Angle", &angle.x, 0.1f);
		ImGui::DragFloat3("Scale", &scale.x, 0.1f);

		ImGui::CheckboxFlags("eUntagged", &wpActor.lock()->g_tag, ACTOR_TAG::eUntagged);
		ImGui::CheckboxFlags("ePlayer", &wpActor.lock()->g_tag, ACTOR_TAG::ePlayer);
		ImGui::CheckboxFlags("eEnemy", &wpActor.lock()->g_tag, ACTOR_TAG::eEnemy);
		ImGui::CheckboxFlags("eWeapon", &wpActor.lock()->g_tag, ACTOR_TAG::eWeapon);
		ImGui::CheckboxFlags("eGround", &wpActor.lock()->g_tag, ACTOR_TAG::eGround);

		if (ImGui::Button("Copy Clipboard TAG"))
		{
			ImGui::SetClipboardText(FormatBuffer("\"Tag\" : %d", wpActor.lock()->g_tag).c_str());
			AddLog(u8"TAGをクリップボードにコピーしました.");
		}

	}
	else ImGui::Text("Select: none");

	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
	ImGui::Text("Camera List");
	ImGui::PopStyleColor();

	// カメラ一覧
	for (auto&& camera : APP.g_gameSystem->g_cameraSystem.GetCameraList())
	{
		ImGui::PushID(camera.get());
		ImGui::Text(camera->g_name.c_str());
		ImGui::PopID();
	}

	ImGui::End();
}

//-----------------------------------------------------------------------------
// シェーダーデバッグ 
//-----------------------------------------------------------------------------
void ImGuiSystem::ShaderDebugMonitor(ImGuiWindowFlags wflags)
{
	if (!ImGui::Begin("ShaderDebug Monitor", nullptr, wflags)) {
		ImGui::End();
		return;
	}

	static bool show_base_color = false;
	static bool show_normal = false;
	static bool show_emissive = false;
	static bool show_metallic_rough = false;

	if (ImGui::BeginTable("split", 2))
	{
		bool darty = false;
		ImGui::TableNextColumn();
		if (ImGui::Checkbox("Base Color", &show_base_color)) {
			RENDERER.Getcb7().Work().g_show_base_color = show_base_color;
			RENDERER.Getcb7().Work().g_show_normal = show_normal;
			RENDERER.Getcb7().Write();
		}// TODO: fix
		ImGui::TableNextColumn();
		if (ImGui::Checkbox("Normal", &show_normal)) {
			RENDERER.Getcb7().Work().g_show_base_color = show_base_color;
			RENDERER.Getcb7().Work().g_show_normal = show_normal;
			RENDERER.Getcb7().Write();
		}
		ImGui::EndTable();
	}

	ImGui::End();
}

//-----------------------------------------------------------------------------
// デバッグログ
//-----------------------------------------------------------------------------
void ImGuiSystem::LogMonitor(ImGuiWindowFlags wflags)
{
	if (!ImGui::Begin("Log Monitor", nullptr, wflags | ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options")) {
			ImGui::Text("todo: fix");
			ImGui::EndMenu();
		}
		if (ImGui::Button("Clear")) {
			m_logBuffer.clear();
		}
		if (ImGui::Button("Copy")) {
			ImGui::SetClipboardText(m_logBuffer.c_str());
			AddLog("INFO: Log copy done.");
		}
		ImGui::EndMenuBar();
	}

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 1, 1));
	ImGui::TextUnformatted(m_logBuffer.begin());
	ImGui::PopStyleColor();

	if (m_addLog)
	{
		ImGui::SetScrollHereY(1.0f);
		m_addLog = false;
	}

	ImGui::End();
}

//-----------------------------------------------------------------------------
// ゲームオーディオ
//-----------------------------------------------------------------------------
void ImGuiSystem::AudioMonitor(ImGuiWindowFlags wflags)
{
	if (!ImGui::Begin("Audio Monitor", nullptr, wflags)) {
		ImGui::End();
		return;
	}

	ImGui::Text("XAudio2 version. (2.9)");

	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
	ImGui::Text("Volume");
	ImGui::PopStyleColor();

	static float volume = APP.g_audioDevice->GetMasterVolume();
	if (ImGui::SliderFloat("Main", &volume, 0.0f, 2.0f, "%.2f"))
		APP.g_audioDevice->SetMasterVolume(volume);

	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
	ImGui::Text("Sound List");
	ImGui::PopStyleColor();

	ImGui::Text(std::string("List Size: " + std::to_string(SOUND_DIRECTOR.GetSoundListSize())).c_str());

	ImGui::BeginChild("##wav list", ImVec2(0, 60), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NavFlattened);
	
	static std::weak_ptr<SoundWork> wpSound;
	for (auto&& sound : SOUND_DIRECTOR.GetSoundList())
	{
		ImGui::PushID(sound.get());

		// 選択したObject
		bool selected = (sound == wpSound.lock());
		if (ImGui::Selectable(sound->GetName().c_str(), selected))
			wpSound = sound;

		ImGui::PopID();
	}
	ImGui::EndChild();

	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
	ImGui::Text("Detailed data");
	ImGui::PopStyleColor();

	if (wpSound.lock())
	{
		ImGui::Text(std::string("Select: " + wpSound.lock()->GetName()).c_str());
		if(wpSound.lock()->Is3D()) ImGui::Text("Is3D?: TRUE");
		else ImGui::Text("Is3D?: FALSE");

		auto val = wpSound.lock()->GetVolume();
		if (ImGui::SliderFloat("Volume", &val, 0.0f, 2.0f, "%.2f")) {
			wpSound.lock()->SetVolume(val);
		}

		auto pan = wpSound.lock()->GetPan();
		if (ImGui::SliderFloat("Pan", &pan, -1.0f, 1.0f, "%.2f")) {
			wpSound.lock()->SetPan(pan);
		}
	}
	else ImGui::Text("Select: none");

	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
	ImGui::Text("Listener");
	ImGui::PopStyleColor();

	auto& cameraMatrix = RENDERER.Getcb9().Get().m_camera_matrix;// APP.g_gameSystem->GetCamera()->GetCameraMatrix();
	float3 pos = cameraMatrix.Translation();
	float3 dir = cameraMatrix.Backward();

	ImGui::DragFloat3("listener pos", &pos.x);
	ImGui::DragFloat3("listener dir", &dir.x);

	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
	ImGui::Text("Volume Meter (Bug)");
	ImGui::PopStyleColor();

	static bool showMeter = true;
	HelpMarker(u8"PlotLinesの継続的な表示は\nFPSの低下につながります.");
	ImGui::Checkbox("Show Meter", &showMeter);
	if (showMeter)
	{
		PlotLinesEx("PeakLevels R", APP.g_audioDevice->g_peakLevels[0]);
		PlotLinesEx("PeakLevels L", APP.g_audioDevice->g_peakLevels[1]);
		PlotLinesEx("RMSLevels R", APP.g_audioDevice->g_RMSLevels[0]);
		PlotLinesEx("RMSLevels L", APP.g_audioDevice->g_RMSLevels[1]);
	}

	ImGui::End();
}

//-----------------------------------------------------------------------------
// プロファイラー分析
//-----------------------------------------------------------------------------
void ImGuiSystem::ProfilerMonitor(ImGuiWindowFlags wflags)
{
	ImGui::Begin("Profile Monitor", nullptr, wflags);

	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
	ImGui::Text("Vendor");
	ImGui::PopStyleColor();

	ImGui::Text(Cpuid::m_brand.c_str());
	ImGui::Text(APP.g_graphicsDevice->GetAdapterName().c_str());

	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
	ImGui::Text("Time");
	ImGui::PopStyleColor();

	auto& fpsTimer = APP.g_fpsTimer;
	ImGui::Text(std::string("Fps: " + std::to_string(fpsTimer->GetFPS())).c_str());
	ImGui::Text(std::string("DeltaTime: " + std::to_string(fpsTimer->GetDeltaTime())).c_str());
	ImGui::Text(std::string("TotalTime: " + std::to_string(fpsTimer->GetTotalTime())).c_str());
	ImGui::Text(std::string("TimeScale: " + std::to_string(fpsTimer->GetTimeScale())).c_str());

	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
	ImGui::Text("Sliders Edit");
	ImGui::PopStyleColor();

	static float timeScale = fpsTimer->GetTimeScale();
	if (ImGui::SliderFloat("TimeScale", &timeScale, 0, 5, "%.2f"))
		fpsTimer->SetTimeScale(timeScale);

	ImGui::End();
}

//-----------------------------------------------------------------------------
// imGuiのPlotLinesのラップ関数
//-----------------------------------------------------------------------------
void ImGuiSystem::PlotLinesEx(const std::string& string, float val)
{
	static float	values[90] = {};
	static int		values_offset = 0;
	static double	refresh_time = 0.0;
	if (refresh_time == 0.0)
		refresh_time = ImGui::GetTime();

	// 60Hzの固定レートでデモ用データを作成
	while (refresh_time < ImGui::GetTime())
	{
		values[values_offset] = val;
		values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
		refresh_time += 1.0f / 60.0f;
	}

	// プロットはオーバーレイテキストを表示できる
	// (この例では平均値を表示します)
	{
		char overlay[32];
		sprintf_s(overlay, "avg %f", val);
		ImGui::PlotLines(string.c_str(), values, IM_ARRAYSIZE(values), values_offset, overlay, 0.0f, 1.0f, ImVec2(0, 40.0f));
	}
}

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}
