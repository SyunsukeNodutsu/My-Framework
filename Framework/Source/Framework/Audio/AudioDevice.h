//-----------------------------------------------------------------------------
// File: AudioDevice.h
//
// XAudio2 MasteringVoice
// TODO: APOを別クラスに移動したほうがいいかも
//-----------------------------------------------------------------------------
#pragma once

// X2Audio X3DAudio
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>

// link
#pragma comment(lib,"xaudio2.lib")

#define INPUTCHANNELS  1 // ソースチャンネルの数
#define OUTPUTCHANNELS 8 // このサンプルでサポートされるデスティネーションチャネルの最大数
#define NUM_PRESETS   30 // PRESETの数

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

    // @brief メディアファイルの位置を確認するためのヘルパー関数
    // @param strDestPath
    // @param cchDest
    // @param strFilename ファイルを確認するパス(位置)
    // @return 成功...S_OK 失敗...エラーメッセージ
    HRESULT FindMediaFileCch(_Out_writes_(cchDest) WCHAR* strDestPath, _In_ int cchDest, _In_z_ LPCWSTR strFilename);

    //--------------------------------------------------
    // 取得・設定
    //--------------------------------------------------
    
    // @brief マスター音量を設定
    // @param value 設定する値(波形の振幅なので0.5にしても半分ではない)
    void SetMasterVolume(float value);

    // @brief マスター音量を返す
    // @return 成功...マスター音量 失敗...FLT_MAX
    float GetMasterVolume() const;

public:

    IXAudio2*               g_xAudio2;
    IXAudio2MasteringVoice* g_pMasteringVoice;
    IXAudio2SubmixVoice*    g_pSubmixVoice;
    X3DAUDIO_HANDLE         g_x3DAudioInstance;
    IUnknown*               g_pReverbEffect;

    X3DAUDIO_LISTENER       m_listener;
    UINT32                  g_channels;

    FLOAT32 g_emitterAzimuths[INPUTCHANNELS];
    FLOAT32 g_matrixCoefficients[INPUTCHANNELS * OUTPUTCHANNELS];

    // ボリュームメータ
    float g_peakLevels[2];	// ピークメータ ※瞬間最大値
    float g_RMSLevels[2];	// RMSメータ ※平均値

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
