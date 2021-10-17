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
    bool Load(const std::string& filepath, bool loop, bool useFilter);

    // @brief 解放
    void Release();

    // @brief ソースボイスを返す ※TODO: 関数名変かも
    // @return 無加工のソースボイス
    IXAudio2SourceVoice* GetRawSourceVoice() const { return m_pSourceVoice; }

private:

    IXAudio2SourceVoice*        m_pSourceVoice;
    XAUDIO2_BUFFER              m_buffer;
    const WAVEFORMATEX*         m_pWaveFormat;  // WAVデータのフォーマット
    std::unique_ptr<uint8_t[]>  m_pWaveData;    // WAVデータの格納場所

    // @brief ボイス作成
    // @param loop ループ再生？
    // @return 成功...true
    bool Create(bool loop, bool useFilter);

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
    // @param useFilter フィルター機能使用？
    // @return 成功...true
    virtual bool Load(const std::string& filepath, bool loop = false, bool useFilter = false);

    // @brief 解放
    virtual void Release();

    // @brief 更新
    virtual void Update() {}

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
    // @note おそらくモノラル音源じゃないとパンできない
    // @param pan -1(左)～1(右)の間
    // @return 成功...true
    virtual bool SetPan(float pan);

    // @brief 音量を返す
    // @return 音量
    virtual float GetVolume();

    // @brief 音量を返す
    // @param targetVolume 到達目標ボリューム
    // @param targetTime フェード時間(秒)
    // @return フェード完了...true
    virtual bool SetFade(float targetVolume, float targetTime);

    // @brief 再生中かを返す
    // @note ループの場合は
    // @return 再生中...true 最後まで再生された...false
    virtual bool IsPlaying();

    // @brief 音源の名前を返す ※TODO: フルパスいらない
    // @return 音源の名前
    const std::string& GetName() const { return m_filepath; }

    // @brief 3Dサウンドかを返す
    // @return 3D...true
    bool Is3D() const { return m_is3D; }

    // @brief imGuiで編集用にパンの値を返す
    // @return パンの値
    float GetPan() const { return m_pan; }

    // @brief ソースボイスにフィルターを設定 ※注意.作成時にuseFilterフラグを立ててください
    // (LowPassFilter, 1, 1)でフィルタをOFFにします
    // @note frequencym...0.0～1.0, oneOverQ...0.0(以下はx)～1.5
    // @param type フィルタタイプ
    // @param frequencym カットオフ周波数 ※実際はラジアン周波数を入れる
    // @param oneOverQ フィルタに使用するQ値の逆数 ※通常は1.0fか1.4142fを使用する
    // @return 成功...true 失敗...false
    bool SetFilter(XAUDIO2_FILTER_TYPE type, float frequencym, float oneOverQ = 1.0f);

protected:

    // エフェクトがかかる可能性のある ボイスデータ
    IXAudio2SourceVoice* m_pSourceVoice;

    SoundData   m_soundData;// サウンドデータ
    std::string m_filepath; // 音源のファイルパス
    float       m_pan;      // imGuiで編集するように panの値を保存
    bool        m_is3D;     // 3Dサウンド？

};
