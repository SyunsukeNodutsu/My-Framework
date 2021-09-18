#include "GraphicsDeviceChild.h"

GraphicsDevice* GraphicsDeviceChild::m_graphicsDevice = nullptr;

//-----------------------------------------------------------------------------
// グラフィックスデバイスの設定
//-----------------------------------------------------------------------------
void GraphicsDeviceChild::SetGraphicsDevice(GraphicsDevice* device)
{
	if (device == nullptr)
		return;

	m_graphicsDevice = device;
	// TODO: ここで各初期化 想定
}
