#include "Audio.h"

//=============================================================================
//
// SoundEffect
//
//=============================================================================

//-----------------------------------------------------------------------------
// 音データの読み込み
//-----------------------------------------------------------------------------
bool SoundEffect::Load(const std::string& filepath)
{
	auto engine = AUDIO.GetAudioEngine().get();
	if (engine == nullptr)
		return false;

	try
	{
		std::wstring wfilepath = sjis_to_wide(filepath);
		m_soundEffect = std::make_unique<DirectX::SoundEffect>(engine, wfilepath.c_str());
	}
	catch (...)
	{
		assert(0 && "エラー：音源ファイルが見つかりません.");
		return false;
	}
	return true;
}



//=============================================================================
//
// SoundInstance
//
//=============================================================================

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void SoundInstance::Initialize(const std::shared_ptr<SoundEffect>& soundEffect)
{
	if (done)
		return;
	if (soundEffect == nullptr)
		return;

	DirectX::SOUND_EFFECT_INSTANCE_FLAGS flags = DirectX::SoundEffectInstance_Default;
	m_instance = (soundEffect->CreateInstance(flags));
	m_soundData = soundEffect;

	done = true;
}

//-----------------------------------------------------------------------------
// 再生
//-----------------------------------------------------------------------------
void SoundInstance::Play(bool loop)
{
	if (m_instance == nullptr)
		return;

	m_instance->Stop();
	m_instance->Play(loop);

	AUDIO.AddPlayList(shared_from_this()); // 再生時リストに登録
}

//-----------------------------------------------------------------------------
// 音量設定
//-----------------------------------------------------------------------------
void SoundInstance::SetVolume(float vol)
{
	if (m_instance == nullptr)
		return;

	m_instance->SetVolume(vol);
}

//-----------------------------------------------------------------------------
// 再生しているか返す
//-----------------------------------------------------------------------------
bool SoundInstance::IsPlaying()
{
	if (m_instance == nullptr)
		return false;

	return (m_instance->GetState() == DirectX::SoundState::PLAYING);
}



//=============================================================================
//
// SoundInstance3D
//
//=============================================================================

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void SoundInstance3D::Initialize(const std::shared_ptr<SoundEffect>& soundEffect)
{
	if (done)
		return;
	if (m_instance == nullptr)
		return;

	DirectX::SOUND_EFFECT_INSTANCE_FLAGS flags = DirectX::SoundEffectInstance_Default |
		DirectX::SoundEffectInstance_Use3D | DirectX::SoundEffectInstance_ReverbUseFilters;
	m_instance = (soundEffect->CreateInstance(flags));
	m_soundData = soundEffect;

	// エミッター設定
	m_emitter.InnerRadius		= 2.0f;
	m_emitter.InnerRadiusAngle	= X3DAUDIO_PI / 4.0f;
	m_emitter.pVolumeCurve		= NULL;
	m_emitter.CurveDistanceScaler = 10.0f;// ワールド定義の単位 ※修正.定数に
	m_emitter.DopplerScaler		= 60.0f;

	done = true;
}

//-----------------------------------------------------------------------------
// 3D再生
//-----------------------------------------------------------------------------
void SoundInstance3D::Play(bool loop)
{
	if (m_instance == nullptr)
		return;

	SoundInstance::Play(loop);
}

//-----------------------------------------------------------------------------
// 座標設定
//-----------------------------------------------------------------------------
void SoundInstance3D::SetPos(const float3& position)
{
	if (m_instance == nullptr)
		return;

	m_emitter.SetPosition(position);
	m_instance->Apply3D(AUDIO.GetListener(), m_emitter, false);
}

//-----------------------------------------------------------------------------
// 減衰率設定
//-----------------------------------------------------------------------------
void SoundInstance3D::SetCurveDistanceScaler(float val)
{
	if (m_instance == nullptr)
		return;

	m_emitter.CurveDistanceScaler = val;
	m_instance->Apply3D(AUDIO.GetListener(), m_emitter, false);
}



//=============================================================================
//
// AudioManager
//
//=============================================================================

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
AudioManager::AudioManager()
	: m_audioEngine(nullptr)
	, m_listener()
	, m_playList()
	, m_userVolume(1.0f)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void AudioManager::Initialize()
{
	// AudioEngine初期化
	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_ReverbUseFilters;
		/*DirectX::AudioEngine_EnvironmentalReverb |
		DirectX::AudioEngine_ReverbUseFilters;*/

	m_audioEngine = std::make_unique<DirectX::AudioEngine>(eflags);
	m_audioEngine->SetReverb(DirectX::Reverb_Off);
	m_audioEngine->SetMasterVolume(m_userVolume);

	m_listener.OrientFront = float3::Backward;
}

//-----------------------------------------------------------------------------
// 解放
//-----------------------------------------------------------------------------
void AudioManager::Finalize()
{
	StopAllSound();

	//m_audioEngine->TrimVoicePool();
	m_audioEngine = nullptr;
	m_playList.clear();
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void AudioManager::Update(const float3& position, const float3& dir)
{
	// エンジンの更新 ※無意味
	if (m_audioEngine != nullptr)
		m_audioEngine->Update();

	// リスナーの更新
	m_listener.SetPosition(position);
	m_listener.OrientFront = dir;

	// 再生中じゃないインスタンスは終了したと判断してリストから削除
	for (auto iter = m_playList.begin(); iter != m_playList.end();)
	{
		if (!iter->second->IsPlaying())
		{
			iter = m_playList.erase(iter);
			continue;
		}
		++iter;
	}
}

//-----------------------------------------------------------------------------
// 再生
//-----------------------------------------------------------------------------
bool AudioManager::Play(const std::string& filepath, bool loop)
{
	if (m_audioEngine == nullptr)
		return false;

	// initialize sound effect
	auto soundData = std::make_shared<SoundEffect>();
	if (!soundData->Load(filepath))
		return false;

	// initialize sound instance
	std::shared_ptr<SoundInstance> instance = std::make_shared<SoundInstance>();
	if (instance == nullptr)
		return false;

	// done
	instance->Initialize(soundData);
	instance->Play(loop);

	return true;
}

//-----------------------------------------------------------------------------
// 3D再生
//-----------------------------------------------------------------------------
bool AudioManager::Play3D(const std::string& filepath, const float3& position, bool loop)
{
	if (m_audioEngine == nullptr)
		return false;

	// initialize sound effect
	auto soundData = std::make_shared<SoundEffect>();
	if (!soundData->Load(filepath))
		return false;

	// initialize sound instance
	std::shared_ptr<SoundInstance3D> instance = std::make_shared<SoundInstance3D>();
	if (instance == nullptr)
		return false;

	// done
	instance->Initialize(soundData);
	instance->Play(loop);
	instance->SetPos(position);

	return true;
}

//-----------------------------------------------------------------------------
// 再生リスト停止
//-----------------------------------------------------------------------------
void AudioManager::StopAllSound()
{
	if (m_audioEngine == nullptr)
		return;

	for (auto& sound : m_playList)
		sound.second->Stop();
}
