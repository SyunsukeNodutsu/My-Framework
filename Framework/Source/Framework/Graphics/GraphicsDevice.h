﻿//-----------------------------------------------------------------------------
// File: Device.h
//
// Direct3D関連の管理
// GraphicsDevice
// -GraphicsDeviceChild
//  -Texture, Shader, RenderTarget など
// 
// TODO: クエリ関連の取得
//-----------------------------------------------------------------------------
#pragma once

class Texture;

// 設定データ
struct MY_DIRECT3D_DESC
{
	int m_bufferCount;
	int m_width, m_height;
	int m_refreshRate;

	bool m_windowed;
	bool m_useHDR;
	bool m_useMSAA;
	bool m_deferredRendering;

	HWND m_hwnd;
};

// Direct3D
class GraphicsDevice
{
public:

	// @brief コンストラクタ
	GraphicsDevice();

	// @brief デバイス作成
	// @param desc 初期化データ
	// @return 成功...true
	bool Create(MY_DIRECT3D_DESC desc);

	// @brief 解放
	void Release();

	// @brief 描画開始とバッファクリア
	// @param clearColor クリア色(nullで白クリア)
	void Begin(const float* clearColor = nullptr);

	// @brief 描画終了と画面更新
	void End(UINT syncInterval = 0, UINT flags = 0);

public:

	// レンダリング インターフェース
	ComPtr<ID3D11Device>			g_cpDevice;
	ComPtr<ID3D11DeviceContext>		g_cpImmediateContext;

	// 11.1
	ComPtr<ID3D11Device1>			g_cpDeviceN;
	ComPtr<ID3D11DeviceContext1>	g_cpImmediateContextM;

	// 遅延コンテキスト
	ComPtr<ID3D11DeviceContext>		g_cpDeferredContext;

private:

	// スワップチェイン
	ComPtr<IDXGISwapChain>			m_cpGISwapChain;

	// コマンドリスト
	ComPtr<ID3D11CommandList>		m_cpCommandList;

	// バックバッファ
	std::shared_ptr<Texture>		m_spBackbuffer;
	std::shared_ptr<Texture>		m_spDefaultZbuffer;

	// アダプタ名
	std::wstring					m_adapterName;

	//------------------------
	// 便利テクスチャ
	//------------------------
	std::shared_ptr<Texture>		m_texWhite;	// 1x1 白テクスチャ
	std::shared_ptr<Texture>		m_texNormal;// デフォルト法線マップ（1x1 Z向き）

};