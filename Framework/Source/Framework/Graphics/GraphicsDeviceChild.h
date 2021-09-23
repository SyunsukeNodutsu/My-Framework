//-----------------------------------------------------------------------------
// File: GraphicsDeviceChild.h
//
// 継承先に単一のGraphicsDeviceへの参照を許可します
//-----------------------------------------------------------------------------
#pragma once
#include "GraphicsDevice.h"

// 描画デバイス管理の子クラス
class GraphicsDeviceChild
{
public:

    // @brief グラフィックスデバイスの設定
    // @param device 設定するデバイスソース
    static void SetGraphicsDevice(GraphicsDevice* device);

    // @brief グラフィックスデバイスを返す
    // @return グラフィックスデバイス
    static GraphicsDevice* GetGraphicsDevice() { return g_graphicsDevice; }

protected:

    // 単一のデバイスを参照
    static GraphicsDevice* g_graphicsDevice;

};
