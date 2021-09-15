//-----------------------------------------------------------------------------
// File: AudioDevice.h
//
// XAudio2 MasteringVoice
//-----------------------------------------------------------------------------
#pragma once

// リトルエンディアン (オーディオファイルの解析用)
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT  ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

// XAudio2のエンジン部分管理
class AudioDevice
{
public:

    // @brief 初期化
    // @return 成功...true
    bool Initialize(XAUDIO2_PROCESSOR processor = XAUDIO2_DEFAULT_PROCESSOR);

    // @brief 終了
    void Finalize();

    // @brief マスター音量を設定
    void SetMasterVolume(float value) {
        g_masteringVoice->SetVolume(value);
    }

    // @brief マスター音量を返す
    // @return マスター音量
    float GetMasterVolume() const {
        float ret = 0.f; g_masteringVoice->GetVolume(&ret); return ret;
    }

    IXAudio2* g_xAudio2 = 0;
    IXAudio2MasteringVoice* g_masteringVoice = 0;

private:

    // 初期化完了？
    bool done = false;

};
