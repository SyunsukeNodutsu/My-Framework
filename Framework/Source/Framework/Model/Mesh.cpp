#include "Mesh.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Mesh::Mesh()
	: m_vertexBuffer()
	, m_indexBuffer()
	, m_subsets()
	, m_boundingB()
	, m_boundingS()
	, m_positions()
	, m_faces()
	, m_isSkinMesh(false)
{
}

//-----------------------------------------------------------------------------
// デバイスにセット GPUへ転送
//-----------------------------------------------------------------------------
void Mesh::SetToDevice() const
{
	if (!GraphicsDeviceChild::g_graphicsDevice) return;
	if (!GraphicsDeviceChild::g_graphicsDevice->g_cpContext) return;

	// 頂点バッファセット
	UINT stride = sizeof(MeshVertex);
	UINT offset = 0;
	g_graphicsDevice->g_cpContext.Get()->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddress(), &stride, &offset);

	// インデックスバッファセット
	g_graphicsDevice->g_cpContext.Get()->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// プリミティブ・トポロジーをセット
	g_graphicsDevice->g_cpContext.Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//-----------------------------------------------------------------------------
// 生成
//-----------------------------------------------------------------------------
bool Mesh::Create(const std::vector<MeshVertex>& vertices, const std::vector<MeshFace>& faces, const std::vector<MeshSubset>& subsets, bool isSkinMesh)
{
	Release();

	//------------------------------
	// サブセット情報
	//------------------------------
	m_subsets = subsets;

	//------------------------------
	// 頂点バッファ作成
	//------------------------------
	if (vertices.size() > 0)
	{
		// 書き込むデータ
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &vertices[0]; // バッファに書き込む頂点配列の先頭アドレス
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		// 頂点バッファ作成
		if (!m_vertexBuffer.Create(D3D11_BIND_VERTEX_BUFFER, sizeof(MeshVertex) * (UINT)vertices.size(), D3D11_USAGE_DEFAULT, &initData)) {
			Release();
			return false;
		}

		// 座標のみの配列
		m_positions.resize(vertices.size());
		for (UINT i = 0; i < m_positions.size(); i++)
		{
			m_positions[i] = vertices[i].m_pos;
		}

		// AA境界データ作成
		DirectX::BoundingBox::CreateFromPoints(m_boundingB, m_positions.size(), &m_positions[0], sizeof(float3));
		// 境界球データ作成
		DirectX::BoundingSphere::CreateFromPoints(m_boundingS, m_positions.size(), &m_positions[0], sizeof(float3));
	}

	//------------------------------
	// インデックスバッファ作成
	//------------------------------
	if (faces.size() > 0)
	{
		// 書き込むデータ
		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem			= &faces[0];
		initData.SysMemPitch		= 0;
		initData.SysMemSlicePitch	= 0;

		// バッファ作成
		if (!m_indexBuffer.Create(D3D11_BIND_INDEX_BUFFER, sizeof(MeshFace) * (UINT)faces.size(), D3D11_USAGE_DEFAULT, &initData)) {
			Release();
			return false;
		}

		// 面情報コピー
		m_faces = faces;
	}

	m_isSkinMesh = isSkinMesh;

	return true;
}

//-----------------------------------------------------------------------------
// サブセット描画
//-----------------------------------------------------------------------------
void Mesh::DrawSubset(int subsetNo) const
{
	if (!GraphicsDeviceChild::g_graphicsDevice) return;
	if (!GraphicsDeviceChild::g_graphicsDevice->g_cpContext) return;

	if (subsetNo >= (int)m_subsets.size()) return; // 範囲外のサブセット
	if (m_subsets[subsetNo].m_faceCount == 0) return; // 面数が0

	// 描画
	g_graphicsDevice->g_cpContext.Get()->DrawIndexed(
		m_subsets[subsetNo].m_faceCount * 3,// 頂点数
		m_subsets[subsetNo].m_faceStart * 3,// 頂点の開始位置
		0
	);
}
