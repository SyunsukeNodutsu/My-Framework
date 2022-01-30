#include "Sound.h"
#include "Vender/WaveBankReader.h"
#include "Vender/WAVFileReader.h"

#include "../../Application/main.h"

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
bool SoundData::Load(const std::string& filepath, bool loop, bool useFilter)
{
    std::wstring wfilepath = sjis_to_wide(filepath);

    // 波形ファイルをディレクトリから探す
    WCHAR strFilePath[MAX_PATH];
    HRESULT hr = FindMediaFileCch(strFilePath, MAX_PATH, wfilepath.c_str());
    if (FAILED(hr)) {
        DebugLog(std::string("ERROR: Failed to find media file: " + filepath).c_str());
        return false;
    }

    // 波形ファイルの読み込み
    // TODO: .wavにシーク設定やループ設定があると下記の方法だと対応できない
    const uint8_t* sampleData;
    uint32_t waveSize;
    if (FAILED(DirectX::LoadWAVAudioFromFile(strFilePath, m_pWaveData, &m_pWaveFormat, &sampleData, &waveSize))) {
        DebugLog(std::string("ERROR: Failed reading WAV file: " + filepath).c_str());
        return false;
    }

    // XAUDIO2_BUFFER構造体の設定
    m_buffer.pAudioData = sampleData;
    m_buffer.Flags      = XAUDIO2_END_OF_STREAM;
    m_buffer.AudioBytes = waveSize;
    m_buffer.LoopCount  = loop ? XAUDIO2_LOOP_INFINITE : 0;

    return Create(loop, useFilter);
}

//-----------------------------------------------------------------------------
// 解放
//-----------------------------------------------------------------------------
void SoundData::Release()
{
    if (!AudioDeviceChild::g_audioDevice) return;
    if (!AudioDeviceChild::g_audioDevice->g_xAudio2) return;

    if (m_pSourceVoice) {
        m_pSourceVoice->DestroyVoice();
        m_pSourceVoice = nullptr;
    }
    m_pWaveData.reset();
}

//-----------------------------------------------------------------------------
// 作成
//-----------------------------------------------------------------------------
bool SoundData::Create(bool loop, bool useFilter)
{
    if (!AudioDeviceChild::g_audioDevice) return false;
    if (!AudioDeviceChild::g_audioDevice->g_xAudio2) return false;

    // 送信先になるの宛先ボイスを定義
    // 今回はサブミックスとマスタリングボイス
    XAUDIO2_SEND_DESCRIPTOR sendDescriptors[2];
    sendDescriptors[0].Flags        = XAUDIO2_SEND_USEFILTER; // LPFダイレクトパス
    sendDescriptors[0].pOutputVoice = g_audioDevice->g_pMasteringVoice;
    sendDescriptors[1].Flags        = XAUDIO2_SEND_USEFILTER; // LPFリバーブパス ※少し重いらしい
    sendDescriptors[1].pOutputVoice = g_audioDevice->g_pSubmixVoice;

    const XAUDIO2_VOICE_SENDS sendList = { 2, sendDescriptors };

    // ソースボイスの作成
    if (FAILED(g_audioDevice->g_xAudio2->CreateSourceVoice(&m_pSourceVoice, m_pWaveFormat,
        useFilter ? XAUDIO2_VOICE_USEFILTER : 0, 2.0f, nullptr, &sendList))) {
        ApplicationChilled::GetApplication()->g_imGuiSystem->AddLog("ERROR: Failed to create source voice.");
        return false;
    }

    // 音声キューに新しいオーディオバッファを追加
    if (FAILED(m_pSourceVoice->SubmitSourceBuffer(&m_buffer))) {
        ApplicationChilled::GetApplication()->g_imGuiSystem->AddLog("ERROR: Failed to submit source buffer.");
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
    , m_pan(0.0f)
    , m_is3D(false)
{
}

//-----------------------------------------------------------------------------
// 読み込み 作成
//-----------------------------------------------------------------------------
bool SoundWork::Load(const std::string& filepath, bool loop, bool useFilter)
{
    if (!AudioDeviceChild::g_audioDevice) return false;
    if (!AudioDeviceChild::g_audioDevice->g_xAudio2) return false;

    // "Resource/Audio/"(15文字) 以降のみ
    m_filepath = filepath.substr(15);

    if (!m_soundData.Load(filepath, loop, useFilter)) {
        ApplicationChilled::GetApplication()->g_imGuiSystem->AddLog("ERROR: Failed to load voice.");
        return false;
    }

    // ボイスコピー
    m_pSourceVoice = m_soundData.GetRawSourceVoice();

    return true;
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
        std::thread([=] {
            Sleep(delay);
            m_pSourceVoice->Start(0);
            return;
        }).detach();
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

    constexpr float MY_MAX_VOLUME_LEVEL = 5.0f;
    val = std::clamp(val, -MY_MAX_VOLUME_LEVEL, MY_MAX_VOLUME_LEVEL);

    m_pSourceVoice->SetVolume(val);
}

//-----------------------------------------------------------------------------
// 音をパンする
//-----------------------------------------------------------------------------
bool SoundWork::SetPan(float pan)
{
    if (!AudioDeviceChild::g_audioDevice) return false;
    if (!AudioDeviceChild::g_audioDevice->g_xAudio2) return false;
    if (!m_pSourceVoice) return false;

    pan = std::clamp(pan, -1.0f, 1.0f);
    m_pan = pan;

    float outputMatrix[8];
    for (int i = 0; i < 8; i++)
        outputMatrix[i] = 0;

    float left  = 0.5f - pan / 2;
    float right = 0.5f + pan / 2;

    // スピーカー構成に応じてパンの値を設定
    switch (g_audioDevice->g_channelMask)
    {
    case SPEAKER_MONO:
        outputMatrix[0] = 1.0;
        break;
    case SPEAKER_STEREO:
    case SPEAKER_2POINT1:
    case SPEAKER_SURROUND:
        outputMatrix[0] = left;
        outputMatrix[1] = right;
        break;
    case SPEAKER_QUAD:
        outputMatrix[0] = outputMatrix[2] = left;
        outputMatrix[1] = outputMatrix[3] = right;
        break;
    case SPEAKER_4POINT1:
        outputMatrix[0] = outputMatrix[3] = left;
        outputMatrix[1] = outputMatrix[4] = right;
        break;
    case SPEAKER_5POINT1:
    case SPEAKER_7POINT1:
    case SPEAKER_5POINT1_SURROUND:
        outputMatrix[0] = outputMatrix[4] = left;
        outputMatrix[1] = outputMatrix[5] = right;
        break;
    case SPEAKER_7POINT1_SURROUND:
        outputMatrix[0] = outputMatrix[4] = outputMatrix[6] = left;
        outputMatrix[1] = outputMatrix[5] = outputMatrix[7] = right;
        break;
    }

    // 出力マトリックスを発信元の音声に適用
    
    // 本フレームワークは単一のターゲットボイスではないので
    // 宛先(第一引数)はNULLではなくMasteringVoiceを指定
    if (FAILED(m_pSourceVoice->SetOutputMatrix(g_audioDevice->g_pMasteringVoice,
        INPUTCHANNELS, g_audioDevice->g_inputChannels, outputMatrix))) {
        ApplicationChilled::GetApplication()->g_imGuiSystem->AddLog("ERROR: Failed to pan voice. 多くの場合、モノラル音源じゃないのが理由です.\n");
        return false;
    }

    return true;
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
// フェード設定
//-----------------------------------------------------------------------------
bool SoundWork::SetFade(float targetVolume, float targetTime)
{
    if (!AudioDeviceChild::g_audioDevice) return false;
    if (!AudioDeviceChild::g_audioDevice->g_xAudio2) return false;
    if (!m_pSourceVoice) return false;

    constexpr float MY_MAX_VOLUME_LEVEL = 5.0f;
    targetVolume = std::clamp(targetVolume, -MY_MAX_VOLUME_LEVEL, MY_MAX_VOLUME_LEVEL);

    constexpr float MAX_TARGET_TIME = 8.0f;// あまりに遅いと違和感が出る
    targetTime = std::clamp(targetTime, -MAX_TARGET_TIME, MAX_TARGET_TIME);

    // TODO: せっかくだからLuaで実装したい

    return true;
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

//-----------------------------------------------------------------------------
// フィルターの設定
//-----------------------------------------------------------------------------
bool SoundWork::SetFilter(XAUDIO2_FILTER_TYPE type, float frequencym, float oneOverQ)
{
    if (!AudioDeviceChild::g_audioDevice) return false;
    if (!AudioDeviceChild::g_audioDevice->g_xAudio2) return false;
    if (!m_pSourceVoice) return false;
    // clampでもいいかも
    if (frequencym < 0 || frequencym > XAUDIO2_MAX_FILTER_FREQUENCY) return false;
    if (oneOverQ <= 0 || oneOverQ > XAUDIO2_MAX_FILTER_ONEOVERQ) return false;

    XAUDIO2_FILTER_PARAMETERS FilterParams;
    FilterParams.Type       = type;
    FilterParams.Frequency  = frequencym;   // 0.0f(未満はx)～1.0f(XAUDIO2_MAX_FILTER_FREQUENCY)
    FilterParams.OneOverQ   = oneOverQ;     // 0.0f(以下はx)～1.5f(XAUDIO2_MAX_FILTER_ONEOVERQ)
    if (FAILED(m_pSourceVoice->SetFilterParameters(&FilterParams))) {
        ApplicationChilled::GetApplication()->g_imGuiSystem->AddLog("WORNING: Failed to set filters.");
        return false;
    }
    return true;
}
