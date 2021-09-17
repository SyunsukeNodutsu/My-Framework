#include "AudioDeviceChild.h"

AudioDevice* AudioDeviceChild::g_audioDevice = nullptr;

//-----------------------------------------------------------------------------
// オーディオデバイスの設定
//-----------------------------------------------------------------------------
void AudioDeviceChild::SetAudioDevice(AudioDevice* device)
{
	if (device == nullptr)
		return;
	g_audioDevice = device;
}
