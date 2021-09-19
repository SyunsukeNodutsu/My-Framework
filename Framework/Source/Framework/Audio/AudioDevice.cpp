#include "AudioDevice.h"
#include "../../Application/ImGuiSystem.h"

// サウンドコーン
static const X3DAUDIO_CONE Listener_DirectionalCone = { X3DAUDIO_PI * 5.0f / 6.0f, X3DAUDIO_PI * 11.0f / 6.0f, 1.0f, 0.75f, 0.0f, 0.25f, 0.708f, 1.0f };

// g_PRESET_NAMESの順序と一致する必要があります
XAUDIO2FX_REVERB_I3DL2_PARAMETERS g_PRESET_PARAMS[NUM_PRESETS] =
{
    XAUDIO2FX_I3DL2_PRESET_FOREST,
    XAUDIO2FX_I3DL2_PRESET_DEFAULT,
    XAUDIO2FX_I3DL2_PRESET_GENERIC,
    XAUDIO2FX_I3DL2_PRESET_PADDEDCELL,
    XAUDIO2FX_I3DL2_PRESET_ROOM,
    XAUDIO2FX_I3DL2_PRESET_BATHROOM,
    XAUDIO2FX_I3DL2_PRESET_LIVINGROOM,
    XAUDIO2FX_I3DL2_PRESET_STONEROOM,
    XAUDIO2FX_I3DL2_PRESET_AUDITORIUM,
    XAUDIO2FX_I3DL2_PRESET_CONCERTHALL,
    XAUDIO2FX_I3DL2_PRESET_CAVE,
    XAUDIO2FX_I3DL2_PRESET_ARENA,
    XAUDIO2FX_I3DL2_PRESET_HANGAR,
    XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY,
    XAUDIO2FX_I3DL2_PRESET_HALLWAY,
    XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR,
    XAUDIO2FX_I3DL2_PRESET_ALLEY,
    XAUDIO2FX_I3DL2_PRESET_CITY,
    XAUDIO2FX_I3DL2_PRESET_MOUNTAINS,
    XAUDIO2FX_I3DL2_PRESET_QUARRY,
    XAUDIO2FX_I3DL2_PRESET_PLAIN,
    XAUDIO2FX_I3DL2_PRESET_PARKINGLOT,
    XAUDIO2FX_I3DL2_PRESET_SEWERPIPE,
    XAUDIO2FX_I3DL2_PRESET_UNDERWATER,
    XAUDIO2FX_I3DL2_PRESET_SMALLROOM,
    XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM,
    XAUDIO2FX_I3DL2_PRESET_LARGEROOM,
    XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL,
    XAUDIO2FX_I3DL2_PRESET_LARGEHALL,
    XAUDIO2FX_I3DL2_PRESET_PLATE,
};

using namespace DirectX;

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
AudioDevice::AudioDevice()
    : g_xAudio2(nullptr)
    , g_pMasteringVoice(nullptr)
    , g_pSubmixVoice(nullptr)
    , g_x3DAudioInstance()
    , g_pReverbEffect(nullptr)
    , g_peakLevels()
    , g_RMSLevels()
    , done(false)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool AudioDevice::Initialize(XAUDIO2_PROCESSOR processor)
{
    if (done) return false;

    // IXAudio2作成
    // DebugConfigurationは設定なし
    UINT32 flags = 0;
    HRESULT hr = XAudio2Create(&g_xAudio2, flags);
    if (!g_xAudio2) {
        IMGUISYSTEM.AddLog("ERROR: Failed to inialize audio device.");
        return false;
    }

    // IXAudio2MasteringVoice作成
    hr = g_xAudio2->CreateMasteringVoice(&g_pMasteringVoice);
    if (!g_pMasteringVoice) {
        IMGUISYSTEM.AddLog("ERROR: Failed to create mastering voice.");
        return false;
    }

    // デバイスの詳細を確認し
    // サンプルでサポートされているパラメータの範囲内であることを確認
    DWORD dwChannelMask;
    UINT32 nSampleRate;

    XAUDIO2_VOICE_DETAILS details;
    g_pMasteringVoice->GetVoiceDetails(&details);

    if (details.InputChannels > OUTPUTCHANNELS) {
        return false;
    }

    if (FAILED(hr = g_pMasteringVoice->GetChannelMask(&dwChannelMask))) {
        return false;
    }

    nSampleRate     = details.InputSampleRate;
    g_channels      = details.InputChannels;
    dwChannelMask   = dwChannelMask;

    // リバーブエフェクトの作成
    UINT32 rflags = 0;
    if (FAILED(XAudio2CreateReverb(&g_pReverbEffect, rflags))) {
        return false;
    }

    // サブミックスボイスの作成

    // パフォーマンスのヒント：グローバルFXを最終ミックスと同じサンプル数のチャンネルで実行する必要はありません。
    // のチャンネル数でグローバルFXを実行する必要はありません。 例えば、このサンプルでは
    // 例えば、このサンプルではリバーブをモノラルモードで動作させ、CPUのオーバーヘッドを減らしています。
    XAUDIO2_EFFECT_DESCRIPTOR effects[] = { { g_pReverbEffect, TRUE, 1 } };
    XAUDIO2_EFFECT_CHAIN effectChain = { 1, effects };

    if (FAILED(hr = g_xAudio2->CreateSubmixVoice(&g_pSubmixVoice,
        1, nSampleRate, 0, 0, nullptr, &effectChain))) {
        return false;
    }

    // デフォルトのFXパラメター設定
    XAUDIO2FX_REVERB_PARAMETERS native;
    ReverbConvertI3DL2ToNative(&g_PRESET_PARAMS[0], &native);
    g_pSubmixVoice->SetEffectParameters(0, &native, sizeof(native));

    Initialize3D();

    // ボリュームメータAPO作成
    InitializeVolumeMeterAPO();

    IMGUISYSTEM.AddLog("INFO: AudioDevice Initialized.");

    return done = true;
}

//-----------------------------------------------------------------------------
// 3DAudio初期化
//-----------------------------------------------------------------------------
bool AudioDevice::Initialize3D()
{
    //
    // X3DAudioの初期化 X3DAudio の初期化
    // 最終ミックスでのスピーカーのジオメトリ設定。WAVEFORMATEXTENSIBLE.dwChannelMaskで定義された、
    // スピーカーの位置へのチャンネルの割り当てを指定します。
    //
    // SpeedOfSound - ユーザー定義のワールドユニット/秒での音速，ドップラー計算にのみ使用されます．
    // ドップラー計算にのみ使用され、 >= FLT_MIN でなければなりません。
    //

    DWORD dwChannelMask;
    g_pMasteringVoice->GetChannelMask(&dwChannelMask);

    constexpr float SPEEDOFSOUND = X3DAUDIO_SPEED_OF_SOUND;
    X3DAudioInitialize(dwChannelMask, SPEEDOFSOUND, g_x3DAudioInstance);

    //--------------------------------------------------
    // リスナー初期化
    //--------------------------------------------------
    
    m_listener.Position.x     = 
    m_listener.Position.y     = 
    m_listener.Position.z     = 0.f;

    m_listener.OrientFront.x  = 
    m_listener.OrientFront.y  = 
    m_listener.OrientTop.x    = 
    m_listener.OrientTop.z    = 0.f;

    m_listener.OrientFront.z  = 
    m_listener.OrientTop.y    = 1.0f;

    m_listener.pCone = (X3DAUDIO_CONE*)&Listener_DirectionalCone;

    return true;
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void AudioDevice::Finalize()
{
    if (!done) return;

    if (g_pSubmixVoice) {
        g_pSubmixVoice->DestroyVoice();
        g_pSubmixVoice = nullptr;
    }
    if (g_pMasteringVoice) {
        g_pMasteringVoice->DestroyVoice();
        g_pMasteringVoice = nullptr;
    }
    if (g_xAudio2) {
        g_xAudio2->StopEngine();
        g_xAudio2->Release();
        g_xAudio2 = nullptr;
    }

    if (g_pReverbEffect) {
        g_pReverbEffect->Release();
        g_pReverbEffect = nullptr;
    }
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void AudioDevice::Update( float fElapsedTime )
{
    if (!done) return;
    if (!g_xAudio2) return;
    if (!g_pMasteringVoice) return;


    UpdateVolumeMeter();
}

//-----------------------------------------------------------------------------
// リスナー更新
//-----------------------------------------------------------------------------
void AudioDevice::UpdateListener(const mfloat4x4& matrix)
{
    // 座標
    float3 pos = matrix.Translation();
    m_listener.Position.x = pos.x;
    m_listener.Position.y = pos.y;
    m_listener.Position.z = pos.z;

    // 方向
    float3 front = matrix.Backward();
    m_listener.OrientFront.x = front.x;
    m_listener.OrientFront.y = front.y;

    float3 top = matrix.Up();
    m_listener.OrientTop.x = top.x;
    m_listener.OrientTop.z = top.z;

    // 奥行 傾き
    m_listener.OrientFront.z = front.z;
    //m_listener.OrientTop.y = top.z;
}

//-----------------------------------------------------------------------------
// メディアファイルの位置を確認するためのヘルパー関数
//-----------------------------------------------------------------------------
HRESULT AudioDevice::FindMediaFileCch(WCHAR* strDestPath, int cchDest, LPCWSTR strFilename)
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

//-----------------------------------------------------------------------------
// ボリュームメータ(APO)の作成
//-----------------------------------------------------------------------------
bool AudioDevice::InitializeVolumeMeterAPO()
{
    // VolumeMeter(APO)作成
    IUnknown* pVolumeMeterAPO = nullptr;
    if (FAILED(XAudio2CreateVolumeMeter(&pVolumeMeterAPO))) {
        IMGUISYSTEM.AddLog("ERROR: Failed to create VolumeMeter(APO).");
        return false;
    }

    // EFFECT_DESCRIPTOR の作成
    XAUDIO2_EFFECT_DESCRIPTOR descriptor = {};
    descriptor.InitialState     = true;
    descriptor.OutputChannels   = 2;
    descriptor.pEffect          = pVolumeMeterAPO;

    // EFFECT_CHAIN の作成
    XAUDIO2_EFFECT_CHAIN chain = {};
    chain.EffectCount           = 1;
    chain.pEffectDescriptors    = &descriptor;
    if (FAILED(g_pMasteringVoice->SetEffectChain(&chain))) {
        IMGUISYSTEM.AddLog("ERROR: Failed to create EFFECT_CHAIN.");
        return false;
    }

    pVolumeMeterAPO->Release();
    IMGUISYSTEM.AddLog("INFO: Initialized VolumeMeterAPO.");

    return true;
}

//-----------------------------------------------------------------------------
// ボリュームメータ更新
//-----------------------------------------------------------------------------
void AudioDevice::UpdateVolumeMeter()
{
    if (g_pMasteringVoice == nullptr)
        return;

    // 受信用構造体 設定
    constexpr int channels = 2;// 左右
    XAUDIO2FX_VOLUMEMETER_LEVELS Levels = {};
    Levels.pPeakLevels  = g_peakLevels;
    Levels.pRMSLevels   = g_RMSLevels;
    Levels.ChannelCount = channels;

    // パラメータ受信
    if (FAILED(g_pMasteringVoice->GetEffectParameters(0, &Levels, sizeof(Levels)))) {
        DebugLog("ERROR: EffectParameters失敗.");
    }
}
