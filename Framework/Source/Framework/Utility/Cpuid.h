//-----------------------------------------------------------------------------
// File: Cpuid.h
//
// CPUID情報取得
// https://gist.github.com/t-mat/3769328
// https://docs.microsoft.com/ja-jp/cpp/intrinsics/cpuid-cpuidex?view=msvc-160
//-----------------------------------------------------------------------------
#pragma once

#if defined(_MSC_VER)// Windows
#include <intrin.h>
static void get_cpuid(void* p, int i) { __cpuid((int*)p, i); }

#elif defined(__GNUC__)// Linux
#include <cpuid.h>
static void get_cpuid(void* p, int i) { int* a = (int*)p; __cpuid(i, a[0], a[1], a[2], a[3]); }
#endif

struct CpuInfo
{
    // メンバーの順番は変えないでください
    uint32_t eax, ebx, ecx, edx;

    // コンストラクタ
    CpuInfo(int infoType)
        : eax(0)
        , ebx(0)
        , ecx(0)
        , edx(0)
    {
        get_cpuid(&eax, infoType);
    }

};

namespace Cpuid
{
    // CPU名
    std::string m_brand = "";

    // CPU名取得
    void Research()
    {
        char brand[16 * 3 + 1] = { 0 };
        for (int i = 0; i < 3; ++i)
        {
            CpuInfo e(i + 0x80000002);
            *(uint32_t*)&brand[16 * i + 4 * 0] = e.eax;
            *(uint32_t*)&brand[16 * i + 4 * 1] = e.ebx;
            *(uint32_t*)&brand[16 * i + 4 * 2] = e.ecx;
            *(uint32_t*)&brand[16 * i + 4 * 3] = e.edx;
        };
        brand[sizeof(brand) - 1] = 0;
        m_brand = brand;
    }

};
