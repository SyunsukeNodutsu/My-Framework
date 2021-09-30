#include "Renderer.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Renderer::Renderer()
	: m_samplerStates()
	, m_rasterizerState()
	, m_depthStencilStates()
	, m_blendStates()
	, m_saveState()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool Renderer::Initialize()
{
	// コンスタントバッファを作成
	bool result = false;
	result = m_cb7ShaderDebug.Create();
	result = m_cb8WorldMatrix.Create();
	result = m_cb9Camera.Create();
	result = m_cb10Light.Create();
	result = m_cb12Time.Create();
	result = m_cb13Atmosphere.Create();

	// 指定スロットにバッファを設定
	m_cb7ShaderDebug.SetToDevice(use_slot_shader_debug);
	m_cb8WorldMatrix.SetToDevice(use_slot_world_matrix);
	m_cb9Camera.SetToDevice(use_slot_camera);
	m_cb10Light.SetToDevice(use_slot_light);
	m_cb12Time.SetToDevice(use_slot_time);
	m_cb13Atmosphere.SetToDevice(use_slot_atmosphere);

	m_cb7ShaderDebug.Work().g_show_base_color = 0;
	m_cb7ShaderDebug.Work().g_show_normal = 0;
	m_cb7ShaderDebug.Write();

	m_cb10Light.Work().m_ambient_power = 0.6f;
	m_cb10Light.Work().m_directional_light_dir = float3(1, -1, 0);
	m_cb10Light.Work().m_directional_light_dir.Normalize();
	m_cb10Light.Work().m_enable = true;
	m_cb10Light.Write();

	// 大気のパラメータを設定
	m_cb13Atmosphere.Work().m_distance_fog_enable = 1;
	m_cb13Atmosphere.Work().m_distance_fog_color = float3(0.5f, 0.6f, 0.7f);
	m_cb13Atmosphere.Work().m_distance_fog_rate = 0.004f;

	m_cb13Atmosphere.Work().m_height_fog_enable = 0;
	m_cb13Atmosphere.Work().m_height_fog_color = float3(0.5f, 0.6f, 0.7f);

	m_cb13Atmosphere.Work().m_mie_streuung_enable= 0;
	m_cb13Atmosphere.Work().m_mie_streuung_factor_coefficient = -0.999f;

	m_cb13Atmosphere.Write();

	// 初期設定
	SetDefaultState();

	m_cb8WorldMatrix.Work().m_world_matrix = mfloat4x4::Identity;
	m_cb8WorldMatrix.Work().m_dither_enable = false;
	m_cb8WorldMatrix.Write();

	// カメラ
	mfloat4x4 viewMatrix, projMatrix;
	viewMatrix = mfloat4x4::CreateTranslation(0.0f, 0.0f, 0.0f);
	projMatrix = DirectX::XMMatrixPerspectiveFovLH(60.0f * ToRadians, 16.0f / 9.0f, 0.01f, 2000.0f);
	m_cb9Camera.Work().m_view_matrix = viewMatrix;
	m_cb9Camera.Work().m_proj_matrix = projMatrix;
	m_cb9Camera.Write();

	return true;
}

//-----------------------------------------------------------------------------
// デフォルト設定
//-----------------------------------------------------------------------------
void Renderer::SetDefaultState()
{
	SetDepthStencil(true, true);
	SetSampler(SS_FilterMode::eAniso, SS_AddressMode::eWrap);
	SetBlend(BlendMode::eAlpha);
	SetRasterize(RS_CullMode::eBack, RS_FillMode::eSolid);
}

//-----------------------------------------------------------------------------
// テクスチャ設定
//-----------------------------------------------------------------------------
void Renderer::SetTexture(Texture* texture, int slot)
{
	if (!g_graphicsDevice) return;
	if (!g_graphicsDevice->g_cpContext) return;
	if (!texture) return;

	g_graphicsDevice->g_cpContext.Get()->VSSetShaderResources(slot, 1, texture->SRVAddress());
	g_graphicsDevice->g_cpContext.Get()->PSSetShaderResources(slot, 1, texture->SRVAddress());
}

//-----------------------------------------------------------------------------
// デプスステンシルステート設定
//-----------------------------------------------------------------------------
void Renderer::SetDepthStencil(bool zUse, bool zWrite)
{
	if (!g_graphicsDevice) return;
	if (!g_graphicsDevice->g_cpContext) return;

	int useIdx = -1;
	if ( zUse &&  zWrite) useIdx = 0;// Zバッファ使用   深度書き込みON
	if ( zUse && !zWrite) useIdx = 1;// Zバッファ使用   深度書き込みOFF
	if (!zUse &&  zWrite) useIdx = 2;// Zバッファ不使用 深度書き込みON
	if (!zUse && !zWrite) useIdx = 3;// Zバッファ不使用 深度書き込みOFF

	if (useIdx < 0)
		return;

	// まだ作成されていない
	if (m_depthStencilStates[useIdx] == nullptr)
		m_depthStencilStates[useIdx] = CreateDepthStencil(zUse, zWrite);

	if (m_depthStencilStates[useIdx] == nullptr)
		return;

	// 設定
	g_graphicsDevice->g_cpContext.Get()->OMSetDepthStencilState(m_depthStencilStates[useIdx].Get(), 0);
	// 記憶/復元用に保存
	g_graphicsDevice->g_cpContext.Get()->OMGetDepthStencilState(&m_saveState.DS, &m_saveState.StencilRef);
}

//-----------------------------------------------------------------------------
// サンプラーステート設定
//-----------------------------------------------------------------------------
bool Renderer::SetSampler(SS_FilterMode filter, SS_AddressMode address)
{
	if (!g_graphicsDevice) return false;
	if (!g_graphicsDevice->g_cpContext) return false;

	// MAP確認 新規作成
	if (m_samplerStates.find(filter | address) == m_samplerStates.end())
	{
		if (!CreateSampler(filter, address))
			return false;
	}

	// PSとVSにサンプラーステートを設定
	g_graphicsDevice->g_cpContext.Get()->VSSetSamplers(0, 1, m_samplerStates[filter | address].GetAddressOf());
	g_graphicsDevice->g_cpContext.Get()->PSSetSamplers(0, 1, m_samplerStates[filter | address].GetAddressOf());
	// 記憶/復元用に保存
	g_graphicsDevice->g_cpContext.Get()->PSGetSamplers(0, 1, &m_saveState.SS);

	return true;
}

//-----------------------------------------------------------------------------
// ブレンドモード設定
//-----------------------------------------------------------------------------
bool Renderer::SetBlend(BlendMode flag)
{
	if (!g_graphicsDevice) return false;
	if (!g_graphicsDevice->g_cpContext) return false;

	if (m_blendStates[flag] == nullptr)
		m_blendStates[flag] = CreateBlend(flag);

	if (m_blendStates[flag] == nullptr)
		return false;

	g_graphicsDevice->g_cpContext.Get()->OMSetBlendState(m_blendStates[flag].Get(), cfloat4x4::Clear, 0xFFFFFFFF);
	// 記憶/復元用に保存
	g_graphicsDevice->g_cpContext.Get()->OMGetBlendState(&m_saveState.BS, m_saveState.BlendFactor, &m_saveState.SampleMask);

	return true;
}

//-----------------------------------------------------------------------------
// ラスタライザー設定
//-----------------------------------------------------------------------------
bool Renderer::SetRasterize(RS_CullMode cull, RS_FillMode fill)
{
	if (!g_graphicsDevice) return false;
	if (!g_graphicsDevice->g_cpContext) return false;

	if (m_rasterizerState.find(cull| fill) == m_rasterizerState.end())
	{
		if (!CreateRasterrize(cull, fill))
			return false;
	}

	g_graphicsDevice->g_cpContext.Get()->RSSetState(m_rasterizerState[(cull | fill)].Get());
	// 記憶/復元用に保存
	g_graphicsDevice->g_cpContext.Get()->RSGetState(&m_saveState.RS);

	return true;
}

//-----------------------------------------------------------------------------
// サンプラーステート作成
//-----------------------------------------------------------------------------
bool Renderer::CreateSampler(SS_FilterMode filter, SS_AddressMode address)
{
	if (!g_graphicsDevice) return false;
	if (!g_graphicsDevice->g_cpContext) return false;

	D3D11_SAMPLER_DESC desc = {};
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	// フィルタリングモード
	switch (filter)
	{
	case SS_FilterMode::eLinear:
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;

	case SS_FilterMode::ePoint:
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		break;

	case SS_FilterMode::eAniso:
		desc.Filter = D3D11_FILTER_ANISOTROPIC;
		desc.MaxAnisotropy = 4;
		break;
	}

	// アドレッシングモード
	switch (address)
	{
	case SS_AddressMode::eWrap:
		desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		break;

	case SS_AddressMode::eMirror:
		desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
		break;

	case SS_AddressMode::eClamp:
		desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	}

	// 作成
	ComPtr<ID3D11SamplerState> state = nullptr;
	HRESULT hr = g_graphicsDevice->g_cpDevice.Get()->CreateSamplerState(&desc, state.GetAddressOf());
	if (FAILED(hr)) {
		assert(0 && "エラー：サンプラーステート作成失敗");
		return false;
	}

	// 登録(追加)
	m_samplerStates[(filter | address)] = state;

	return true;
}

//-----------------------------------------------------------------------------
// ラスタライザーステート作成
//-----------------------------------------------------------------------------
bool Renderer::CreateRasterrize(RS_CullMode cull, RS_FillMode fill)
{
	if (!g_graphicsDevice) return false;
	if (!g_graphicsDevice->g_cpContext) return false;

	D3D11_RASTERIZER_DESC desc = {};

	// カリングモード
	switch (cull)
	{
	case RS_CullMode::eCullNone: desc.CullMode = D3D11_CULL_NONE;  break;
	case RS_CullMode::eFront:	 desc.CullMode = D3D11_CULL_FRONT; break;
	case RS_CullMode::eBack:	 desc.CullMode = D3D11_CULL_BACK;  break;
	}
	// 塗りつぶし設定
	switch (fill)
	{
	case RS_FillMode::eSolid: desc.FillMode = D3D11_FILL_SOLID;		 break;
	case RS_FillMode::eWire:  desc.FillMode = D3D11_FILL_WIREFRAME;  break;
	}

	desc.FrontCounterClockwise	= FALSE;
	desc.DepthBias				= 0;
	desc.DepthBiasClamp			= 0;
	desc.SlopeScaledDepthBias	= 0;
	desc.DepthClipEnable		= TRUE;
	desc.ScissorEnable			= FALSE;
	desc.MultisampleEnable		= TRUE;
	desc.AntialiasedLineEnable	= FALSE;

	// 作成
	ComPtr<ID3D11RasterizerState> state = nullptr;
	HRESULT hr = g_graphicsDevice->g_cpDevice.Get()->CreateRasterizerState(&desc, state.GetAddressOf());
	if (FAILED(hr)) {
		assert(0 && "エラー：ラスタライザーステートの作成に失敗しました.");
		return false;
	}

	m_rasterizerState[(cull | fill)] = state;

	return true;
}

//-----------------------------------------------------------------------------
// デプスステンシルステート作成
//-----------------------------------------------------------------------------
ComPtr<ID3D11DepthStencilState> Renderer::CreateDepthStencil(bool zUse, bool zWrite)
{
	if (!g_graphicsDevice) return nullptr;
	if (!g_graphicsDevice->g_cpContext) return nullptr;

	D3D11_DEPTH_STENCIL_DESC desc = {};

	// 深度テスト
	desc.DepthEnable		= zUse;
	desc.DepthFunc			= D3D11_COMPARISON_LESS_EQUAL;// 判定方法

	// 深度書き込み
	desc.DepthWriteMask		= zWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;

	// Stencil使用せず
	desc.StencilEnable		= FALSE;
	desc.StencilReadMask	= 0;
	desc.StencilWriteMask	= 0;

	// 作成
	ComPtr<ID3D11DepthStencilState> state = nullptr;
	HRESULT hr = g_graphicsDevice->g_cpDevice.Get()->CreateDepthStencilState(&desc, &state);
	if (FAILED(hr)) {
		assert(0 && "エラー：デプスステンシルステート作成失敗.");
		return nullptr;
	}

	return state;
}

//-----------------------------------------------------------------------------
// ブレンドステート作成
//-----------------------------------------------------------------------------
ComPtr<ID3D11BlendState> Renderer::CreateBlend(BlendMode flag)
{
	if (!g_graphicsDevice) return nullptr;
	if (!g_graphicsDevice->g_cpContext) return nullptr;

	D3D11_BLEND_DESC desc = {};
	desc.RenderTarget[0].BlendEnable			= TRUE;
	// 書き込みマスク ALL...RGBA全て出力
	desc.RenderTarget[0].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;
	desc.IndependentBlendEnable					= FALSE;
	desc.AlphaToCoverageEnable					= FALSE;

	switch (flag)
	{
	case BlendMode::eAlpha:
		// 色の合成方法
		desc.RenderTarget[0].BlendOp	= D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend	= D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend	= D3D11_BLEND_INV_SRC_ALPHA;
		// アルファの合成方法
		desc.RenderTarget[0].BlendOpAlpha	= D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha	= D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		break;

	case BlendMode::eAdd:// 加算ブレンド
		desc.RenderTarget[0].BlendOp	= D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend	= D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend	= D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha	= D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha	= D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		break;

	case BlendMode::eBlendNone:// ブレンドしない
		desc.RenderTarget[0].BlendOp	= D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend	= D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend	= D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha	= D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha	= D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		break;
	}

	// 作成
	ComPtr<ID3D11BlendState> state = nullptr;
	HRESULT hr = g_graphicsDevice->g_cpDevice.Get()->CreateBlendState(&desc, state.GetAddressOf());
	if (FAILED(hr)) {
		assert(0 && "エラー：ブレンドステート作成失敗.");
		return nullptr;
	}

	return state;
}
