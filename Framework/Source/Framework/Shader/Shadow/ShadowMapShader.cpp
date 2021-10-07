#include "ShadowMapShader.h"
#include "../../../Application/main.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ShadowMapShader::ShadowMapShader()
	: m_shadowMaps()
	, m_zBuffer()
	, m_lvpcMatrix()
	, m_cascadeAreaTable()
	, m_nearDepth(0.01f)
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

	// 定数バッファ作成
	m_cb2.Create();
	m_cb2.SetToDevice(2);

	return true;
}

//-----------------------------------------------------------------------------
// 描画開始
//-----------------------------------------------------------------------------
void ShadowMapShader::Begin(int numShadow)
{
	if (!g_graphicsDevice) return;
	if (!g_graphicsDevice->g_cpContext) return;

	// 初回のみ保存
	if (numShadow == 0)
	{
		// 現在のRTとZとViewportを記憶
		g_graphicsDevice->g_cpContext->OMGetRenderTargets(1, &m_saveRT, &m_saveZ);
		g_graphicsDevice->g_cpContext->RSGetViewports(&m_numVP, &m_saveVP);
	}

	// テクスチャからVP作成
	auto& desc = m_shadowMaps[numShadow]->GetDesc();
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width	= static_cast<FLOAT>(desc.Width);
	vp.Height	= static_cast<FLOAT>(desc.Height);
	vp.MinDepth = D3D11_MIN_DEPTH;
	vp.MaxDepth = D3D11_MAX_DEPTH;

	// RTとVPを変更
	g_graphicsDevice->g_cpContext->OMSetRenderTargets(1, m_shadowMaps[numShadow]->RTVAddress(), m_zBuffer[numShadow]->DSV());
	g_graphicsDevice->g_cpContext->RSSetViewports(1, &vp);
	// クリア
	g_graphicsDevice->g_cpContext->ClearRenderTargetView(m_shadowMaps[numShadow]->RTV(), cfloat4x4::White);
	g_graphicsDevice->g_cpContext->ClearDepthStencilView(m_zBuffer[numShadow]->DSV(), D3D11_CLEAR_DEPTH, 1, 0);

	// カメラ
	SettingLightCamera(numShadow);

	// シェーダーや定数バッファをセット
	g_graphicsDevice->g_cpContext->IASetInputLayout(m_cpInputLayout.Get());

	g_graphicsDevice->g_cpContext->VSSetShader(m_cpVS.Get(), 0, 0);
	g_graphicsDevice->g_cpContext->PSSetShader(m_cpPS.Get(), 0, 0);
}

//-----------------------------------------------------------------------------
// 描画終了
//-----------------------------------------------------------------------------
void ShadowMapShader::End()
{
	if (!g_graphicsDevice) return;
	if (!g_graphicsDevice->g_cpContext) return;

	if (m_saveRT == nullptr || m_saveZ == nullptr) {
		assert(0 && "エラー：レンダーターゲットの保存に失敗."); return;
	}

	// 記憶してたRTとZとViewportを復元
	g_graphicsDevice->g_cpContext->OMSetRenderTargets(1, &m_saveRT, m_saveZ);
	g_graphicsDevice->g_cpContext->RSSetViewports(m_numVP, &m_saveVP);

	SafeRelease(m_saveRT);
	SafeRelease(m_saveZ);
}

//-----------------------------------------------------------------------------
// モデル描画 レンダーターゲットに影を落とす
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
void ShadowMapShader::SettingLightCamera(int numShadow)
{
	auto& camera = RENDERER.Getcb9().Work();

	mfloat4x4 lvpMat;
	{
		mfloat4x4 viewMatrix = mfloat4x4::CreateRotationX(45 * ToRadians);
		mfloat4x4 transMatrix = mfloat4x4::CreateTranslation(0, 20, 0);
		viewMatrix = transMatrix * viewMatrix;
		viewMatrix.Invert();

		APP.g_gameSystem->AddDebugSphereLine(viewMatrix.Translation(), 2.0f);

		// ライトの射影行列を適当に作成　50mx50mの正射影行列
		mfloat4x4 projMatrix = mfloat4x4::CreateOrthographic(50, 50, 0, 100);

		lvpMat = viewMatrix * projMatrix;
	}

	mfloat4x4 lvpMatrix = lvpMat;// camera.m_view_matrix* camera.m_proj_matrix;
	float3 cameraForward = camera.m_camera_matrix.Forward();
	float3 cameraRight = camera.m_camera_matrix.Right();

	float3 cameraUp;
	cameraUp.Cross(cameraForward, cameraRight);

	//--------------------------------------------------
	// エリアを内包する視錐台の８頂点を求める
	//--------------------------------------------------
	
	float3 vertex[8];
	float viewAngle = 60.0f * ToRadians;
	float aspect = 16.0f / 9.0f;

	// 近平面
	float3 nearPos = camera.m_camera_matrix.Translation() + cameraForward * m_nearDepth;// 中心座標
	float nearY = tanf(viewAngle * 0.5f) * m_nearDepth;
	float nearX = nearY * aspect;
	vertex[0] += nearPos + cameraUp *  nearY + cameraRight *  nearX;
	vertex[1] += nearPos + cameraUp *  nearY + cameraRight * -nearX;
	vertex[2] += nearPos + cameraUp * -nearY + cameraRight *  nearX;
	vertex[3] += nearPos + cameraUp * -nearY + cameraRight * -nearX;

	// 遠平面
	float3 farPos = camera.m_camera_matrix.Translation() + cameraForward * m_cascadeAreaTable[numShadow];
	float farY = tanf(viewAngle * 0.5f) * m_cascadeAreaTable[numShadow];
	float farX = farY * aspect;
	vertex[4] += farPos + cameraUp *  farY + cameraRight *  farX;
	vertex[5] += farPos + cameraUp *  farY + cameraRight * -farX;
	vertex[6] += farPos + cameraUp * -farY + cameraRight *  farX;
	vertex[7] += farPos + cameraUp * -farY + cameraRight * -farX;

	/*APP.g_gameSystem->AddDebugLine(vertex[0], vertex[1]);
	APP.g_gameSystem->AddDebugLine(vertex[1], vertex[2]);
	APP.g_gameSystem->AddDebugLine(vertex[2], vertex[3]);
	APP.g_gameSystem->AddDebugLine(vertex[3], vertex[1]);

	APP.g_gameSystem->AddDebugLine(vertex[0], vertex[4]);
	APP.g_gameSystem->AddDebugLine(vertex[1], vertex[5]);
	APP.g_gameSystem->AddDebugLine(vertex[2], vertex[6]);
	APP.g_gameSystem->AddDebugLine(vertex[3], vertex[7]);

	APP.g_gameSystem->AddDebugLine(vertex[4], vertex[5]);
	APP.g_gameSystem->AddDebugLine(vertex[5], vertex[6]);
	APP.g_gameSystem->AddDebugLine(vertex[6], vertex[7]);
	APP.g_gameSystem->AddDebugLine(vertex[7], vertex[4]);*/

	//--------------------------------------------------
	// 8頂点を変換して最大値、最小値を求める
	//--------------------------------------------------
	float3 vMax, vMin;
	vMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	vMin = {  FLT_MAX,  FLT_MAX,  FLT_MAX };
	for (auto& v : vertex)
	{
		DirectX::XMStoreFloat3(&v, DirectX::XMVector3Transform(v, lvpMatrix));

		vMax.x = std::max(vMax.x, v.x);
		vMax.y = std::max(vMax.y, v.y);
		vMax.z = std::max(vMax.z, v.z);

		vMin.x = std::min(vMin.x, v.x);
		vMin.y = std::min(vMin.y, v.y);
		vMin.z = std::min(vMin.z, v.z);
	}

	//--------------------------------------------------
	// クロップ行列の作成
	//--------------------------------------------------
	float xScale = 2.0f / (vMax.x - vMin.x);
	float yScale = 2.0f / (vMax.y - vMin.y);
	float xOffset = (vMax.x + vMin.x) * -0.5f * xScale;
	float yOffset = (vMax.y + vMin.y) * -0.5f * yScale;

	mfloat4x4 clopMatrix;
	clopMatrix.m[0][0] = xScale;
	clopMatrix.m[1][1] = yScale;
	clopMatrix.m[3][0] = xOffset;
	clopMatrix.m[3][1] = yOffset;

	// ライトビュープロジェクション行列にクロップ行列を乗算
	m_lvpcMatrix[numShadow] = lvpMatrix * clopMatrix;

	// 定数バッファに送信
	m_cb2.Work().m_mLVPC = m_lvpcMatrix[numShadow];
	m_cb2.Write();

	// 次のエリアの近平面までの距離を代入する
	m_nearDepth = m_cascadeAreaTable[numShadow];
}

//-----------------------------------------------------------------------------
// メッシュ描画
//-----------------------------------------------------------------------------
void ShadowMapShader::DrawMeshDepth(const Mesh* mesh, const std::vector<Material>& materials)
{
	mesh->SetToDevice();

	for (UINT i = 0; i < mesh->GetSubsets().size(); i++)
	{
		if (mesh->GetSubsets()[i].m_faceCount == 0) continue;

		// slot0 baecolor textureはすでに設定済み
		mesh->DrawSubset(i);
	}
}
