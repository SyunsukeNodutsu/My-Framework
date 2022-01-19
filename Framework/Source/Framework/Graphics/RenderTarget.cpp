#include "RenderTarget.h"

RestoreRenderTarget::RestoreRenderTarget()
{
	g_graphicsDevice->g_cpContext->OMGetRenderTargets(1, &m_pSaveRT, &m_pSaveZ);
}

RestoreRenderTarget::~RestoreRenderTarget()
{
	g_graphicsDevice->g_cpContext->OMSetRenderTargets(1, &m_pSaveRT, m_pSaveZ);

	// 解放
	if (m_pSaveRT) m_pSaveRT->Release();
	if (m_pSaveZ)  m_pSaveZ->Release();
}
