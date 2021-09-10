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
// 作成
//-----------------------------------------------------------------------------
void CubeMapGenerator::Generate(int size, const float3& position, std::function<void()> drawProc)
{
	//----------------------------------
	// RTとZバッファを作成
	//----------------------------------
	{
		// 作成する2Dテクスチャ設定
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width				= (UINT)size;
		desc.Height				= (UINT)size;
		desc.Format				= DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.ArraySize			= 6;
		// キューブマップ | Mipmap生成
		desc.MiscFlags			= D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;

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
	ComPtr<ID3D11RenderTargetView> saveRTV[8] = {};
	ComPtr<ID3D11DepthStencilView> saveDSV = nullptr;
	D3D11_VIEWPORT saveVP;

	//
	D3D.GetDeviceContext()->OMGetRenderTargets(8, saveRTV->GetAddressOf(), &saveDSV);

	UINT numVPs = 1;
	D3D.GetDeviceContext()->RSGetViewports(&numVPs, &saveVP);

	// Viewportを変更
	D3D11_VIEWPORT vp = { 0, 0, (float)size, (float)size, 0, 1 };
	D3D.GetDeviceContext()->RSSetViewports(1, &vp);

	// RTのリソースを取得
	ID3D11Resource* cubeMapResource = nullptr;
	m_cubeMap->RTV()->GetResource(&cubeMapResource);

	// カメラ
	mfloat4x4 projMatrix = {};
	projMatrix = DirectX::XMMatrixPerspectiveFovLH(90.0f * ToRadians, 1.0f, 0.01f, 2000.0f);
	D3D.GetRenderer().SetProjMatrix(projMatrix);

	// キューブマップの各面(6枚)を描画
	for (int i = 0; i < 6; i++)
	{
		// キューブマップの各画像へのビューを作成
		ID3D11RenderTargetView* rtv = nullptr;
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = m_cubeMap->GetInfo().Format;	// Format
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Texture2DArray.ArraySize = 1;
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		rtvDesc.Texture2DArray.MipSlice = 0;
		// レンダーターゲットビュー作成
		HRESULT hr = D3D.GetDevice()->CreateRenderTargetView(cubeMapResource, &rtvDesc, &rtv);
		if (FAILED(hr)) {
			assert(0 && "RenderTargetViewの作成に失敗");
		}

		// RT、Zクリア
		D3D.GetDeviceContext()->ClearRenderTargetView(rtv, cfloat4x4::Black);
		D3D.GetDeviceContext()->ClearDepthStencilView(m_zBuffer->DSV(), D3D11_CLEAR_DEPTH, 1, 0);

		// RTとZを変更する
		D3D.GetDeviceContext()->OMSetRenderTargets(1, &rtv, m_zBuffer->DSV());

		// カメラ設定
		mfloat4x4 mView;
		switch (i)
		{
		case D3D11_TEXTURECUBE_FACE_POSITIVE_X:
			mView = DirectX::XMMatrixLookAtLH(position, position + float3(1, 0, 0), float3(0, 1, 0));
			break;
		case D3D11_TEXTURECUBE_FACE_NEGATIVE_X:
			mView = DirectX::XMMatrixLookAtLH(position, position + float3(-1, 0, 0), float3(0, 1, 0));
			break;
		case D3D11_TEXTURECUBE_FACE_POSITIVE_Y:
			mView = DirectX::XMMatrixLookAtLH(position, position + float3(0, 1, 0), float3(0, 0, -1));
			break;
		case D3D11_TEXTURECUBE_FACE_NEGATIVE_Y:
			mView = DirectX::XMMatrixLookAtLH(position, position + float3(0, -1, 0), float3(0, 0, 1));
			break;
		case D3D11_TEXTURECUBE_FACE_POSITIVE_Z:
			mView = DirectX::XMMatrixLookAtLH(position, position + float3(0, 0, 1), float3(0, 1, 0));
			break;
		case D3D11_TEXTURECUBE_FACE_NEGATIVE_Z:
			mView = DirectX::XMMatrixLookAtLH(position, position + float3(0, 0, -1), float3(0, 1, 0));
			break;
		}
		D3D.GetRenderer().SetViewMatrix(mView.Invert());

		// 描画実行
		drawProc();

		rtv->Release();
	}

	// RTとZを元に戻す
	D3D.GetDeviceContext()->OMSetRenderTargets(8, saveRTV->GetAddressOf(), saveDSV.Get());

	// Viewportを元に戻す
	D3D.GetDeviceContext()->RSSetViewports(numVPs, &saveVP);

	//
	projMatrix = DirectX::XMMatrixPerspectiveFovLH(90.0f * ToRadians, 16.0f / 9, 0.01f, 2000.0f);
	D3D.GetRenderer().SetProjMatrix(projMatrix);

	// 取得したリソースの参照カウンターを減らす
	cubeMapResource->Release();

	// ミップマップの生成
	D3D.GetDeviceContext()->GenerateMips(m_cubeMap->SRV());
}

//-----------------------------------------------------------------------------
// 読み込み
//-----------------------------------------------------------------------------
void CubeMapGenerator::LoadFromFile(const std::string& filename)
{
	m_cubeMap = std::make_shared<Texture>();
	//m_cubeMap->Load(filename);
}

//-----------------------------------------------------------------------------
// 保存
//-----------------------------------------------------------------------------
void CubeMapGenerator::SaveToFile(const std::string& filename)
{
	if (m_cubeMap == nullptr)
		return;

	ID3D11Resource* cubeMapResource = nullptr;
	m_cubeMap->RTV()->GetResource(&cubeMapResource);
	cubeMapResource->Release();

	HRESULT hr = S_FALSE;
	/*DirectX::ScratchImage image;
	hr = DirectX::CaptureTexture(
		D3D.GetDevice(), D3D.GetDeviceContext(),
		cubeMapResource, image
	);*/

	if (FAILED(hr)) {
		assert(0 && "");
		return;
	}

	//hr = DirectX::SaveToDDSFile(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::DDS_FLAGS_NONE, sjis_to_wide(filename).c_str());
	if (FAILED(hr)) {
		assert(0 && "");
		return;
	}
}
