//-----------------------------------------------------------------------------
// File: Mesh.h
//
// メッシュ マテリアルデータなど
//-----------------------------------------------------------------------------
#pragma once

//==========================================================
// メッシュ用 頂点情報
//==========================================================
struct MeshVertex
{
	float3	m_pos;							// 座標
	float2	m_uv;							// UV
	float3	m_normal;						// 法線
	UINT	m_color = 0xFFFFFFFF;			// RGBA色(容量削減のため、各色0～255のUINT型)
	float3	m_tangent;						// 接線

	std::array<short, 4> m_skinIndexList;	// スキニングIndexリスト
	std::array<float, 4> m_skinWeightList;	// スキニングウェイトリスト
};

//==========================================================
// メッシュ用 面情報
//==========================================================
struct MeshFace
{
	UINT		m_index[3];					// 三角形を構成する頂点のIndex
};

//==========================================================
// メッシュ用 サブセット情報
//==========================================================
struct MeshSubset
{
	UINT		m_materialNo = 0;			// マテリアルNo

	UINT		m_faceStart = 0;			// 面Index　このマテリアルで使用されている最初の面のIndex
	UINT		m_faceCount = 0;			// 面数　FaceStartから、何枚の面が使用されているかの
};


//==========================================================
//
// メッシュクラス
//
//==========================================================
class Mesh
{
public:

	// @brief コンストラクタ
	Mesh();

	// @brief デストラクタ
	~Mesh() { Release(); }

	// @brief GPUへ転送
	void SetToDevice() const;

	// メッシュ作成
	// @param vertices 頂点配列
	// @param faces 面インデックス情報配列
	// @param subsets サブセット情報配列
	// @return 成功...true 失敗...false
	bool Create(const std::vector<MeshVertex>& vertices, const std::vector<MeshFace>& faces, const std::vector<MeshSubset>& subsets, bool isSkinMesh);

	// @brief 解放
	void Release()
	{
		m_subsets.clear();
		m_positions.clear();
		m_faces.clear();
	}

	// @brief 指定サブセットを描画
	void DrawSubset(int subsetNo) const;

	//--------------------------------------------------
	// 取得・設定
	//--------------------------------------------------

	// @brief サブセット情報配列を返す
	const std::vector<MeshSubset>& GetSubsets() const { return m_subsets; }

	// @brief 頂点の座標配列を返す
	const std::vector<float3>& GetVertexPositions() const { return m_positions; }

	// @brief 面の配列を返す
	const std::vector<MeshFace>& GetFaces() const { return m_faces; }

	// @brief 軸平行境界ボックスを返す
	const DirectX::BoundingBox& GetBoundingBox() const { return m_boundingB; }

	// @brief 境界球を返す
	const DirectX::BoundingSphere& GetBoundingSphere() const { return m_boundingS; }

	// @brief スキンメッシュかどうかを返す
	bool IsSkinMesh() const { return m_isSkinMesh; }

private:

	// バッファ
	Buffer m_vertexBuffer;	// 頂点バッファ
	Buffer m_indexBuffer;	// インデックスバッファ

	// サブセット情報
	std::vector<MeshSubset>	m_subsets;

	// 境界データ
	DirectX::BoundingBox	m_boundingB;	// 軸平行境界ボックス
	DirectX::BoundingSphere	m_boundingS;	// 境界球

	// 座標のみの配列(複製)
	std::vector<float3>		m_positions;
	// 面情報のみの配列(複製)
	std::vector<MeshFace>	m_faces;

	// スキンメッシュ？
	bool m_isSkinMesh;

private:

	Mesh(const Mesh& src) = delete;
	void operator=(const Mesh& src) = delete;

};

//==========================================================
// マテリアル (glTFベースのPBRマテリアル
//==========================================================
struct Material
{
	typedef std::shared_ptr<Texture> texture_sharedPtr;

	//---------------------------------------
	// 材質データ
	//---------------------------------------

	// 名前
	std::string			m_name;

	// 基本色
	texture_sharedPtr	m_baseColorTexture;					// 基本色テクスチャ
	float4				m_baseColor = float4(1, 1, 1, 1);	// 基本色のスケーリング係数(RGBA)

	// 金属性、粗さ
	texture_sharedPtr	m_metallicRoughnessTexture;			// B:金属製 G:粗さ
	float				m_metallic = 0.0f;					// 金属性のスケーリング係数
	float				m_roughness = 1.0f;					// 粗さのスケーリング係数

	// 自己発光
	texture_sharedPtr	m_emissiveTexture;					// 自己発光テクスチャ
	float3				m_emissive = float3(0, 0, 0);		// 自己発光のスケーリング係数(RGB)

	// 法線マップ
	texture_sharedPtr	m_normalTexture;

	// @brief コンストラクタ
	Material()
	{
		m_baseColorTexture			= D3D.GetWhiteTex();
		m_metallicRoughnessTexture	= D3D.GetWhiteTex();
		m_emissiveTexture			= D3D.GetWhiteTex();
		m_normalTexture				= D3D.GetNormalTex();
	}

};
