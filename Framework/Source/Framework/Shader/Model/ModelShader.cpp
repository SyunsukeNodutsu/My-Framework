#include "ModelShader.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ModelShader::ModelShader()
	: m_cpVS(nullptr)
	, m_cpPS(nullptr)
	, m_cpInputLayout(nullptr)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool ModelShader::Initialize()
{
	HRESULT hr = S_OK;

	//--------------------------------------------------
	// 頂点シェーダ
	//--------------------------------------------------
	{
		#include "ModelShader_VS.shaderinc"

		hr = D3D.GetDevice()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpVS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "エラー：頂点シェーダ作成失敗.");
			return false;
		}

		// 1頂点の詳細な情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		hr = D3D.GetDevice()->CreateInputLayout(&layout[0], (UINT)layout.size(), compiledBuffer, sizeof(compiledBuffer), m_cpInputLayout.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "エラー：頂点入力レイアウト作成失敗.");
			return false;
		}
	}

	//--------------------------------------------------
	// ピクセルシェーダ
	//--------------------------------------------------
	{
		#include "ModelShader_PS.shaderinc"

		hr = D3D.GetDevice()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpPS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "エラー：ピクセルシェーダ作成失敗.");
			return false;
		}
	}

	m_cd11Material.Create();
	m_cd11Material.SetToDevice(Renderer::use_slot_material);

	return true;
}

//-----------------------------------------------------------------------------
// 開始
//-----------------------------------------------------------------------------
void ModelShader::Begin()
{
	if (/*mesh->IsSkinMesh()*/false)
	{

	}
	else
	{
		D3D.GetDeviceContext()->VSSetShader(m_cpVS.Get(), 0, 0);
		D3D.GetDeviceContext()->IASetInputLayout(m_cpInputLayout.Get());
	}

	D3D.GetDeviceContext()->PSSetShader(m_cpPS.Get(), 0, 0);
}

//-----------------------------------------------------------------------------
// モデル描画
//-----------------------------------------------------------------------------
void ModelShader::DrawModel(const ModelWork& model, const mfloat4x4& worldMatrix)
{
	//if (!model.IsEnable()) return;

	auto& data = model.GetData();
	if (data == nullptr)
		return;

	// 全メッシュノードを描画
	for (auto& nodeIdx : data->GetMeshNodeIndices())
	{
		auto& rWorkNode = model.GetNodes()[nodeIdx];
		auto& mesh = model.GetMesh(nodeIdx);

		// ワールド行列 設定
		D3D.GetRenderer().SetWorldMatrix(rWorkNode.m_worldTransform * worldMatrix);

		// メッシュ描画
		DrawMesh(mesh.get(), data->GetMaterials());
	}
}

//-----------------------------------------------------------------------------
// メッシュ描画
//-----------------------------------------------------------------------------
void ModelShader::DrawMesh(const Mesh* mesh, const std::vector<Material>& materials)
{
	if (mesh == nullptr)
		return;

	// メッシュ情報をセット
	mesh->SetToDevice();

	for (UINT subi = 0; subi < mesh->GetSubsets().size(); subi++)
	{
		if (mesh->GetSubsets()[subi].m_faceCount == 0)
			continue;

		const Material& material = materials[mesh->GetSubsets()[subi].m_materialNo];

		//-----------------------
		// マテリアル情報を定数バッファへ書き込む
		//-----------------------
		/*m_cb1_Material.Work().BaseColor = material.BaseColor;
		m_cb1_Material.Work().Emissive = material.Emissive;
		m_cb1_Material.Work().Metallic = material.Metallic;
		m_cb1_Material.Work().Roughness = material.Roughness;
		m_cb1_Material.Write();*/

		m_cd11Material.Work().m_baseColor = material.m_baseColor;
		m_cd11Material.Work().m_roughness = material.m_roughness;
		m_cd11Material.Write();

		//-----------------------
		// テクスチャセット
		//-----------------------
		ID3D11ShaderResourceView* srvs[4] = {};

		//D3D.GetDeviceContext()->PSSetShaderResources(0, _countof(srvs), srvs);

		D3D.GetRenderer().SetTexture(material.m_baseColorTexture.get());

		//-----------------------
		// サブセット描画
		//-----------------------
		mesh->DrawSubset(subi);
	}
}
