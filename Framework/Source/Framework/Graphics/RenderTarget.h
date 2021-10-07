//-----------------------------------------------------------------------------
// File: RenderTarget.h
// 
// レンダーターゲットクラス
// グラフィックスパイプラインの出力先
//-----------------------------------------------------------------------------
#pragma once
#include "GraphicsDeviceChild.h"

// レンダーターゲットクラス
class RenderTarget : public GraphicsDeviceChild
{
public:

	// @brief コンストラクタ
	RenderTarget();

	// @brief デストラクタ
	~RenderTarget();

	// @brief 作成
	// @param w 幅
	// @param h 高さ
	// @param mipLevel ミップマップのレベル 0指定で1x1まで作成
	// @param arraySize テクスチャ配列のサイズ
	// @param colorF カラーのフォーマット
	// @param dsF 深度ステンシルのフォーマット
	// @param clearColor クリア色
	// @return 成功...true
	bool Create(int w, int h, int mipLevel, int arraySize, DXGI_FORMAT colorF, DXGI_FORMAT dsF, float clearColor[4] = nullptr);

	//--------------------------------------------------
	// 取得
	//--------------------------------------------------

	// @brief レンダリングターゲットテクスチャを返す
	// @return レンダリングターゲットとなるテクスチャ
	Texture& GetRenderTargetTexture() { return m_renderTargetTexture; }

	// @brief テクスチャの幅を返す
	// @return テクスチャの幅
	int GetWidth() const { return m_width; }

	// @brief テクスチャの高さを返す
	// @return テクスチャの高さ
	int GetHeight() const { return m_height; }

	// @brief カラーバッファのフォーマットを返す
	// @return カラーバッファのフォーマット
	DXGI_FORMAT GetColorBufferFormat() const { return m_renderTargetTexture.GetDesc().Format; }

	// @brief
	// @return
	const float* GetRTVClearColor() const { return m_rtvClearColor; }

	// @brief
	// @return
	float GetDSVClearValue() const { return m_dsvClearValue; }

private:

	Texture m_renderTargetTexture;
	ID3D11Texture2D* m_renderTargetTextureDx12;
	ID3D11Texture2D* m_depthStencilTexture;

	int m_width;
	int m_height;
	float m_rtvClearColor[4];
	float m_dsvClearValue = 1.0f;

private:

	//
	bool CreateRenderTargetTexture(int w, int h, int mipLevel, int arraySize, DXGI_FORMAT format, float clearColor[4]);

	//
	bool CreateDepthStencilTexture(int w, int h, DXGI_FORMAT format);

};
