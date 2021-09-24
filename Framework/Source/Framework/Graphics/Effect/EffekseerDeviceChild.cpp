#include "EffekseerDeviceChild.h"

EffekseerDevice* EffekseerDeviceChild::g_effectDevice = nullptr;

//-----------------------------------------------------------------------------
// エフェクトデバイスの設定
//-----------------------------------------------------------------------------
void EffekseerDeviceChild::SetEffekseerDevice(EffekseerDevice* device)
{
	if (!device) return;
	g_effectDevice = device;
}
