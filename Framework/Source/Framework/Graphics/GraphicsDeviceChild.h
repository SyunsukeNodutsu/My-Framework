//-----------------------------------------------------------------------------
// File: GameSystem.h
//
// Device DeviceContext ...
// 描画デバイスの処理を行うクラスに継承
//-----------------------------------------------------------------------------
#pragma once

// TODO:
class GraphicsDevice{};

// 描画デバイス管理の子クラス
class GraphicsDeviceChild
{
public:

    // @brief グラフィックスデバイスの設定
    // @param device 設定するデバイスソース
    static void SetGraphicsDevice(GraphicsDevice* device);

    // @brief グラフィックスデバイスを返す
    // @return グラフィックスデバイス
    static GraphicsDevice* GetGraphicsDevice() { return m_graphicsDevice; }

protected:

    // 単一のデバイスを参照
    static GraphicsDevice* m_graphicsDevice;

};
