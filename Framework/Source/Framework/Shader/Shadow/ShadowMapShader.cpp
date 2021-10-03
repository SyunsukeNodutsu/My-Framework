#include "ShadowMapShader.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ShadowMapShader::ShadowMapShader()
	: m_shadowMaps()
	, m_zBuffer()
	, m_lvpcMatrix()
	, m_cascadeAreaTable()
	, m_saveRT(nullptr)
	, m_saveZ(nullptr)
	, m_numVP(1)
	, m_saveVP()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool ShadowMapShader::Initialize()
{
	HRESULT hr = S_FALSE;

	//-------------------------------------
	// 頂点シェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "ShadowMapShader_VS.shaderinc"

		hr = g_graphicsDevice->g_cpDevice.Get()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpVS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "エラー：頂点シェーダ作成失敗.");
			return false;
		}

		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		hr = g_graphicsDevice->g_cpDevice.Get()->CreateInputLayout(&layout[0], (UINT)layout.size(), compiledBuffer, sizeof(compiledBuffer), m_cpInputLayout.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "エラー：頂点入力レイアウト作成失敗.");
			return false;
		}
	}

	//-------------------------------------
	// ピクセルシェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "ShadowMapShader_PS.shaderinc"

		hr = g_graphicsDevice->g_cpDevice.Get()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpPS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "エラー：ピクセルシェーダ作成失敗.");
			return false;
		}
	}

	//-------------------------------------
	// レンダーターゲット作成
	//-------------------------------------
	m_shadowMaps[0] = std::make_shared<Texture>();
	m_shadowMaps[1] = std::make_shared<Texture>();
	m_shadowMaps[2] = std::make_shared<Texture>();

	m_shadowMaps[0]->CreateRenderTarget(2048, 2048, false, DXGI_FORMAT_R32_FLOAT);
	m_shadowMaps[1]->CreateRenderTarget(1024, 1024, false, DXGI_FORMAT_R32_FLOAT);
	m_shadowMaps[2]->CreateRenderTarget( 512,  512, false, DXGI_FORMAT_R32_FLOAT);

	m_zBuffer[0] = std::make_shared<Texture>();
	m_zBuffer[1] = std::make_shared<Texture>();
	m_zBuffer[2] = std::make_shared<Texture>();

	m_zBuffer[0]->CreateDepthStencil(2048, 2048, false, DXGI_FORMAT_R32_TYPELESS);
	m_zBuffer[1]->CreateDepthStencil(1024, 1024, false, DXGI_FORMAT_R32_TYPELESS);
	m_zBuffer[2]->CreateDepthStencil( 512,  512, false, DXGI_FORMAT_R32_TYPELESS);

	//-------------------------------------
	// 分割エリアの設定
	//-------------------------------------
	m_cascadeAreaTable[0] =  500;
	m_cascadeAreaTable[1] = 1000;
	m_cascadeAreaTable[2] = 2000;// camera far

#pragma region Samplers
	D3D11_SAMPLER_DESC desc = {};
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	desc.MipLODBias = 0;
	desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = g_graphicsDevice->g_cpDevice.Get()->CreateSamplerState(&desc, state.GetAddressOf());
	if (FAILED(hr)) { assert(0 && "エラー：サンプラーステート作成失敗"); return false; }

	g_graphicsDevice->g_cpContext.Get()->VSSetSamplers(10, 1, state.GetAddressOf());
	g_graphicsDevice->g_cpContext.Get()->PSSetSamplers(10, 1, state.GetAddressOf());
#pragma endregion

	return true;
}

//-----------------------------------------------------------------------------
// 描画開始
//-----------------------------------------------------------------------------
void ShadowMapShader::Begin(int numShadow)
{
	if (!g_graphicsDevice) return;
	if (!g_graphicsDevice->g_cpContext) return;

	if (m_saveRT || m_saveZ) return;

	// 1枚目のシャドウ 初回のみ保存
	if (numShadow == 0)
	{
		// 現在のRTとZとViewportを記憶
		g_graphicsDevice->g_cpContext->OMGetRenderTargets(1, &m_saveRT, &m_saveZ);
		g_graphicsDevice->g_cpContext->RSGetViewports(&m_numVP, &m_saveVP);
	}

	// RTとViewportを変更
	g_graphicsDevice->g_cpContext->OMSetRenderTargets(1, m_shadowMaps[numShadow]->RTVAddress(), m_zBuffer[numShadow]->DSV());

	auto& desc = m_shadowMaps[numShadow]->GetInfo();
	D3D11_VIEWPORT vp = { 0, 0, static_cast<FLOAT>(desc.Width), static_cast<FLOAT>(desc.Height), 0, 1 };
	g_graphicsDevice->g_cpContext->RSSetViewports(1, &vp);

	// クリア
	g_graphicsDevice->g_cpContext->ClearRenderTargetView(m_shadowMaps[numShadow]->RTV(), cfloat4x4::White);
	g_graphicsDevice->g_cpContext->ClearDepthStencilView(m_zBuffer[numShadow]->DSV(), D3D11_CLEAR_DEPTH, 1, 0);

	// カメラ
	SettingLightCamera();

	// シェーダーや定数バッファをセット
	{
		g_graphicsDevice->g_cpContext.Get()->VSSetShader(m_cpVS.Get(), 0, 0);
		g_graphicsDevice->g_cpContext.Get()->IASetInputLayout(m_cpInputLayout.Get());

		g_graphicsDevice->g_cpContext.Get()->PSSetShader(m_cpPS.Get(), 0, 0);
	}
}

//-----------------------------------------------------------------------------
// 描画終了
//-----------------------------------------------------------------------------
void ShadowMapShader::End()
{
	if (!g_graphicsDevice) return;
	if (!g_graphicsDevice->g_cpContext) return;

	if (!m_saveRT || !m_saveZ) return;

	// 記憶してたRTとZとViewportを復元
	g_graphicsDevice->g_cpContext->OMSetRenderTargets(1, &m_saveRT, m_saveZ);
	g_graphicsDevice->g_cpContext->RSSetViewports(m_numVP, &m_saveVP);

	SafeRelease(m_saveRT);
	SafeRelease(m_saveZ);
}

//-----------------------------------------------------------------------------
// モデル描画
//-----------------------------------------------------------------------------
void ShadowMapShader::DrawModel(const ModelWork& model, const mfloat4x4& worldMatrix)
{
	if (!model.IsEnable()) return;

	auto& data = model.GetData();
	if (data == nullptr) return;

	// 全メッシュノードを描画
	for (auto& index : data->GetMeshNodeIndices())
	{
		auto& node = model.GetNodes()[index];
		auto& mesh = model.GetMesh(index);

		RENDERER.Getcb8().Work().m_world_matrix = node.m_worldTransform * worldMatrix;
		RENDERER.Getcb8().Write();

		// メッシュ描画
		DrawMeshDepth(mesh.get(), data->GetMaterials());
	}
}

//-----------------------------------------------------------------------------
// ライトカメラ設定
//-----------------------------------------------------------------------------
void ShadowMapShader::SettingLightCamera()
{
	auto& camera = RENDERER.Getcb9().Work().m_camera_matrix;

	mfloat4x4 viewMatrix = mfloat4x4::CreateRotationX(45 * ToRadians);
	//mfloat4x4 viewMatrix = mfloat4x4::CreateLookAt(float3(0, -10, -20), float3(0.5f, -1.0f, 0.5f), float3::Up);
	mfloat4x4 transMatrix = mfloat4x4::CreateTranslation(0, 0, -20);
	viewMatrix = transMatrix * viewMatrix;
	viewMatrix.Invert();

	// ライトの射影行列を適当に作成　50mx50mの正射影行列
	mfloat4x4 projMatrix = mfloat4x4::CreateOrthographic(50, 50, 0, 100);

	// 定数バッファをセット ※ビュー * 射影
	RENDERER.Getcb10().Work().m_directional_light_vp = viewMatrix * projMatrix;
	RENDERER.Getcb10().Write();
}

//-----------------------------------------------------------------------------
// メッシュ描画
//-----------------------------------------------------------------------------
void ShadowMapShader::DrawMeshDepth(const Mesh* mesh, const std::vector<Material>& materials)
{
	// メッシュ情報をセット
	mesh->SetToDevice();

	for (UINT subi = 0; subi < mesh->GetSubsets().size(); subi++)
	{
		if (mesh->GetSubsets()[subi].m_faceCount == 0) continue;

		// slot0 baecolor textureはすでに設定済み
		mesh->DrawSubset(subi);
	}
}
