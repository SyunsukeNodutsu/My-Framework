#include "CpuUseRate.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
CpuUseRate::CpuUseRate()
{
    // 新規クエリーを作成
    PdhOpenQuery(NULL, 0, &m_hQuery);
    // 現在のCPU％を取得
    PdhAddCounter(m_hQuery, L"\\Processor(_Total)\\% Processor Time", 0, &m_hCounter);
    PdhGetRawCounterValue(m_hCounter, NULL, &m_RawValueStack);
}

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
CpuUseRate::~CpuUseRate()
{
    // 終了
    PdhCloseQuery(m_hQuery);
}

//-----------------------------------------------------------------------------
// CPU使用率取得
//-----------------------------------------------------------------------------
double CpuUseRate::GetCpuUseRate()
{
    PDH_RAW_COUNTER	RawValue;
    PDH_FMT_COUNTERVALUE FmtValue;

    // 計測
    PdhCollectQueryData(m_hQuery);

    // 現在のカウンタの値を取得
    PdhGetRawCounterValue(m_hCounter, NULL, &RawValue);
    PdhCalculateCounterFromRawValue(
        m_hCounter,
        PDH_FMT_DOUBLE,
        &RawValue,
        &m_RawValueStack,
        &FmtValue
    );
    m_RawValueStack = RawValue;

    return FmtValue.doubleValue;
}
