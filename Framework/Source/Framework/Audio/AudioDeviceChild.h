//-----------------------------------------------------------------------------
// File: AudioDeviceChild.h
//
// いい加減シングルトンはやばいので
// オーディオ処理を行うクラスに継承
//-----------------------------------------------------------------------------
#pragma once
#include "Audio.h"

// todo
class AudioDevice {};

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

/*
* AudioDevice
* - AudioDeviceChild
*   - SoundInstance
*   - SoundInstance3D
*/
