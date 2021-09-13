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
	, m_upSpriteBatch(nullptr)
	, m_upSpriteFont(nullptr)
	, m_spObjectList()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void GameSystem::Initialize()
{
	// SpriteFont作成
	m_upSpriteBatch = std::make_unique<DirectX::SpriteBatch>(D3D.GetDeviceContext());
	m_upSpriteFont = std::make_unique<DirectX::SpriteFont>(D3D.GetDevice(), L"Resource/Font/myfile.spritefont");

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

	//--------------------------------------------------
	// Sprite font
	//--------------------------------------------------

	// 記憶用のステートを取得し SpriteBatch Begin() 呼び出し
	auto saveState = D3D.GetRenderer().GetSaveState();
	m_upSpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred, saveState.BS.Get(), saveState.SS.Get(), saveState.DS.Get(), saveState.RS.Get());

	auto fps = m_fpsTimer.GetFPS();
	auto totalSecond = m_fpsTimer.GetTotalTime();

	DrawSpriteString(Cpuid::m_brand, float2(0, 0));
	DrawSpriteString(wide_to_sjis(D3D.GetAdapterName()), float2(0, 30));
	DrawSpriteString("FPS: " + std::to_string(fps), float2(0, 60));
	DrawSpriteString("totalSecond: " + std::to_string(totalSecond), float2(0, 90));
	DrawSpriteString("deltaTime: " + std::to_string(deltaTime), float2(0, 120));

	RECT clientRect = APP.g_window.GetWinInfo().rcClient;
	DrawSpriteString("width: " + std::to_string(clientRect.right - clientRect.left), float2(0, 150));
	DrawSpriteString("height: " + std::to_string(clientRect.bottom - clientRect.top), float2(0, 180));

	auto m_pos = RAW_INPUT.GetMouse()->GetMousePos();
	DrawSpriteString("pos.x: " + std::to_string(m_pos.x), float2(0, 210));
	DrawSpriteString("pos.y: " + std::to_string(m_pos.y), float2(0, 240));

	auto volume = AUDIO.GetUserSettingVolume();
	DrawSpriteString("volume: " + std::to_string(volume), float2(0, 270));

	auto cameraPos = D3D.GetRenderer().GetCameraPos();
	DrawSpriteString("cameraPos: x->" + std::to_string(cameraPos.x) + "y->" + std::to_string(cameraPos.y) + "->" + std::to_string(cameraPos.z), float2(0, 300));

	m_upSpriteBatch->End();
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

//-----------------------------------------------------------------------------
// SpriteFontによる文字列描画
//-----------------------------------------------------------------------------
void GameSystem::DrawSpriteString(std::string string, float2 position, float scaling, cfloat4x4 color)
{
	if ((m_upSpriteFont == nullptr) || (m_upSpriteBatch == nullptr))
		return;

	m_upSpriteFont->DrawString(m_upSpriteBatch.get(), string.c_str(), position, color, 0, float2(0, 0), scaling);
}
