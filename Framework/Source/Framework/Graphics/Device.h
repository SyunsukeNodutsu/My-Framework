//-----------------------------------------------------------------------------
// File: Device.h
//
// DirectX関連の管理
//-----------------------------------------------------------------------------
#pragma once

class Renderer;
class Texture;

// Direct3Dの操作 管理
class Device
{
public:

	// @brief デバイス.スワップチェインの作成
	// @param hWnd ウィンドウハンドル
	// @param isUseHDR HDRの使用
	// @param width 解像度(横)s
	// @param height 解像度(縦)
	// @param isFullScreen フルスクリーンか
	// @param isDebug デバッグモードで起動
	// @param resultErrMsg エラーメッセージを格納
	// @return 成功...true 失敗...false
	bool Create( const HWND& hWnd, bool isUseHDR, int width, int height, bool isFullScreen, bool isDebug, std::string& resultErrMsg );

	// @brief 解放
	void Release();

	// @brief 描画開始とバッファクリア
	// @param clearColor クリア色
	void Begin( const float* clearColor = nullptr );

	// @brief 描画終了と画面更新
	void End();

	//--------------------------------------------------
	// 取得
	//--------------------------------------------------

	// @brief インスタンスを返す
	// @return nz3DDevice インスタンス
	static Device& GetInstance() { static Device instance; return instance; }

	// @brief デバイスを返す
	// @return デバイス ID3D11Device
	ID3D11Device* GetDevice() const { return m_cpDevice.Get(); }

	// @brief 即時コンテキストを返す
	// @return 即時デバイスコンテキスト ID3D11DeviceContext
	ID3D11DeviceContext* GetDeviceContext() const { return m_cpImmediateContext.Get(); }

	// @brief スワップチェイン返す
	// @return スワップチェイン IDXGISwapChain
	ComPtr<IDXGISwapChain> GetSwapChain() const { return m_cpGISwapChain; }

	// @brief 描画状態管理を返す
	// @return 描画状態管理クラス
	Renderer& GetRenderer() { return *m_spRenderer; }

	// @brief アダプタ名(グラフィックカード名)を返す
	// @return アダプタ名
	std::wstring GetAdapterName() { return m_adapterName; }

	// 1x1 白テクスチャ取得
	const std::shared_ptr<Texture>	GetWhiteTex() const { return m_texWhite; }

	// 1x1 Z向き法線マップ取得
	const std::shared_ptr<Texture>	GetNormalTex() const { return m_texNormal; }

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------

	// @brief 垂直同期の使用を設定
	// @param onoff 使用の設定
	void SetUseVerticalSync(bool onoff) { m_useVerticalSync = onoff; }

private:

	ComPtr<ID3D11Device>			m_cpDevice;				// デバイス
	ComPtr<ID3D11DeviceContext>		m_cpImmediateContext;	// 即時デバイスコンテキスト
	ComPtr<IDXGISwapChain>			m_cpGISwapChain;		// スワップチェイン

	// 読み込み用
	ComPtr<ID3D11DeviceContext>		m_cpDefferedContext;	// 遅延デバイスコンテキスト
	ComPtr<ID3D11CommandList>		m_cpCommandList;		// コマンドリスト

	std::shared_ptr<Renderer>		m_spRenderer;			// 描画設定
	std::shared_ptr<Texture>		m_spBackbuffer;			// バックバッファ用テクスチャ
	std::shared_ptr<Texture>		m_spDefaultZbuffer;		// デフォルトZバッファ用テクスチャ

	std::wstring					m_adapterName;			// アダプタ名
	bool							m_useVerticalSync;		// 垂直同期？ ※Present(1, 0)
	bool							m_useMSAA;				// MSAA？

	//------------------------
	// 便利テクスチャ
	//------------------------
	std::shared_ptr<Texture> m_texWhite;	// 1x1 白テクスチャ
	std::shared_ptr<Texture> m_texNormal;	// デフォルト法線マップ（1x1 Z向き）

private:

	// @brief コンストラクタ
	Device();

	// @brief デストラクタ
	~Device() = default;

};

//-----------------------------------------------------------------------------
// define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define D3D Device::GetInstance()
