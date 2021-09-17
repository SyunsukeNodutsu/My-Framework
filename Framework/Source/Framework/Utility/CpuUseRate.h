//-----------------------------------------------------------------------------
// File: CCpuUseRate.h
//
// HクエリからCPU使用率の取得を行う
//-----------------------------------------------------------------------------
#pragma once
#include <pdh.h>    // Pdh.Lib
#pragma comment(lib, "pdh.lib")

// CPU使用率の取得クラス
class CpuUseRate
{
public:

    // @brief コンストラクタ
    CpuUseRate();

    // @brief デストラクタ
    ~CpuUseRate();

    // @brief 使用率をパーセンテージで返す
    // @return CPU使用率
    double GetCpuUseRate();

protected:

    HQUERY          m_hQuery;
    HCOUNTER        m_hCounter;
    PDH_RAW_COUNTER m_RawValueStack;

};
