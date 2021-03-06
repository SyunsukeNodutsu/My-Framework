//-----------------------------------------------------------------------------
// File: AudioDevice.h
//
// XAudio2 MasteringVoice
// TODO: APOを別クラスに移動したほうがいいかも
// TODO: public変数の選定 必要じゃない変数(メータなど)をprivateに移動
//-----------------------------------------------------------------------------
#pragma once
#define INPUTCHANNELS  1 // ソースチャンネルの数
#define OUTPUTCHANNELS 8 // デスティネーションチャネルの最大数

class SoundWork;

// XAudio2のエンジン部分管理
class AudioDevice
{
public:

    // @brief コンストラクタ
    AudioDevice();

    // @brief 初期化
    // @return 成功...true
    bool Initialize();

    // @brief 終了
    void Finalize();

    // @brief サウンド更新
    // @param matrix リスナーのワールド行列
    void Update(const mfloat4x4& listener);

    //--------------------------------------------------
    // 取得・設定
    //--------------------------------------------------
    
    // @brief マスター音量を設定
    // @param value 設定する値(波形の振幅なので0.5にしても半分ではない)
    void SetMasterVolume(float value);

    // @brief マスター音量を返す
    // @return 成功...マスター音量 失敗...FLT_MAX
    float GetMasterVolume() const;

    // @brief リバーブを設定する
    // @param rparam 設定するリバーブ
    // @return 成功...true
    bool SetReverb(XAUDIO2FX_REVERB_I3DL2_PARAMETERS rparam);

public:

    IXAudio2*               g_xAudio2;
    IXAudio2MasteringVoice* g_pMasteringVoice;
    IXAudio2SubmixVoice*    g_pSubmixVoice;
    X3DAUDIO_HANDLE         g_x3DAudioInstance;
    IUnknown*               g_pReverbEffect;

    X3DAUDIO_LISTENER       m_listener;
    UINT32                  g_inputSampleRate;
    UINT32                  g_inputChannels;
    DWORD                   g_channelMask;

    FLOAT32 g_emitterAzimuths[INPUTCHANNELS];
    FLOAT32 g_matrixCoefficients[INPUTCHANNELS * OUTPUTCHANNELS];

    // ボリュームメータ
    std::array<float, 8> g_peakLevels;  // ピークメータ ※瞬間最大値
    std::array<float, 8> g_RMSLevels;   // RMSメータ ※平均値

private:

    // 初期化完了？
    bool done = false;

    // サウンド一覧(プレイリスト)
    std::list<std::shared_ptr<SoundWork>> m_soundList;

private:

    // @brief x3DAudio初期化
    // @return 成功...true
    bool Initialize3D();

    // @brief ボリュームメータ(APO)の作成
    // @return 成功...true
    bool InitializeVolumeMeterAPO();

    // @brief ボリュームメータ更新
    void UpdateVolumeMeter();

};
