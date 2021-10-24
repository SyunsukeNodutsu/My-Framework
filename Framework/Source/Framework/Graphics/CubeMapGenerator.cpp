#include "CubeMapGenerator.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
CubeMapGenerator::CubeMapGenerator()
	: m_cubeMap(nullptr)
	, m_zBuffer(nullptr)
{
}

//-----------------------------------------------------------------------------
// 指定サイズのキューブマップを生成し 風景を描画する
//-----------------------------------------------------------------------------
void CubeMapGenerator::Generate(int size, const float3& position, std::function<void()> drawProc)
{
	if (!GraphicsDeviceChild::g_graphicsDevice) return;
	if (!GraphicsDeviceChild::g_graphicsDevice->g_cpContext) return;

	//----------------------------------
	// RTとZバッファを作成
	//----------------------------------
	{
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width				= (UINT)size;
		desc.Height				= (UINT)size;
		desc.Format				= DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.ArraySize			= 6;
		desc.MiscFlags			= D3D11_RESOURCE_MISC_TEXTURECUBE |	D3D11_RESOURCE_MISC_GENERATE_MIPS;

		desc.Usage				= D3D11_USAGE_DEFAULT;
		desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.CPUAccessFlags		= 0;
		desc.MipLevels			= 0; // 完全なミップマップチェーン
		desc.SampleDesc.Count	= 1;
		desc.SampleDesc.Quality = 0;

		m_cubeMap = std::make_shared<Texture>();
		m_cubeMap->Create(desc);

		m_zBuffer = std::make_shared<Texture>();
		m_zBuffer->CreateDepthStencil(size, size);
	}

	//----------------------------------
	// RT、Z、VPの変更
	//----------------------------------
	// 現在のRTとZとVPを取得しておく
	ID3D11RenderTargetView* saveRTV[8] = {};
	ID3D11DepthStencilView* saveDSV = nullptr;
	g_graphicsDevice->g_cpContext->OMGetRenderTargets(8, saveRTV, &saveDSV);
	D3D11_VIEWPORT saveVP;
	UINT numVPs = 1;
	g_graphicsDevice->g_cpContext->RSGetViewports(&numVPs, &saveVP);

	// Viewportを変更
	D3D11_VIEWPORT vp = { 0, 0, (float)size, (float)size, 0, 1 };
	g_graphicsDevice->g_cpContext->RSSetViewports(1, &vp);

	// RTのリソースを取得
	ID3D11Resource* cubeMapResource = nullptr;
	m_cubeMap->RTV()->GetResource(&cubeMapResource);

	// カメラ
	Camera camera;
	camera.SetFovAngleY(90);
	camera.SetAspect(2000);
	camera.SetNearZ(0.01f);
	camera.SetFarZ(1.0f);

	// キューブマップの各面(6枚)を描画
	for (int i = 0; i < 6; i++)
	{
		// キューブマップの各画像へのビューを作成
		ID3D11RenderTargetView* rtv = nullptr;
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format							= m_cubeMap->GetDesc().Format;
		rtvDesc.ViewDimension					= D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Texture2DArray.ArraySize		= 1;
		rtvDesc.Texture2DArray.FirstArraySlice	= i;
		rtvDesc.Texture2DArray.MipSlice			= 0;

		// レンダーターゲットビュー作成
		HRESULT hr = g_graphicsDevice->g_cpDevice->CreateRenderTargetView(cubeMapResource, &rtvDesc, &rtv);
		if (FAILED(hr))
		{
			assert(0 && "RenderTargetViewの作成に失敗");
		}

		// RT、Zクリア
		g_graphicsDevice->g_cpContext->ClearRenderTargetView(rtv, cfloat4x4(0, 0, 0, 1));
		g_graphicsDevice->g_cpContext->ClearDepthStencilView(m_zBuffer->DSV(), D3D11_CLEAR_DEPTH, 1, 0);

		// RTとZを変更する
		g_graphicsDevice->g_cpContext->OMSetRenderTargets(1, &rtv, m_zBuffer->DSV());

		// カメラ設定
		mfloat4x4 viewMatrix;
		switch (i)
		{
		case D3D11_TEXTURECUBE_FACE_POSITIVE_X:
			viewMatrix = DirectX::XMMatrixLookAtLH(position, position + float3( 1,  0,  0), float3(0, 1,  0));
			break;
		case D3D11_TEXTURECUBE_FACE_NEGATIVE_X:
			viewMatrix = DirectX::XMMatrixLookAtLH(position, position + float3(-1,  0,  0), float3(0, 1,  0));
			break;
		case D3D11_TEXTURECUBE_FACE_POSITIVE_Y:
			viewMatrix = DirectX::XMMatrixLookAtLH(position, position + float3( 0,  1,  0), float3(0, 0, -1));
			break;
		case D3D11_TEXTURECUBE_FACE_NEGATIVE_Y:
			viewMatrix = DirectX::XMMatrixLookAtLH(position, position + float3( 0, -1,  0), float3(0, 0,  1));
			break;
		case D3D11_TEXTURECUBE_FACE_POSITIVE_Z:
			viewMatrix = DirectX::XMMatrixLookAtLH(position, position + float3( 0,  0,  1), float3(0, 1,  0));
			break;
		case D3D11_TEXTURECUBE_FACE_NEGATIVE_Z:
			viewMatrix = DirectX::XMMatrixLookAtLH(position, position + float3( 0,  0, -1), float3(0, 1,  0));
			break;
		}
		camera.SetCameraMatrix(viewMatrix.Invert());
		camera.SetToShader();

		// 描画実行
		drawProc();

		rtv->Release();
	}

	// RTとZを元に戻す
	g_graphicsDevice->g_cpContext->OMSetRenderTargets(8, saveRTV, saveDSV);
	for (auto&& rtv : saveRTV)
		SafeRelease(rtv);
	SafeRelease(saveDSV);

	// Viewportを元に戻す
	g_graphicsDevice->g_cpContext->RSSetViewports(numVPs, &saveVP);

	// 取得したリソースの参照カウンターを減らす
	cubeMapResource->Release();

	// ミップマップの生成
	g_graphicsDevice->g_cpContext->GenerateMips(m_cubeMap->SRV());
}

//-----------------------------------------------------------------------------
// ファイルから読み込む
//-----------------------------------------------------------------------------
void CubeMapGenerator::LoadFromFile(const std::string& filepath)
{
	m_cubeMap = std::make_shared<Texture>();
	m_cubeMap->Create(filepath);
}
