#include "AudioDevice.h"

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool AudioDevice::Initialize(XAUDIO2_PROCESSOR processor)
{
    if (done) return false;

    // xAudio2 作成
    UINT32 flags = 0;
    HRESULT hr = XAudio2Create(&g_xAudio2, flags, processor);
    if (!g_xAudio2) {
        DebugLog("[WARNING]Failed to inialize audio device.\n");
        return false;
    }

    // mastering voice 作成
    hr = g_xAudio2->CreateMasteringVoice(&g_masteringVoice);
    if (!g_masteringVoice) {
        DebugLog("[WARNING]Failed to create mastering voice.\n");
        return false;
    }

    done = true;

    return true;
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
