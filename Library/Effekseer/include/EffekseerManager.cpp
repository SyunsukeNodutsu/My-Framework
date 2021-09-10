#include "EffekseerManager.h"

void EffekseerManager::Initialize()
{
	//Effekseer作成
	m_pEffekseerRenderer = EffekseerRendererDX11::Renderer::Create(D3D.GetDevice(), D3D.GetDeviceContext(), m_maxDrawSpriteCount);

	// エフェクトのマネージャーの作成
	m_pEffekseerManager = Effekseer::Manager::Create(m_maxDrawSpriteCount);

	// 描画モジュールの設定
	m_pEffekseerManager->SetSpriteRenderer(m_pEffekseerRenderer->CreateSpriteRenderer());
	m_pEffekseerManager->SetRibbonRenderer(m_pEffekseerRenderer->CreateRibbonRenderer());
	m_pEffekseerManager->SetRingRenderer(m_pEffekseerRenderer->CreateRingRenderer());
	m_pEffekseerManager->SetTrackRenderer(m_pEffekseerRenderer->CreateTrackRenderer());
	m_pEffekseerManager->SetModelRenderer(m_pEffekseerRenderer->CreateModelRenderer());

	// テクスチャ、モデル、マテリアルローダーの設定する。
	// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
	m_pEffekseerManager->SetTextureLoader(m_pEffekseerRenderer->CreateTextureLoader());
	m_pEffekseerManager->SetModelLoader(m_pEffekseerRenderer->CreateModelLoader());
	m_pEffekseerManager->SetMaterialLoader(m_pEffekseerRenderer->CreateMaterialLoader());

	//Effekseerはデフォルトが右手座標系なので、左手座標系に修正する
	m_pEffekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

	// 投影行列を設定(CameraComponentの設定を引き継ぎ)
	m_pEffekseerRenderer->SetProjectionMatrix(::Effekseer::Matrix44().PerspectiveFovLH(
		60 * ToRadians, 16 / 9, 0.01f, 5000.0f));

	// カメラ行列を入れておかないと落ちるので、適当な値を代入
	m_pEffekseerRenderer->SetCameraMatrix(
		Effekseer::Matrix44().LookAtLH(Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 0.0f, 1.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

}

void EffekseerManager::Finalize()
{
	m_pEffekseerManager->Destroy();
	m_pEffekseerRenderer->Destroy();
}

void EffekseerManager::Update(float deltaTime)
{
	// マネージャーの更新
	m_pEffekseerManager->Update(deltaTime);

	// エフェクトの描画開始処理を行う。
	m_pEffekseerRenderer->BeginRendering();

	// エフェクトの描画を行う。
	m_pEffekseerManager->Draw();

	// エフェクトの描画終了処理を行う。
	m_pEffekseerRenderer->EndRendering();
}

bool EffekseerManager::Play(const std::u16string& filepath, Math::Vector3 &position)
{
	////再生するエフェクトを検索
	//auto& instance = FindEffect(filepath);
	//if (instance == nullptr) { return false; }

	////指定された座標で再生
	//instance->Play(position);
	//return true;
	return true;
}

std::shared_ptr<EffekseerEffectInstance> EffekseerManager::LoadEffect(const std::u16string& filepath)
{
	/*auto& effect = std::make_shared<EffekseerEffect>();
	bool isLoad = effect->Load(filepath);
	if (isLoad == false)
	{
		return nullptr;
	}
	auto& instance = std::make_shared<EffekseerEffectInstance>();
	instance->Initialize(effect);

	m_instanceMap.emplace(filepath, instance);
	return instance;*/

	return nullptr;
}

std::shared_ptr<EffekseerEffectInstance> EffekseerManager::FindEffect(const std::u16string& filepath)
{
	//すでに読み込まれたエフェクトかチェック
	auto pair = m_instanceMap.find(filepath);
	if (pair == m_instanceMap.end())
	{
		//見つからなかった場合、新規に読み込む
		return LoadEffect(filepath);
	}
	else
	{
		//見つかった場合は使い回す
		return pair->second;
	}
}

void EffekseerManager::StopAll()
{
	for (auto& pair : m_instanceMap)
	{
		pair.second->Stop();
	}
	m_pEffekseerManager->StopAllEffects();
}

void EffekseerManager::SetCameraMatrix(Math::Matrix& matrix)
{
	//m_pEffekseerRenderer->SetCameraMatrix(matrix);
}

bool EffekseerEffect::Load(const std::u16string& filepath)
{
	m_pEffect = Effekseer::Effect::Create(EFFEKSEER.GetManager(), filepath.c_str());
	if (m_pEffect == nullptr)
	{
		assert(0 && "Effekseerファイルのパスが間違っています");
		return false;
	}
	return true;
}

void EffekseerEffectInstance::Initialize(const std::shared_ptr<EffekseerEffect>& effectData)
{
	m_effectData = effectData;
}

void EffekseerEffectInstance::Play(const Math::Vector3& position)
{
	m_handle = EFFEKSEER.GetManager()->Play(m_effectData->Get(), position.x, position.y, position.z);
}

void EffekseerEffectInstance::Move(Math::Vector3& addPosition)
{
	//EFFEKSEER.GetManager()->AddLocation(m_handle, addPosition);
}

void EffekseerEffectInstance::Stop()
{
	EFFEKSEER.GetManager()->StopEffect(m_handle);
}
