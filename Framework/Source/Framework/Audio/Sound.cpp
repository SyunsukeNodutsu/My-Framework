#include "Sound.h"
#include "Vender/WaveBankReader.h"
#include "Vender/WAVFileReader.h"

#include "../../Application/ImGuiSystem.h"

//=============================================================================
//
// SoundData
//
//=============================================================================

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
SoundData::SoundData()
    : m_pSourceVoice(nullptr)
    , m_buffer()
{
}

//-----------------------------------------------------------------------------
// 読み込み
//-----------------------------------------------------------------------------
bool SoundData::Load(const std::string& filepath, bool loop)
{
    std::wstring wfilepath = sjis_to_wide(filepath);

    // 波形ファイルをディレクトリから探す
    // TODO: WinAPIに似たような機能があるらしい
    WCHAR strFilePath[MAX_PATH];
    HRESULT hr = g_audioDevice->FindMediaFileCch(strFilePath, MAX_PATH, wfilepath.c_str());
    if (FAILED(hr)) {
        IMGUISYSTEM.AddLog(std::string("ERROR: Failed to find media file: " + filepath).c_str());
        return false;
    }

    // 波形ファイルの読み込み
    // TODO: .wavにシーク設定やループ設定があると下記の方法だと対応できない
    const uint8_t* sampleData;
    uint32_t waveSize;
    if (FAILED(DirectX::LoadWAVAudioFromFile(strFilePath, m_pWaveData, &m_pWaveFormat, &sampleData, &waveSize))) {
        IMGUISYSTEM.AddLog(std::string("ERROR: Failed reading WAV file: " + filepath).c_str());
        return false;
    }

    // XAUDIO2_BUFFER構造体の設定
    m_buffer.pAudioData = sampleData;
    m_buffer.Flags      = XAUDIO2_END_OF_STREAM;
    m_buffer.AudioBytes = waveSize;
    m_buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;

    return Create(loop);
}

//-----------------------------------------------------------------------------
// 解放
//-----------------------------------------------------------------------------
void SoundData::Release()
{
    if (m_pSourceVoice) {
        m_pSourceVoice->DestroyVoice();
        m_pSourceVoice = nullptr;
    }
    m_pWaveData.reset();
}

//-----------------------------------------------------------------------------
// 作成
//-----------------------------------------------------------------------------
bool SoundData::Create(bool loop)
{
    if (!AudioDeviceChild::g_audioDevice) return false;
    if (!AudioDeviceChild::g_audioDevice->g_xAudio2) return false;

    // 送信先になるの宛先ボイスを定義
    // 今回はサブミックスとマスタリングボイス
    XAUDIO2_SEND_DESCRIPTOR sendDescriptors[2];
    sendDescriptors[0].Flags        = XAUDIO2_SEND_USEFILTER; // LPFダイレクトパス
    sendDescriptors[0].pOutputVoice = g_audioDevice->g_pMasteringVoice;
    sendDescriptors[1].Flags        = XAUDIO2_SEND_USEFILTER; // LPFリバーブパス -- 省略するとパフォーマンスが向上しますが、オクルージョンがリアルでなくなります。
    sendDescriptors[1].pOutputVoice = g_audioDevice->g_pSubmixVoice;

    const XAUDIO2_VOICE_SENDS sendList = { 2, sendDescriptors };

    // ソースボイスの作成
    if (FAILED(g_audioDevice->g_xAudio2->CreateSourceVoice(&m_pSourceVoice, m_pWaveFormat, 0, 2.0f, nullptr, &sendList))) {
        return false;
    }

    // 音声キューに新しいオーディオバッファを追加
    if (FAILED(m_pSourceVoice->SubmitSourceBuffer(&m_buffer))) {
        return false;
    }

    return true;
}



//=============================================================================
//
// SoundWork
//
//=============================================================================

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
SoundWork::SoundWork()
    : m_pSourceVoice(nullptr)
    , m_soundData()
    , m_filepath("")
{
}

//-----------------------------------------------------------------------------
// 読み込み 作成
//-----------------------------------------------------------------------------
bool SoundWork::Load(const std::string& filepath, bool loop)
{
    if (!AudioDeviceChild::g_audioDevice) return false;
    if (!AudioDeviceChild::g_audioDevice->g_xAudio2) return false;

    m_filepath = filepath;

    if (!m_soundData.Load(filepath, loop)) {
        IMGUISYSTEM.AddLog("ERROR: Failed to load voice.");
        return false;
    }

    // ボイスコピー
    m_pSourceVoice = m_soundData.GetRawVoice();

    IMGUISYSTEM.AddLog(std::string("INFO: Load voice done: " + filepath).c_str());

    return false;
}

//-----------------------------------------------------------------------------
// 解放
//-----------------------------------------------------------------------------
void SoundWork::Release()
{
    // ソース元の解放
    m_soundData.Release();
}

//-----------------------------------------------------------------------------
// 再生
//-----------------------------------------------------------------------------
void SoundWork::Play(DWORD delay)
{
    if (!AudioDeviceChild::g_audioDevice) return;
    if (!AudioDeviceChild::g_audioDevice->g_xAudio2) return;
    if (!m_pSourceVoice) return;

    if (delay > 0)
    {
        std::thread([=]
            { Sleep(delay); m_pSourceVoice->Start(0); }
        ).detach();
        return;
    }

    m_pSourceVoice->Start(0);
}

//-----------------------------------------------------------------------------
// 停止
//-----------------------------------------------------------------------------
void SoundWork::Stop()
{
    if (!AudioDeviceChild::g_audioDevice) return;
    if (!AudioDeviceChild::g_audioDevice->g_xAudio2) return;
    if (!m_pSourceVoice) return;

    m_pSourceVoice->Stop();
}

//-----------------------------------------------------------------------------
// 音量設定
//-----------------------------------------------------------------------------
void SoundWork::SetVolume(float val)
{
    if (!AudioDeviceChild::g_audioDevice) return;
    if (!AudioDeviceChild::g_audioDevice->g_xAudio2) return;
    if (!m_pSourceVoice) return;

    m_pSourceVoice->SetVolume(val);
}

//-----------------------------------------------------------------------------
// 音量を返す
//-----------------------------------------------------------------------------
float SoundWork::GetVolume()
{
    if (!AudioDeviceChild::g_audioDevice) return FLT_MAX;
    if (!AudioDeviceChild::g_audioDevice->g_xAudio2) return FLT_MAX;
    if (!m_pSourceVoice) return false;

    float ret; m_pSourceVoice->GetVolume(&ret);
    return ret;
}

//-----------------------------------------------------------------------------
// 再生中かどうかを返す
//-----------------------------------------------------------------------------
bool SoundWork::IsPlaying()
{
    if (!AudioDeviceChild::g_audioDevice) return false;
    if (!AudioDeviceChild::g_audioDevice->g_xAudio2) return false;
    if (!m_pSourceVoice) return false;

    XAUDIO2_VOICE_STATE pState;
    m_pSourceVoice->GetState(&pState);

    return (pState.BuffersQueued > 0);
}
