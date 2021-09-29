//-----------------------------------------------------------------------------
// File: Device.h
//
// Direct3D管理
// TODO: クエリ関連の取得
//-----------------------------------------------------------------------------
#pragma once

class Texture;
class Buffer;

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

	// @brief 初期化
	// @param desc 初期化データ
	// @return 成功...true
	bool Initialize(MY_DIRECT3D_DESC desc);

	// @brief 終了
	void Finalize();

	// @brief 描画開始とバッファクリア
	// @param clearColor クリア色(nullで白クリア)
	void Begin(const float* clearColor = nullptr);

	// @brief 描画終了と画面更新
	// @param syncInterval
	// @param flags
	void End(UINT syncInterval = 0, UINT flags = 0);

	// @brief 1x1 白色テクスチャを返す
	// @return デフォルトの白色テクスチャ
	std::shared_ptr<Texture> GetWhiteTex() { return m_texWhite; }

	// @brief 1x1 Z向きテクスチャを返す
	// @return デフォルトの法線マップ
	std::shared_ptr<Texture> GetNormalTex() { return m_texNormal; }

	// @brief 使用しているアダプタ名を返す
	// @return GPUアダプタ名
	const std::string GetAdapterName() const { return m_adapterName; }

	// @brief 頂点を描画する簡易的な関数
	// @param topology 頂点をどのような形状で描画するか
	// @param vertexCount 頂点数
	// @param pVertexStream 頂点配列の先頭アドレス
	// @param stride 1頂点のバイトサイズ
	void DrawVertices(D3D_PRIMITIVE_TOPOLOGY topology, int vertexCount, const void* pVertexStream, UINT stride);

public:

	// レンダリング インターフェース
	ComPtr<ID3D11Device>			g_cpDevice;
	ComPtr<ID3D11DeviceContext>		g_cpContext; // 即時コンテキスト

	// 11.1
	ComPtr<ID3D11Device1>			g_cpDeviceN;
	ComPtr<ID3D11DeviceContext1>	g_cpContextN;

	// 遅延コンテキスト
	ComPtr<ID3D11DeviceContext>		g_cpContextDeferred;

private:

	// スワップチェイン
	ComPtr<IDXGISwapChain>			m_cpGISwapChain;

	// コマンドリスト
	ComPtr<ID3D11CommandList>		m_cpCommandList;

	// バックバッファ
	std::shared_ptr<Texture>		m_spBackbuffer;
	std::shared_ptr<Texture>		m_spDefaultZbuffer;

	// アダプタ名
	std::string						m_adapterName;

	// DrawVertices用 頂点バッファ
	std::shared_ptr<Buffer> m_tempFixedVertexBuffer[10];	// 固定長 頂点バッファ
	std::shared_ptr<Buffer> m_tempVertexBuffer;				// 可変長 頂点バッファ

	//------------------------
	// 便利テクスチャ
	//------------------------
	std::shared_ptr<Texture>		m_texWhite;	// 1x1 白テクスチャ
	std::shared_ptr<Texture>		m_texNormal;// デフォルト法線マップ（1x1 Z向き）

};
