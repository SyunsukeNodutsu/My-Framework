#include "AudioDevice.h"
#include "SoundDirector.h"
#include "../../Application/main.h"

// サウンドコーン
static const X3DAUDIO_CONE Listener_DirectionalCone = { X3DAUDIO_PI * 5.0f / 6.0f, X3DAUDIO_PI * 11.0f / 6.0f, 1.0f, 0.75f, 0.0f, 0.25f, 0.708f, 1.0f };

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
AudioDevice::AudioDevice()
    : g_xAudio2(nullptr)
    , g_pMasteringVoice(nullptr)
    , g_pSubmixVoice(nullptr)
    , g_x3DAudioInstance()
    , g_pReverbEffect(nullptr)
    , g_peakLevels()
    , g_RMSLevels()
    , done(false)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool AudioDevice::Initialize()
{
    if (done) return false;

    // IXAudio2作成
    // DebugConfigurationは設定なし
    UINT32 flags = 0;
    HRESULT hr = XAudio2Create(&g_xAudio2, flags);
    if (!g_xAudio2) {
        APP.g_imGuiSystem->AddLog("ERROR: Failed to inialize audio device.");
        return false;
    }

    // IXAudio2MasteringVoice作成
    hr = g_xAudio2->CreateMasteringVoice(&g_pMasteringVoice);
    if (!g_pMasteringVoice) {
        APP.g_imGuiSystem->AddLog("ERROR: Failed to create mastering voice.");
        return false;
    }

    // スピーカー構成の確認
    if (FAILED(g_pMasteringVoice->GetChannelMask(&g_channelMask))) {
        APP.g_imGuiSystem->AddLog("ERROR: Failed to get speaker configuration.");
        return false;
    }

    // デバイスの詳細を確認
    XAUDIO2_VOICE_DETAILS details;
    g_pMasteringVoice->GetVoiceDetails(&details);
    g_inputSampleRate = details.InputSampleRate;
    g_inputChannels = details.InputChannels;

    if (details.InputChannels > OUTPUTCHANNELS) {
        APP.g_imGuiSystem->AddLog("ERROR: We have exceeded the number of channels we support.");
        return false;
    }

    // 3DAuido初期化
    Initialize3D();

    // ボリュームメータAPO作成
    InitializeVolumeMeterAPO();

    APP.g_imGuiSystem->AddLog("INFO: AudioDevice Initialized.");

    return done = true;
}

//-----------------------------------------------------------------------------
// 3DAudio初期化
//-----------------------------------------------------------------------------
bool AudioDevice::Initialize3D()
{
    // ReverbEffectの作成
    if (FAILED(XAudio2CreateReverb(&g_pReverbEffect))) {
        APP.g_imGuiSystem->AddLog("ERROR: Failed to create reverb.");
        return false;
    }

    // SubmixVoiceの作成
    XAUDIO2_EFFECT_DESCRIPTOR effects[] = { { g_pReverbEffect, TRUE, g_inputChannels } };
    XAUDIO2_EFFECT_CHAIN effectChain = { 1, effects };

    if (FAILED(g_xAudio2->CreateSubmixVoice(&g_pSubmixVoice,
        g_inputChannels, g_inputSampleRate, 0, 0, nullptr, &effectChain))) {
        APP.g_imGuiSystem->AddLog("ERROR: Failed to create submix voice.");
        return false;
    }

    // デフォルトのFXパラメータ設定
    XAUDIO2FX_REVERB_I3DL2_PARAMETERS param = XAUDIO2FX_I3DL2_PRESET_GENERIC;
    if (!SetReverb(param)) {
        APP.g_imGuiSystem->AddLog("ERROR: Failed to set effect parameters.");
        return false;
    }

    // X3DAudio初期化 Speed of sound ... 20度の乾燥した空気
    X3DAudioInitialize(g_channelMask, X3DAUDIO_SPEED_OF_SOUND, g_x3DAudioInstance);

    //--------------------------------------------------
    // リスナー初期化
    //--------------------------------------------------
    
    m_listener.Position.x     = 
    m_listener.Position.y     = 
    m_listener.Position.z     = 0.f;

    m_listener.OrientFront.x  = 
    m_listener.OrientFront.y  = 
    m_listener.OrientTop.x    = 
    m_listener.OrientTop.z    = 0.f;

    m_listener.OrientFront.z  = 
    m_listener.OrientTop.y    = 1.0f;

    m_listener.pCone = (X3DAUDIO_CONE*)&Listener_DirectionalCone;

    return true;
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void AudioDevice::Finalize()
{
    if (!done) return;

    SOUND_DIRECTOR.Finalize();

    if (g_pSubmixVoice) {
        g_pSubmixVoice->DestroyVoice();
        g_pSubmixVoice = nullptr;
    }
    if (g_pMasteringVoice) {
        g_pMasteringVoice->DestroyVoice();
        g_pMasteringVoice = nullptr;
    }
    if (g_xAudio2) {
        g_xAudio2->StopEngine();
        g_xAudio2->Release();
        g_xAudio2 = nullptr;
    }

    if (g_pReverbEffect) {
        g_pReverbEffect->Release();
        g_pReverbEffect = nullptr;
    }
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void AudioDevice::Update(const mfloat4x4& listener)
{
    if (!done) return;
    if (!g_xAudio2) return;
    if (!g_pMasteringVoice) return;

    {
        // 座標
        float3 pos = listener.Translation();
        m_listener.Position.x = pos.x;
        m_listener.Position.y = pos.y;
        m_listener.Position.z = pos.z;

        // 方向
        float3 front = listener.Backward();
        m_listener.OrientFront.x = front.x;
        m_listener.OrientFront.y = front.y;
        m_listener.OrientFront.z = front.z;

        float3 top = listener.Up();
        m_listener.OrientTop.x = top.x;
        m_listener.OrientTop.y = top.y;
        m_listener.OrientTop.z = top.z;
    }

    SOUND_DIRECTOR.Update();

    UpdateVolumeMeter();
}

//-----------------------------------------------------------------------------
// マスター音量を設定
//-----------------------------------------------------------------------------
void AudioDevice::SetMasterVolume(float value)
{
    if (!done) return;
    if (!g_xAudio2) return;
    if (!g_pMasteringVoice) return;

    g_pMasteringVoice->SetVolume(value);
}

//-----------------------------------------------------------------------------
// マスター音量を返す
//-----------------------------------------------------------------------------
float AudioDevice::GetMasterVolume() const
{
    if (!done) return FLT_MAX;
    if (!g_xAudio2) return FLT_MAX;
    if (!g_pMasteringVoice) return FLT_MAX;

    float ret = 0;
    g_pMasteringVoice->GetVolume(&ret);
    return ret;
}

//-----------------------------------------------------------------------------
// リバーブを設定する
//-----------------------------------------------------------------------------
bool AudioDevice::SetReverb(XAUDIO2FX_REVERB_I3DL2_PARAMETERS rparam)
{
    if (!g_xAudio2) return false;
    if (!g_pSubmixVoice) return false;

    XAUDIO2FX_REVERB_PARAMETERS native = {};
    ReverbConvertI3DL2ToNative(&rparam, &native);
    HRESULT hr = g_pSubmixVoice->SetEffectParameters(0, &native, sizeof(native));

    return SUCCEEDED(hr);
}

//-----------------------------------------------------------------------------
// ボリュームメータ(APO)の作成
//-----------------------------------------------------------------------------
bool AudioDevice::InitializeVolumeMeterAPO()
{
    if (g_xAudio2 == nullptr) return false;
    if (g_pMasteringVoice == nullptr) return false;

    // VolumeMeter(APO)作成
    IUnknown* pVolumeMeterAPO = nullptr;
    if (FAILED(XAudio2CreateVolumeMeter(&pVolumeMeterAPO))) {
        APP.g_imGuiSystem->AddLog("ERROR: Failed to create VolumeMeter(APO).");
        return false;
    }

    // EFFECT_DESCRIPTOR の作成
    XAUDIO2_EFFECT_DESCRIPTOR descriptor = {};
    descriptor.InitialState     = true;
    descriptor.OutputChannels   = g_inputChannels;
    descriptor.pEffect          = pVolumeMeterAPO;

    // EFFECT_CHAIN の作成
    XAUDIO2_EFFECT_CHAIN chain = {};
    chain.EffectCount           = 1;
    chain.pEffectDescriptors    = &descriptor;
    if (FAILED(g_pMasteringVoice->SetEffectChain(&chain))) {
        APP.g_imGuiSystem->AddLog("ERROR: Failed to create EFFECT_CHAIN.");
        return false;
    }

    pVolumeMeterAPO->Release();

    return true;
}

//-----------------------------------------------------------------------------
// ボリュームメータ更新
//-----------------------------------------------------------------------------
void AudioDevice::UpdateVolumeMeter()
{
    if (g_xAudio2 == nullptr) return;
    if (g_pMasteringVoice == nullptr) return;

    g_peakLevels.fill(0);
    g_RMSLevels.fill(0);

    // 受信用構造体 設定
    XAUDIO2FX_VOLUMEMETER_LEVELS Levels = {};
    Levels.pPeakLevels  = &g_peakLevels[0];
    Levels.pRMSLevels   = &g_RMSLevels[0];
    Levels.ChannelCount = g_inputChannels;

    // パラメータ受信
    if (FAILED(g_pMasteringVoice->GetEffectParameters(0, &Levels, sizeof(Levels)))) {
        APP.g_imGuiSystem->AddLog("ERROR: Failed to set EffectParameters.");
    }
}
