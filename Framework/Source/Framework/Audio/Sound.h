//-----------------------------------------------------------------------------
// File: Human.h
//
// 音声データ単位のサウンドクラス
// IXAudio2SourceVoice XAUDIO2_BUFFER
//-----------------------------------------------------------------------------
#pragma once
#include "AudioDeviceChild.h"
#include "Vender/WAVFileReader.h"

// WAVEデータ単位(データ側)
class SoundData : public AudioDeviceChild
{
public:

    // @brief コンストラクタ
    SoundData();

    // @brief 読み込み
    // @param filepath 音源のファイルパス
    // @param loop ループ再生？
    // @return 成功...true
    bool Load(const std::string& filepath, bool loop = false);

    // @brief 解放
    void Release();

    // @brief ソースボイスを返す ※TODO: 関数名変かも
    // @return 無加工のソースボイス
    IXAudio2SourceVoice* GetRawVoice() const { return m_pSourceVoice; }

private:

    IXAudio2SourceVoice*        m_pSourceVoice;
    XAUDIO2_BUFFER              m_buffer;
    const WAVEFORMATEX*         m_pWaveFormat;  // WAVデータのフォーマット
    std::unique_ptr<uint8_t[]>  m_pWaveData;    // WAVデータの格納場所

    // @brief ボイス作成
    // @param loop ループ再生？
    // @return 成功...true
    bool Create(bool loop);

};

// サウンドデータ(ロジック.操作側)
class SoundWork : public AudioDeviceChild
{
public:

    // @brief コンストラクタ
    SoundWork();

    // @brief 読み込み作成
    // @param filepath 音源のファイルパス
    // @param loop ループ再生？
    // @return 成功...true
    virtual bool Load(const std::string& filepath, bool loop = false);

    // @brief 解放
    virtual void Release();

    // @brief 再生
    // @param delay 再生までの待ち時間(ミリ秒)
    virtual void Play(DWORD delay = 0);

    // @brief 停止
    virtual void Stop();

    // @brief 音量を設定する
    // @param val 設定音量
    virtual void SetVolume(float val);

    // @brief 音をパンする
    // @note 3Dサウンドでパンすると3D計算が無効になってしまう
    // @param pan -1(左)～1(右)の間
    virtual void SetPan(float pan);

    // @brief 音量を返す
    // @return 音量
    virtual float GetVolume();

    // @brief 再生中かを返す
    // @note ループの場合は
    // @return 再生中...true 最後まで再生された...false
    virtual bool IsPlaying();

protected:

    // エフェクトがかかる可能性のある ボイスデータ
    IXAudio2SourceVoice* m_pSourceVoice;

    // サウンドデータ
    SoundData m_soundData;

    // 音源のファイルパス
    std::string m_filepath;

};
