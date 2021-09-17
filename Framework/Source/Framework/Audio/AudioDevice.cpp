#include "AudioDevice.h"
#include "../../Application/ImGuiSystem.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
AudioDevice::AudioDevice()
    : g_xAudio2(nullptr)
    , g_pMasteringVoice(nullptr)
    , g_peakLevels()
    , g_RMSLevels()
    , done(false)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool AudioDevice::Initialize(XAUDIO2_PROCESSOR processor)
{
    if (done) return false;

    // IXAudio2作成
    UINT32 flags = 0;
    HRESULT hr = XAudio2Create(&g_xAudio2, flags);
    if (!g_xAudio2) {
        IMGUISYSTEM.AddLog("ERROR: Failed to inialize audio device.");
        return false;
    }

#if _DEBUG
    XAUDIO2_DEBUG_CONFIGURATION debug = { 0 };
    debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
    debug.BreakMask = XAUDIO2_LOG_ERRORS;
    g_xAudio2->SetDebugConfiguration(&debug, 0);
#endif // _DEBUG

    // IXAudio2MasteringVoice作成
    hr = g_xAudio2->CreateMasteringVoice(&g_pMasteringVoice);
    if (!g_pMasteringVoice) {
        IMGUISYSTEM.AddLog("ERROR: Failed to create mastering voice.");
        return false;
    }

    InitializeVolumeMeterAPO();

    IMGUISYSTEM.AddLog("INFO: AudioDevice Initialized.");

    return done = true;
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void AudioDevice::Finalize()
{
    if (g_pMasteringVoice) {
        g_pMasteringVoice->DestroyVoice();
        g_pMasteringVoice = nullptr;
    }
    if (g_xAudio2) {
        g_xAudio2->Release();
        g_xAudio2 = nullptr;
    }
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void AudioDevice::Update()
{
    // TODO: ADD 3D Audio

    UpdateVolumeMeter();
}

//-----------------------------------------------------------------------------
// ボリュームメータ(APO)の作成
//-----------------------------------------------------------------------------
bool AudioDevice::InitializeVolumeMeterAPO()
{
    // VolumeMeter(APO)作成
    IUnknown* pVolumeMeterAPO = nullptr;
    if (FAILED(XAudio2CreateVolumeMeter(&pVolumeMeterAPO))) {
        IMGUISYSTEM.AddLog("ERROR: Failed to create VolumeMeter(APO).");
        return false;
    }

    // EFFECT_DESCRIPTOR の作成
    XAUDIO2_EFFECT_DESCRIPTOR descriptor = {};
    descriptor.InitialState     = true;
    descriptor.OutputChannels   = 2;
    descriptor.pEffect          = pVolumeMeterAPO;

    // EFFECT_CHAIN の作成
    XAUDIO2_EFFECT_CHAIN chain = {};
    chain.EffectCount           = 1;
    chain.pEffectDescriptors    = &descriptor;
    if (FAILED(g_pMasteringVoice->SetEffectChain(&chain))) {
        IMGUISYSTEM.AddLog("ERROR: Failed to create EFFECT_CHAIN.");
        return false;
    }

    pVolumeMeterAPO->Release();
    IMGUISYSTEM.AddLog("INFO: Initialized VolumeMeterAPO.");

    return true;
}

//-----------------------------------------------------------------------------
// ボリュームメータ更新
//-----------------------------------------------------------------------------
void AudioDevice::UpdateVolumeMeter()
{
    if (g_pMasteringVoice == nullptr)
        return;

    // 受信用構造体 設定
    constexpr int channels = 2;// 左右
    XAUDIO2FX_VOLUMEMETER_LEVELS Levels = {};
    Levels.pPeakLevels  = g_peakLevels;
    Levels.pRMSLevels   = g_RMSLevels;
    Levels.ChannelCount = channels;

    // パラメータ受信
    if (FAILED(g_pMasteringVoice->GetEffectParameters(0, &Levels, sizeof(Levels)))) {
        DebugLog("ERROR: EffectParameters失敗.");
    }
}
