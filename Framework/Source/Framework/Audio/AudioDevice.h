//-----------------------------------------------------------------------------
// File: AudioDevice.h
//
// XAudio2 MasteringVoice
// TODO: APOを別クラスに移動したほうがいいかも
//-----------------------------------------------------------------------------
#pragma once
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>
#pragma comment(lib,"xaudio2.lib")

// XAudio2のエンジン部分管理
class AudioDevice
{
public:

    // @brief コンストラクタ
    AudioDevice();

    // @brief 初期化
    // @return 成功...true
    bool Initialize(XAUDIO2_PROCESSOR processor = XAUDIO2_DEFAULT_PROCESSOR);

    // @brief 終了
    void Finalize();

    // @brief 更新 TODO: 3Dサウンド処理実装予定
    void Update();

    //--------------------------------------------------
    // 取得・設定
    //--------------------------------------------------

    // @brief マスター音量を設定
    void SetMasterVolume(float value) {
        g_pMasteringVoice->SetVolume(value);
    }

    // @brief マスター音量を返す
    // @return マスター音量
    float GetMasterVolume() const {
        float ret = 0; g_pMasteringVoice->GetVolume(&ret); return ret;
    }

public:

    // デバイス マスターボイス
    IXAudio2* g_xAudio2 = nullptr;
    IXAudio2MasteringVoice* g_pMasteringVoice = nullptr;

    // ボリュームメータ
    float g_peakLevels[2];	// ピークメータ ※瞬間最大値
    float g_RMSLevels[2];	// RMSメータ ※平均値

private:

    // 初期化完了？
    bool done = false;

private:

    // @brief ボリュームメータ(APO)の作成
    // @return 成功...true
    bool InitializeVolumeMeterAPO();

    // @brief ボリュームメータ更新
    void UpdateVolumeMeter();

};
