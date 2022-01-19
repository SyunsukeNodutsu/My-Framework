//-----------------------------------------------------------------------------
// File: RenderTarget.h
// 
// 
//-----------------------------------------------------------------------------
#pragma once
#include "GraphicsDeviceChild.h"

// レンダーターゲットを自動で元に戻す便利クラス
class RestoreRenderTarget : public GraphicsDeviceChild
{
public:
	RestoreRenderTarget();
	~RestoreRenderTarget();

private:
	ID3D11RenderTargetView* m_pSaveRT = nullptr;
	ID3D11DepthStencilView* m_pSaveZ = nullptr;
};
