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
	bool m_debugMode;

	HWND m_hwnd;
};

// Direct3D11 描画デバイスクラス
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
	void Begin(ID3D11DeviceContext* pd3dContext, const float* clearColor = nullptr);

	// @brief 描画終了と画面更新
	// @param syncInterval フレームの表示を垂直ブランクと同期する方法
	// @param flags スワップチェーン表示オプション
	void End(UINT syncInterval = 0, UINT flags = 0);

	// @brief 頂点を描画する簡易的な関数
	// @param pd3dContext 描画コンテキスト
	// @param topology 頂点をどのような形状で描画するか
	// @param vertexCount 頂点数
	// @param pVertexStream 頂点配列の先頭アドレス
	// @param stride 1頂点のバイトサイズ
	void DrawVertices(ID3D11DeviceContext* pd3dContext, D3D_PRIMITIVE_TOPOLOGY topology, int vertexCount, const void* pVertexStream, UINT stride);

	//--------------------------------------------------
	// 取得
	//--------------------------------------------------

	// @brief システムのバックバッファ(RT)を返す
	// @return バックバッファ(RT)
	const std::shared_ptr<Texture>& GetBackbuffer() const { return m_spBackbuffer; }

	// @brief 1x1 白色テクスチャを返す
	// @return デフォルトの白色テクスチャ
	std::shared_ptr<Texture> GetWhiteTex() { return m_texWhite; }

	// @brief 1x1 Z向きテクスチャを返す
	// @return デフォルトの法線マップ
	std::shared_ptr<Texture> GetNormalTex() { return m_texNormal; }

	// @brief 使用しているアダプタ名を返す
	// @return GPUアダプタ名
	const std::string& GetAdapterName() const { return m_adapterName; }

	// @brief リソースのマルチサンプリングパラメータを返す
	// @return マルチサンプリングパラメータ
	const DXGI_SAMPLE_DESC& GetSampleDesc() const { return m_sampleDesc; }

	//
	const ComPtr<IDXGISwapChain> GetSwapChain() const { return m_cpGISwapChain; }

public:

	// レンダリング インターフェース
	ComPtr<ID3D11Device>			g_cpDevice;
	ComPtr<ID3D11DeviceContext>		g_cpContext; // 即時コンテキスト
	ComPtr<ID3D11DeviceContext>		g_cpContextDeferred;// 遅延コンテキスト
	ComPtr<ID3D11CommandList>		g_cpCommandList;//コマンドリスト

	D3D11_VIEWPORT g_viewport;

private:

	// スワップチェイン
	ComPtr<IDXGISwapChain>			m_cpGISwapChain;

	// バックバッファ
	std::shared_ptr<Texture>		m_spBackbuffer;
	std::shared_ptr<Texture>		m_spDefaultZbuffer;

	// アダプタ名
	std::string						m_adapterName;

	// サンプル情報
	DXGI_SAMPLE_DESC				m_sampleDesc;

	// DrawVertices用 頂点バッファ
	std::shared_ptr<Buffer> m_tempFixedVertexBuffer[10];	// 固定長 頂点バッファ
	std::shared_ptr<Buffer> m_tempVertexBuffer;				// 可変長 頂点バッファ

	//------------------------
	// 便利テクスチャ
	//------------------------
	std::shared_ptr<Texture>		m_texWhite;	// 1x1 白テクスチャ
	std::shared_ptr<Texture>		m_texNormal;// デフォルト法線マップ（1x1 Z向き）

};
