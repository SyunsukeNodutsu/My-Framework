//-----------------------------------------------------------------------------
// File: Texture.h
//
// テクスチャを管理しやすくラップ
// DirectXTKを使用して読み込みを行っています
//-----------------------------------------------------------------------------
#pragma once
#include "GraphicsDeviceChild.h"

// テクスチャ管理
class Texture : public GraphicsDeviceChild
{
public:

	// @brief コンストラクタ
	Texture();

	// @brief デストラクタ
	~Texture() = default;

	// @brief テクスチャリソースから作成
	// @param pTexBuffer
	// @return 成功...true 失敗...false
	bool Create(ID3D11Texture2D* pTexBuffer, bool useMSAA = false);

	// @brief ファイル名から作成
	// @param filepath 作成元ファイルパス
	// @return 成功...true 失敗...false
	bool Create(const std::string& filepath, bool useMSAA = false);

	//@brief 通常テクスチャとして作成
	bool Create(int w, int h, DXGI_FORMAT format, UINT arrayCnt, const D3D11_SUBRESOURCE_DATA* fillData, bool useMSAA = false);

	// @brief DESC情報から作成
	// @param desc DESC情報
	// @param fillData バッファに書き込むデータ ※nullptrだと書き込みなし
	// @return 成功...true 失敗...false
	bool Create(const D3D11_TEXTURE2D_DESC& desc, const D3D11_SUBRESOURCE_DATA* fillData = nullptr, bool useMSAA = false);

	// @brief レンダーターゲットテクスチャとして作成
	// @note テクスチャリソースを作成し、ShaderResourceViewのみを作成します
	// @param height テクスチャ高さ
	// @param width テクスチャ幅
	// @param useMSAA MSAA機能？
	// @param format 画像の形式　DXGI_FORMATを使用
	// @param arrayCnt 「テクスチャ配列」を使用する場合、その数。1で通常の1枚テクスチャ
	// @return 成功...true 失敗...false
	bool CreateRenderTarget(int height, int width, bool useMSAA = false, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, UINT arrayCnt = 1);

	// @brief 深度テクスチャとして作成
	// @param height テクスチャ高さ
	// @param width テクスチャ幅
	// @param useMSAA MSAA機能？
	// @param format フォーマット
	// @return 成功...true 失敗...false
	bool CreateDepthStencil(int height, int width, bool useMSAA = false, DXGI_FORMAT format = DXGI_FORMAT_R24G8_TYPELESS);

	//--------------------------------------------------
	// 取得
	//--------------------------------------------------

	// @brief RTVを(ポインタ)を返す
	// @return RTV(ポインタ)
	inline ID3D11RenderTargetView* RTV() const { return m_cpRTV.Get(); }

	// @brief RTV(ダブルポインタ)を返す
	// @return RTV(ダブルポインタ)
	inline ID3D11RenderTargetView* const* RTVAddress() const { return m_cpRTV.GetAddressOf(); }

	// @brief SRV(ポインタ)を返す
	// @return SRV(ポインタ)
	inline ID3D11ShaderResourceView* SRV() const { return m_cpSRV.Get(); }

	// @brief SRV(ダブルポインタ)を返す
	// @return SRV(ダブルポインタ)
	inline ID3D11ShaderResourceView* const * SRVAddress() const { return m_cpSRV.GetAddressOf(); }

	// @brief DSV(ポインタ)を返す
	// @return DSV(ポインタ)
	inline ID3D11DepthStencilView* DSV() const { return m_cpDSV.Get(); }

	// @brief リソースを返す
	// @return リソース
	ID3D11Texture2D* GetResource() const;

	// @brief 画像の全情報を返す
	// @return desc情報
	const D3D11_TEXTURE2D_DESC& GetDesc() const { return m_desc; }

private:

	ComPtr<ID3D11Texture2D>				m_cpBuffer;	// 作成されたテクスチャバッファ
	D3D11_TEXTURE2D_DESC				m_desc;		// テクスチャ情報
	ComPtr<ID3D11RenderTargetView>		m_cpRTV;	// RTV
	ComPtr<ID3D11ShaderResourceView>	m_cpSRV;	// SRV
	ComPtr<ID3D11DepthStencilView>		m_cpDSV;	// DSV 深度バッファ書き込み用

private:

	// @brief RTVの作成
	// @return 成功...true 失敗...false
	bool CreateRTV(bool useMSAA = false);

	// @brief DSVの作成
	// @return 成功...true 失敗...false
	bool CreateDSV(bool useMSAA = false);

	//2D画像(resource)リソースから、最適なビューを作成する
	//@param resource
	//@param ppSRV
	//@param ppRTV
	//@param ppDSV
	//@param useMSAA MSAA有効？
	bool KdCreateViewsFromTexture2D(ID3D11Texture2D* resource, ID3D11ShaderResourceView** ppSRV, ID3D11RenderTargetView** ppRTV, ID3D11DepthStencilView** ppDSV, bool useMSAA);

};
