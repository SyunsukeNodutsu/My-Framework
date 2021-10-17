#include "ModelShader.h"
#include "../../../Application/main.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ModelShader::ModelShader()
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

		hr = g_graphicsDevice->g_cpDevice.Get()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpVS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "エラー：頂点シェーダ作成失敗.");
			return false;
		}

		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		hr = g_graphicsDevice->g_cpDevice.Get()->CreateInputLayout(&layout[0], (UINT)layout.size(), compiledBuffer, sizeof(compiledBuffer), m_cpInputLayout.GetAddressOf());
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

		hr = g_graphicsDevice->g_cpDevice.Get()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpPS.GetAddressOf());
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
	g_graphicsDevice->g_cpContext.Get()->IASetInputLayout(m_cpInputLayout.Get());

	g_graphicsDevice->g_cpContext.Get()->VSSetShader(m_cpVS.Get(), 0, 0);
	g_graphicsDevice->g_cpContext.Get()->PSSetShader(m_cpPS.Get(), 0, 0);
}

//-----------------------------------------------------------------------------
// モデル描画
//-----------------------------------------------------------------------------
void ModelShader::DrawModel(const ModelWork& model, const mfloat4x4& worldMatrix)
{
	if (!model.IsEnable()) return;

	auto& data = model.GetData();
	if (data == nullptr) return;

	auto& camera = APP.g_gameSystem->g_cameraSystem.GetCamera();

	// 全メッシュノードを描画
	for (auto& index : data->GetMeshNodeIndices())
	{
		auto& node = model.GetNodes()[index];
		auto& mesh = model.GetMesh(index);

		// 視錐台カリング
		if(camera->g_isFrustumCull)
		{
			// AABB -> OBB作成
			DirectX::BoundingOrientedBox obb = {};
			DirectX::BoundingOrientedBox::CreateFromBoundingBox(obb, mesh->GetBoundingBox());
			obb.Transform(obb, (node.m_localTransform * worldMatrix));

			if (!camera->GetFrustum().Intersects(obb)) continue;

			//DrawOBB(obb);
		}

		// ワールド行列 設定
		RENDERER.Getcb8().Work().m_world_matrix = node.m_worldTransform * worldMatrix;
		RENDERER.Getcb8().Write();

		// メッシュ描画
		DrawMesh(mesh.get(), data->GetMaterials());
	}
}

//-----------------------------------------------------------------------------
// メッシュ描画
//-----------------------------------------------------------------------------
void ModelShader::DrawMesh(const Mesh* mesh, const std::vector<Material>& materials)
{
	if (mesh == nullptr) return;

	// メッシュ情報を転送
	mesh->SetToDevice();

	for (UINT i = 0; i < mesh->GetSubsets().size(); i++)
	{
		if (mesh->GetSubsets()[i].m_faceCount == 0) continue;

		const Material& material = materials[mesh->GetSubsets()[i].m_materialNo];

		// マテリアル情報を転送
		m_cd11Material.Work().m_baseColor = material.m_baseColor;
		m_cd11Material.Work().m_emissive  = material.m_emissive;
		m_cd11Material.Work().m_metallic  = material.m_metallic;
		m_cd11Material.Work().m_roughness = material.m_roughness;
		m_cd11Material.Write();

		// テクスチャセット
		RENDERER.SetResources(material.m_baseColorTexture.get(), 0);
		RENDERER.SetResources(material.m_emissiveTexture.get(), 1);
		RENDERER.SetResources(material.m_metallicRoughnessTexture.get(), 2);
		RENDERER.SetResources(material.m_normalTexture.get(), 3);

		// サブセット描画
		mesh->DrawSubset(i);
	}
}

//-----------------------------------------------------------------------------
// OBBを3D線でデバッグ描画
//-----------------------------------------------------------------------------
void ModelShader::DrawOBB(const DirectX::BoundingOrientedBox& obb)
{
	float3 obbCorners[obb.CORNER_COUNT];
	obb.GetCorners(obbCorners);

	auto& gameSystem = APP.g_gameSystem;// TODO: APP側で管理すべきではない

	gameSystem->AddDebugLine(obbCorners[0], obbCorners[1]);
	gameSystem->AddDebugLine(obbCorners[1], obbCorners[2]);
	gameSystem->AddDebugLine(obbCorners[2], obbCorners[3]);
	gameSystem->AddDebugLine(obbCorners[3], obbCorners[0]);

	gameSystem->AddDebugLine(obbCorners[4], obbCorners[5]);
	gameSystem->AddDebugLine(obbCorners[5], obbCorners[6]);
	gameSystem->AddDebugLine(obbCorners[6], obbCorners[7]);
	gameSystem->AddDebugLine(obbCorners[7], obbCorners[4]);

	gameSystem->AddDebugLine(obbCorners[0], obbCorners[4]);
	gameSystem->AddDebugLine(obbCorners[1], obbCorners[5]);
	gameSystem->AddDebugLine(obbCorners[2], obbCorners[6]);
	gameSystem->AddDebugLine(obbCorners[3], obbCorners[7]);
}
