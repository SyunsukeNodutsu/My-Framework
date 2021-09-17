//-----------------------------------------------------------------------------
// File: Human.h
//
// サウンドクラス
// IXAudio2SourceVoice XAUDIO2_BUFFER
//-----------------------------------------------------------------------------
#pragma once
#include "AudioDeviceChild.h"

#include "Vender/WAVFileReader.h"

// Sound単位 読み込みから再生までを行うクラス
class Sound : public AudioDeviceChild
{
public:

    // @brief コンストラクタ
    Sound();

    // @brief 解放
    void Release();

    // @brief 再生
    void Play();

protected:

    IXAudio2SourceVoice*        m_pSourceVoice;
    XAUDIO2_BUFFER              m_buffer;
    std::unique_ptr<uint8_t[]>  m_waveFile; //
    DirectX::WAVData            m_waveData; //

protected:

    // @brief 読み込み ソースボイスの作成
    // @param filepath WAVEファイル パス
    HRESULT Load(const std::string& filepath);

};
