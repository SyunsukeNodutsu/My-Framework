//-----------------------------------------------------------------------------
// File: AudioDeviceChild.h
//
// 継承先に単一のAudioDeviceへの参照を許可します
//-----------------------------------------------------------------------------
#pragma once
#include "AudioDevice.h"

// オーディオ管理の子クラス
class AudioDeviceChild
{
public:

    // @brief オーディオデバイスの設定
    // @param device 設定するデバイスソース
    static void SetAudioDevice(AudioDevice* device);

protected:

    // 単一のデバイスを参照
    static AudioDevice* g_audioDevice;

};
