#include "Sound.h"
#include "Vender/WaveBankReader.h"
#include "Vender/WAVFileReader.h"

#include "../../Application/ImGuiSystem.h"

HRESULT FindMediaFileCch(WCHAR* strDestPath, int cchDest, LPCWSTR strFilename);

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Sound::Sound()
    : m_pSourceVoice(nullptr)
    , m_buffer()
    , m_waveFile()
    , m_waveData()
{
}

//-----------------------------------------------------------------------------
// 解放
//-----------------------------------------------------------------------------
void Sound::Release()
{
    if (m_pSourceVoice) {
        m_pSourceVoice->DestroyVoice();
        m_pSourceVoice = nullptr;
    }
}

//-----------------------------------------------------------------------------
// 再生
//-----------------------------------------------------------------------------
void Sound::Play()
{
    if (m_pSourceVoice) m_pSourceVoice->Start(0);
}

//-----------------------------------------------------------------------------
// 読み込み
//-----------------------------------------------------------------------------
HRESULT Sound::Load(const std::string& filepath)
{
    //--------------------------------------------------
    // 読み込み
    //--------------------------------------------------

    std::wstring wfilepath = sjis_to_wide(filepath);

    // 波形ファイルを探す
    // TODO: WinAPIに似たような機能があるらしい
    WCHAR strFilePath[MAX_PATH];
    HRESULT hr = FindMediaFileCch(strFilePath, MAX_PATH, wfilepath.c_str());
    if (FAILED(hr)) {
        IMGUISYSTEM.AddLog(std::string("ERROR: Failed to find media file: " + filepath).c_str());
        return hr;
    }

    // 波形ファイルの読み込み
    // Microsoftのサンプル"LoadWAVAudioFromFileEx"を使用
    if (FAILED(hr = DirectX::LoadWAVAudioFromFileEx(strFilePath, m_waveFile, m_waveData))) {
        IMGUISYSTEM.AddLog(std::string("ERROR: Failed reading WAV file: " + filepath).c_str());
        return hr;
    }

    //--------------------------------------------------
    // 作成
    //--------------------------------------------------

    // IXAudio2SourceVoiceの作成
    if (FAILED(hr = g_audioDevice->g_xAudio2->CreateSourceVoice(&m_pSourceVoice, m_waveData.wfx))) {
        IMGUISYSTEM.AddLog("ERROR: Failed to create source voice.");
        return hr;
    }

    // XAUDIO2_BUFFER構造体を使用し 波形のサンプルデータを送信

    m_buffer.pAudioData = m_waveData.startAudio;
    m_buffer.Flags      = XAUDIO2_END_OF_STREAM;// データは単発
    m_buffer.AudioBytes = m_waveData.audioBytes;

    if (m_waveData.loopLength > 0)
    {
        m_buffer.LoopBegin  = m_waveData.loopStart;
        m_buffer.LoopLength = m_waveData.loopLength;
        m_buffer.LoopCount  = 1;
    }

    // WAVEに対応
    if (m_waveData.seek) {
        IMGUISYSTEM.AddLog("ERROR: This platform does not support xWMA or XMA2.");
        Release();
        return hr;
    }

    // 音声キューに新しいオーディオバッファを追加
    if (FAILED(hr = m_pSourceVoice->SubmitSourceBuffer(&m_buffer))) {
        IMGUISYSTEM.AddLog("ERROR: Failed to add audio buffer.");
        Release();
        return hr;
    }

    return hr;
}

//--------------------------------------------------------------------------------------
// メディアファイルの位置を確認するためのヘルパー関数
//--------------------------------------------------------------------------------------
HRESULT FindMediaFileCch(WCHAR* strDestPath, int cchDest, LPCWSTR strFilename)
{
    bool bFound = false;

    if (!strFilename || strFilename[0] == 0 || !strDestPath || cchDest < 10)
        return E_INVALIDARG;

    // Get the exe name, and exe path
    WCHAR strExePath[MAX_PATH] = { 0 };
    WCHAR strExeName[MAX_PATH] = { 0 };
    WCHAR* strLastSlash = nullptr;
    GetModuleFileName(nullptr, strExePath, MAX_PATH);
    strExePath[MAX_PATH - 1] = 0;
    strLastSlash = wcsrchr(strExePath, TEXT('\\'));
    if (strLastSlash)
    {
        wcscpy_s(strExeName, MAX_PATH, &strLastSlash[1]);

        // Chop the exe name from the exe path
        *strLastSlash = 0;

        // Chop the .exe from the exe name
        strLastSlash = wcsrchr(strExeName, TEXT('.'));
        if (strLastSlash)
            *strLastSlash = 0;
    }

    wcscpy_s(strDestPath, cchDest, strFilename);
    if (GetFileAttributes(strDestPath) != 0xFFFFFFFF)
        return S_OK;

    // Search all parent directories starting at .\ and using strFilename as the leaf name
    WCHAR strLeafName[MAX_PATH] = { 0 };
    wcscpy_s(strLeafName, MAX_PATH, strFilename);

    WCHAR strFullPath[MAX_PATH] = { 0 };
    WCHAR strFullFileName[MAX_PATH] = { 0 };
    WCHAR strSearch[MAX_PATH] = { 0 };
    WCHAR* strFilePart = nullptr;

    GetFullPathName(L".", MAX_PATH, strFullPath, &strFilePart);
    if (!strFilePart)
        return E_FAIL;

    while (strFilePart && *strFilePart != '\0')
    {
        swprintf_s(strFullFileName, MAX_PATH, L"%s\\%s", strFullPath, strLeafName);
        if (GetFileAttributes(strFullFileName) != 0xFFFFFFFF)
        {
            wcscpy_s(strDestPath, cchDest, strFullFileName);
            bFound = true;
            break;
        }

        swprintf_s(strFullFileName, MAX_PATH, L"%s\\%s\\%s", strFullPath, strExeName, strLeafName);
        if (GetFileAttributes(strFullFileName) != 0xFFFFFFFF)
        {
            wcscpy_s(strDestPath, cchDest, strFullFileName);
            bFound = true;
            break;
        }

        swprintf_s(strSearch, MAX_PATH, L"%s\\..", strFullPath);
        GetFullPathName(strSearch, MAX_PATH, strFullPath, &strFilePart);
    }
    if (bFound)
        return S_OK;

    // On failure, return the file as the path but also return an error code
    wcscpy_s(strDestPath, cchDest, strFilename);

    return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
}
