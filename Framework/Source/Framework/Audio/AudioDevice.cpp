#include "AudioDevice.h"

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool AudioDevice::Initialize(XAUDIO2_PROCESSOR processor)
{
    if (done) return false;

    // xAudio2 作成
    UINT32 flags = 0;
    HRESULT hr = XAudio2Create(&g_xAudio2, flags);
    if (!g_xAudio2) {
        DebugLog("[WARNING]Failed to inialize audio device.\n");
        return false;
    }

    /*
    XAUDIO2_DEBUG_CONFIGURATION debug = { 0 };
    debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
    debug.BreakMask = XAUDIO2_LOG_ERRORS;
    g_xAudio2->SetDebugConfiguration(&debug, 0);
    */

    // mastering voice 作成
    hr = g_xAudio2->CreateMasteringVoice(&g_masteringVoice);
    if (!g_masteringVoice) {
        DebugLog("[WARNING]Failed to create mastering voice.\n");
        return false;
    }

    InitializeVolumeMeterAPO();

    return done = true;
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void AudioDevice::Finalize()
{
    DebugLog("Release AudioDevice.\n");
    g_masteringVoice->DestroyVoice();
    g_xAudio2->Release();
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
void AudioDevice::InitializeVolumeMeterAPO()
{
    HRESULT result = S_OK;

    // ボリュームメータ(APO)作成
    IUnknown* pVolumeMeterAPO = nullptr;
    result = XAudio2CreateVolumeMeter(&pVolumeMeterAPO);

    // EFFECT_DESCRIPTOR の作成
    XAUDIO2_EFFECT_DESCRIPTOR descriptor = {};
    descriptor.InitialState = true;
    descriptor.OutputChannels = 2;
    descriptor.pEffect = pVolumeMeterAPO;

    // EFFECT_CHAIN の作成
    XAUDIO2_EFFECT_CHAIN chain = {};
    chain.EffectCount = 1;
    chain.pEffectDescriptors = &descriptor;
    result = g_masteringVoice->SetEffectChain(&chain);

    // APO解放
    pVolumeMeterAPO->Release();
}

//-----------------------------------------------------------------------------
// ボリュームメータ更新
//-----------------------------------------------------------------------------
void AudioDevice::UpdateVolumeMeter()
{
    if (g_masteringVoice == nullptr)
        return;

    // 受信用構造体 設定
    XAUDIO2FX_VOLUMEMETER_LEVELS Levels = {};
    Levels.pPeakLevels = g_peakLevels;
    Levels.pRMSLevels = g_RMSLevels;
    Levels.ChannelCount = 2;

    // パラメータ受信
    auto result = g_masteringVoice->GetEffectParameters(0, &Levels, sizeof(Levels));
}
