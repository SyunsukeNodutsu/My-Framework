#include "Sound3D.h"
#include "Vender/WAVFileReader.h"

#include "../../Application/ImGuiSystem.h"

// LFEレベルの距離曲線
static const X3DAUDIO_DISTANCE_CURVE_POINT Emitter_LFE_CurvePoints[3] = { 0.0f, 1.0f, 0.25f, 0.0f, 1.0f, 0.0f };
static const X3DAUDIO_DISTANCE_CURVE       Emitter_LFE_Curve = { (X3DAUDIO_DISTANCE_CURVE_POINT*)&Emitter_LFE_CurvePoints[0], 3 };

// リバーブセンドレベルの距離曲線
static const X3DAUDIO_DISTANCE_CURVE_POINT Emitter_Reverb_CurvePoints[3] = { 0.0f, 0.5f, 0.75f, 1.0f, 1.0f, 0.0f };
static const X3DAUDIO_DISTANCE_CURVE       Emitter_Reverb_Curve = { (X3DAUDIO_DISTANCE_CURVE_POINT*)&Emitter_Reverb_CurvePoints[0], 3 };

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
SoundWork3D::SoundWork3D()
    : m_emitter()
    , m_emitterCone()
    , m_dspSettings()
{
    m_is3D = true;
}

//-----------------------------------------------------------------------------
// 再生
//-----------------------------------------------------------------------------
void SoundWork3D::Play3D(const float3& pos, DWORD delay)
{
    if (!AudioDeviceChild::g_audioDevice) return;
    if (!AudioDeviceChild::g_audioDevice->g_xAudio2) return;
    if (!m_pSourceVoice) return;

    SetEmitter(pos);

    SoundWork::Play(delay);
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void SoundWork3D::Update()
{
    if (!AudioDeviceChild::g_audioDevice) return;
    if (!AudioDeviceChild::g_audioDevice->g_xAudio2) return;
    if (!AudioDeviceChild::g_audioDevice->g_x3DAudioInstance) return;

    // 計算フラグ設定 ※TODO: 要調査
    DWORD dwCalcFlags = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER | X3DAUDIO_CALCULATE_LPF_DIRECT |
        X3DAUDIO_CALCULATE_LPF_REVERB | X3DAUDIO_CALCULATE_REVERB;

    // DSP設定の計算
    X3DAudioCalculate(g_audioDevice->g_x3DAudioInstance, &g_audioDevice->m_listener,
        &m_emitter, dwCalcFlags, &m_dspSettings);

    if (m_pSourceVoice)
    {
        // X3DAudioが生成したDSP設定をXAudio2に適用する
        // MasteringVoice SubmixVoice にそれぞれ送信
        m_pSourceVoice->SetFrequencyRatio(m_dspSettings.DopplerFactor);
        m_pSourceVoice->SetOutputMatrix(g_audioDevice->g_pMasteringVoice, INPUTCHANNELS, g_audioDevice->g_channels, g_audioDevice->g_matrixCoefficients);
        m_pSourceVoice->SetOutputMatrix(g_audioDevice->g_pSubmixVoice, 1, 1, &m_dspSettings.ReverbLevel);

        // この式の詳細については、XAudio2.h の XAudio2CutoffFrequencyToRadians() を参照してください。
        XAUDIO2_FILTER_PARAMETERS FilterParametersDirect = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * m_dspSettings.LPFDirectCoefficient), 1.0f };
        m_pSourceVoice->SetOutputFilterParameters(g_audioDevice->g_pMasteringVoice, &FilterParametersDirect);

        XAUDIO2_FILTER_PARAMETERS FilterParametersReverb = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * m_dspSettings.LPFReverbCoefficient), 1.0f };
        m_pSourceVoice->SetOutputFilterParameters(g_audioDevice->g_pSubmixVoice, &FilterParametersReverb);
    }
}

//-----------------------------------------------------------------------------
// エミッターの初期設定
//-----------------------------------------------------------------------------
void SoundWork3D::SetEmitter(const float3& pos)
{
    // コーン設定
    m_emitter.pCone = &m_emitterCone;
    m_emitter.pCone->InnerAngle = 0.0f;
    m_emitter.pCone->OuterAngle = 0.0f;
    m_emitter.pCone->InnerVolume = 0.0f;
    m_emitter.pCone->OuterVolume = 1.0f;
    m_emitter.pCone->InnerLPF = 0.0f;
    m_emitter.pCone->OuterLPF = 1.0f;
    m_emitter.pCone->InnerReverb = 0.0f;
    m_emitter.pCone->OuterReverb = 1.0f;

    // transform
    m_emitter.Position.x = pos.x;
    m_emitter.Position.y = pos.y;
    m_emitter.Position.z = pos.z;

    m_emitter.OrientFront.x =
    m_emitter.OrientFront.y =
    m_emitter.OrientTop.x   =
    m_emitter.OrientTop.z   = 0.f;

    m_emitter.OrientFront.z =
    m_emitter.OrientTop.y   = 1.f;

    // チャンネル情報
    m_emitter.ChannelCount  = INPUTCHANNELS;// TODO: 要調査 ステレオ...2 に設定する必要？
    m_emitter.ChannelRadius = 1.0f;
    m_emitter.pChannelAzimuths = g_audioDevice->g_emitterAzimuths;

    // 内側の半径を使うことで、よりスムーズなトランジションが可能になります。
    // 音がリスナーを直接通過したり、上や下に移動したりする際に、よりスムーズな移行が可能になります。
    // また、高さ方向の合図にも使用できます。
    m_emitter.InnerRadius = 2.0f;
    m_emitter.InnerRadiusAngle = X3DAUDIO_PI / 4.0f;

    // 各カーブ nullでDefaultのカーブを使用
    m_emitter.pVolumeCurve      = (X3DAUDIO_DISTANCE_CURVE*)&X3DAudioDefault_LinearCurve;
    m_emitter.pLFECurve         = (X3DAUDIO_DISTANCE_CURVE*)&Emitter_LFE_Curve;
    m_emitter.pLPFDirectCurve   = nullptr; // use default curve
    m_emitter.pLPFReverbCurve   = nullptr; // use default curve
    m_emitter.pReverbCurve      = (X3DAUDIO_DISTANCE_CURVE*)&Emitter_Reverb_Curve;

    // 各曲線距離スケーラー ※世界の大きさに合わせる必要あり
    m_emitter.CurveDistanceScaler = 200.0f;
    m_emitter.DopplerScaler     = 200.0f;

    // DSP設定
    // 3D信号処理のための低レベルオーディオレンダリングAPIに送られる。
    m_dspSettings.SrcChannelCount = INPUTCHANNELS;
    m_dspSettings.DstChannelCount = g_audioDevice->g_channels;
    m_dspSettings.pMatrixCoefficients = g_audioDevice->g_matrixCoefficients;
}
