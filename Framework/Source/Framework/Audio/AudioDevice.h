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

#define INPUTCHANNELS 1 // ソースチャンネルの数
#define OUTPUTCHANNELS 8 // このサンプルでサポートされるデスティネーションチャネルの最大数

enum REVERB_PRESET {};

// XAudio2のエンジン部分管理
class AudioDevice
{
public:

    // @brief コンストラクタ
    AudioDevice();

    // @brief 初期化
    // @return 成功...true
    bool Initialize( XAUDIO2_PROCESSOR processor = XAUDIO2_DEFAULT_PROCESSOR );

    // @brief 終了
    void Finalize();

    // @brief 更新 TODO: 3Dサウンド処理実装予定
    void Update( float fElapsedTime );

    //--------------------------------------------------
    // 取得・設定
    //--------------------------------------------------

    // @brief マスター音量を設定
    void SetMasterVolume( float value ) {
        g_pMasteringVoice->SetVolume(value);
    }

    // @brief マスター音量を返す
    // @return マスター音量
    float GetMasterVolume() const {
        float ret = 0; g_pMasteringVoice->GetVolume(&ret); return ret;
    }

public:

    IXAudio2*               g_xAudio2;
    IXAudio2MasteringVoice* g_pMasteringVoice;
    IXAudio2SubmixVoice*    g_pSubmixVoice;
    X3DAUDIO_HANDLE         g_x3DAudioInstance;
    IUnknown*               g_pReverbEffect;

    int nFrameToApply3DAudio;

    DWORD dwChannelMask;
    UINT32 nChannels;

    X3DAUDIO_DSP_SETTINGS dspSettings;
    X3DAUDIO_LISTENER listener;

    float3 vListenerPos;
    float fListenerAngle;
    bool  fUseListenerCone;
    bool  fUseInnerRadius;
    bool  fUseRedirectToLFE;

    X3DAUDIO_EMITTER emitter;
    float3 vEmitterPos;
    X3DAUDIO_CONE emitterCone;

    FLOAT32 emitterAzimuths[INPUTCHANNELS];
    FLOAT32 matrixCoefficients[INPUTCHANNELS * OUTPUTCHANNELS];

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
