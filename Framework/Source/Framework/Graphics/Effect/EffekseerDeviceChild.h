//-----------------------------------------------------------------------------
// File: EffekseerDeviceChild.h
//
// 継承先に単一のEffekseerDeviceへの参照を許可します
//-----------------------------------------------------------------------------
#pragma once
#include "EffekseerDevice.h"

// エフェクトデバイス管理の子クラス
class EffekseerDeviceChild
{
public:

	// @brief エフェクトデバイスの設定
	// @param device 設定するデバイスソース
	static void SetEffekseerDevice(EffekseerDevice* device);

protected:

	// 単一のデバイスを参照
	static EffekseerDevice* g_effectDevice;

};
