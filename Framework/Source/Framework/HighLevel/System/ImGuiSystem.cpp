#include "../../Utility/Cpuid.h"
#include "../../Audio/SoundDirector.h"

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

	ImGui_ImplWin32_Init(ApplicationChilled::GetApplication()->g_window->GetWndHandle());
	ImGui_ImplDX11_Init(device, context);

	ImFontConfig config;
	config.MergeMode = true;

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &config, glyphRangesJapanese);

	// Enable docking(available in imgui `docking` branch at the moment)
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// 初期スタイル
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

	style.ScrollbarSize = 10;

	AddLog("INFO: Dear imGui Initialized.");
	AddLog(u8"u8を付けるとUTF-8に対応.");//TODO: C++20以降だとu8が使用不可能
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
	if (!m_enable) return;

	// 共通のWindowフラグ
	ImGuiWindowFlags wflags;
	if (false) wflags = ImGuiWindowFlags_NoCollapse;
	else wflags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

	//大本のモニター
	if (ImGui::Begin("Main", nullptr, wflags | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {}
	ImGui::End();

	//ゲームモニター
	if (ImGui::Begin("Scene", nullptr, wflags | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
	{
		const auto& sceneRT = ApplicationChilled::GetApplication()->GetScreenRT();

		//サイズ調整
		const auto& sizeWd = ImGui::GetWindowSize();
		const auto& sizeRT = float2((float)sceneRT->GetDesc().Width, (float)sceneRT->GetDesc().Height);

		//TODO: バーのサイズがどうなってるか調査
		float sizeBar = 36.0f;
		float rate = (sizeWd.y - sizeBar) / sizeRT.y;

		ImGui::Image((ImTextureID)sceneRT->SRV(), ImVec2(sizeRT.x, sizeRT.y) * rate);
	}
	ImGui::End();

	//高輝度抽出テクスチャ
	if (ImGui::Begin("HeightBright Texture", nullptr, ImGuiWindowFlags_NoCollapse))
	{
		const auto& hbRT = ApplicationChilled::GetApplication()->GetHeightBrightRT();

		//サイズ調整
		const auto& sizeWd = ImGui::GetWindowSize();
		const auto& sizeRT = float2((float)hbRT->GetDesc().Width, (float)hbRT->GetDesc().Height);

		//TODO: バーのサイズがどうなってるか調査
		float sizeBar = 36.0f;
		float rate = (sizeWd.y - sizeBar) / sizeRT.y;

		ImGui::Image((ImTextureID)hbRT->SRV(), ImVec2(sizeRT.x, sizeRT.y) * rate);
	}
	ImGui::End();

	// DemoWindow
	if (m_showDemoMonitor)
		ImGui::ShowDemoWindow();

	// シーンモニターはいつでも表示
	SceneMonitor(wflags);

	// フラグで表示のON/OFF切り替え
	if (m_showEachMonitor)
	{
		LogMonitor(wflags);
		AudioMonitor(wflags);
		ProfilerMonitor(wflags);
	}
}

//-----------------------------------------------------------------------------
// ログの追加
//-----------------------------------------------------------------------------
void ImGuiSystem::AddLog(std::string_view log)
{
	auto time = std::to_string(ApplicationChilled::GetApplication()->g_fpsTimer->GetTotalTime());

	std::string buff;
	buff += log.substr();
	//buff += "[Time: " + time + "]";
	buff += "\n";

	//
	m_logBuffer.append(buff.c_str());
	m_addLog = true;

	//DebugLog(string.c_str());
}
/*void ImGuiSystem::AddLog(std::string_view log, std::source_location sinfo)
{
	auto time = std::to_string(ApplicationChilled::GetApplication()->g_fpsTimer->GetTotalTime());

	std::string buff;
	buff += log.substr();
	//buff += "[Time: " + time + "]";

	//ソースの情報
	const auto& file_name = sinfo.file_name();
	const auto& function_name = sinfo.function_name();
	const auto& line = sinfo.line();
	const auto& column = sinfo.column();

	//
	int nn = 60 - log.size();
	for (int i = 0; i < nn; i++) buff += " ";

	//buff += file_name;
	buff += "/Function: ";
	buff += function_name;
	buff += "/Line: ";
	buff += std::to_string(line);
	buff += "/Column: ";
	buff += std::to_string(column);

	buff += "\n";

	//
	m_logBuffer.append(buff.c_str());
	m_addLog = true;

	//DebugLog(string.c_str());
}*/

//-----------------------------------------------------------------------------
// ゲームのシーン
//-----------------------------------------------------------------------------
void ImGuiSystem::SceneMonitor(ImGuiWindowFlags wflags)
{
	if (!ImGui::Begin("Scene Monitor", nullptr, wflags | ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Save")) {
			ApplicationChilled::GetApplication()->g_gameSystem->Serialize("");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Load")) {

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	//ImGui::Checkbox("ShowEachMonitor", &m_showEachMonitor);
#ifdef _DEBUG
	ImGui::Text(std::string("Debug").c_str());
#else
	ImGui::Text(std::string("Release").c_str());
#endif

	// シーンのパス表示
	ImGui::Text(std::string("Scene: " + ApplicationChilled::GetApplication()->g_gameSystem->g_sceneFilepath).c_str());

	// Actor 作成
	
	// Jsonパス格納
	static char jsonPath[128] = "";
	// Jsonパス格納 : エクスプローラから選択ver
	static std::string string = "Resource/Jsons/";

	ImGui::InputText("Jsonpath", jsonPath, IM_ARRAYSIZE(jsonPath));

	if (ImGui::Button("Create"))
	{
		const auto& json = RES_FAC.GetJsonData(jsonPath);
		if (json.is_null()) {
			AddLog(u8"Actor生成失敗.");
			return;
		}

		const auto& actor = GenerateActor(json["class_name"].string_value());
		if (actor) {
			actor->Deserialize(json);
			actor->Initialize();

			ApplicationChilled::GetApplication()->g_gameSystem->AddActor(actor);

			AddLog(std::string(u8"Actor生成: " + json["class_name"].string_value()).c_str());
		}
	}

	if (ImGui::Button(u8"Jsonパス所得"))
	{
		if (ApplicationChilled::GetApplication()->g_window->OpenFileDialog(string, "Select path.", "json"))
		{
			// 選択したjsonのパスをコピー
			strcpy_s(jsonPath, string.c_str());

			AddLog("読み込み成功");
		}
	}

	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
	ImGui::Text("Actor List");
	ImGui::PopStyleColor();

	// SceneのActor一覧
	static std::weak_ptr<Actor> wpActor;
	for (auto&& actor : ApplicationChilled::GetApplication()->g_gameSystem->GetActorList())
	{
		ImGui::PushID(actor.get());

		// 選択したActor
		bool selected = (actor == wpActor.lock());
		if (ImGui::Selectable(actor->g_name.c_str(), selected))
			wpActor = actor;

		ImGui::PopID();
	}

	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
	ImGui::Text("Detailed data");
	ImGui::PopStyleColor();

	if (wpActor.lock())
	{
		auto& transform = wpActor.lock()->GetTransform();
		float3 pos = transform.GetPosition();
		float3 angle = transform.GetAngle();
		float3 scale = transform.GetScale();

		ImGui::Text(std::string("Select: " + wpActor.lock()->g_name).c_str());

		bool selected = false;
		selected |= ImGui::DragFloat3("Position", &pos.x, 0.1f);
		selected |= ImGui::DragFloat3("Angle", &angle.x, 0.1f);
		selected |= ImGui::DragFloat3("Scale", &scale.x, 0.1f);

		if (selected) {
			transform.SetPosition(pos);
			transform.SetAngle(angle);
			transform.SetScale(scale);
		}

		ImGui::CheckboxFlags("eUntagged", &wpActor.lock()->g_tag, ACTOR_TAG::UNTAGGED);
		ImGui::CheckboxFlags("ePlayer", &wpActor.lock()->g_tag, ACTOR_TAG::PLAYER);
		ImGui::CheckboxFlags("eEnemy", &wpActor.lock()->g_tag, ACTOR_TAG::ENEMY);
		ImGui::CheckboxFlags("eWeapon", &wpActor.lock()->g_tag, ACTOR_TAG::WEPON);
		ImGui::CheckboxFlags("eGround", &wpActor.lock()->g_tag, ACTOR_TAG::GROUND);

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
	for (auto&& camera : ApplicationChilled::GetApplication()->g_gameSystem->g_cameraSystem.GetCameraList())
	{
		ImGui::PushID(camera.get());
		ImGui::Text(camera->g_name.c_str());
		ImGui::PopID();
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

	static float volume = ApplicationChilled::GetApplication()->g_audioDevice->GetMasterVolume();
	if (ImGui::SliderFloat("Main", &volume, 0.0f, 2.0f, "%.2f"))
		ApplicationChilled::GetApplication()->g_audioDevice->SetMasterVolume(volume);

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

	auto& cameraMatrix = RENDERER.Getcb9().Get().m_camera_matrix;
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
		PlotLinesEx("PeakLevels R", ApplicationChilled::GetApplication()->g_audioDevice->g_peakLevels[0]);
		PlotLinesEx("PeakLevels L", ApplicationChilled::GetApplication()->g_audioDevice->g_peakLevels[1]);
		PlotLinesEx("RMSLevels R", ApplicationChilled::GetApplication()->g_audioDevice->g_RMSLevels[0]);
		PlotLinesEx("RMSLevels L", ApplicationChilled::GetApplication()->g_audioDevice->g_RMSLevels[1]);
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
	ImGui::Text(ApplicationChilled::GetApplication()->g_graphicsDevice->GetAdapterName().c_str());

	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
	ImGui::Text("Time");
	ImGui::PopStyleColor();

	auto& fpsTimer = ApplicationChilled::GetApplication()->g_fpsTimer;
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
