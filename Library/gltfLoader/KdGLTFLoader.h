//-----------------------------------------------------------------------------
// File: KdGLTFLoader.h
//
// GLTF形式の3Dモデルファイルを読み込む
//-----------------------------------------------------------------------------
#pragma once

//============================
// マテリアル
//============================
struct KdGLTFMaterial
{
	std::string		m_name;						// マテリアル名


	std::string		m_alphaMode = "OPAQUE";		// "OPAQUE" : レンダリングされた出力は完全に不透明で、アルファ値は無視されます。
												// "MASK"   : レンダリングされた出力は、アルファ値と指定されたアルファカットオフ値に応じて、完全に不透明または完全に透明になります。このモードは、木の葉やワイヤーフェンスなどのジオメトリをシミュレートするために使用されます。
												// "BLEND"  :

	float			m_alphaCutoff = 0.5f;		// MASKモード時に、カットオフの閾値として使用　それ以外のモードでは使用されない
	bool			m_doubleSided = false;		// 両面か？

	//------------------------------
	// PBR材質データ
	//------------------------------
	std::string		m_baseColorTexture;			// 基本色テクスチャのファイル名
	float4			m_baseColor = { 1,1,1,1 };	// 上記テクスチャのRGBAのスケーリング要素

	// 金属性、粗さ
	std::string		m_metallicRoughnessTexture;	// メタリックとラフネスのテクスチャ　青成分 = メタリック 緑成分 = ラフネス
	float			m_metallic = 1.0f;			// 上記テクスチャのメタリック要素の乗算用　テクスチャが無い時は乗算ではなくそのまま使用する
	float			m_roughness = 1.0f;			// 上記テクスチャのラフネス要素の乗算用　テクスチャが無い時は乗算ではなくそのまま使用する

	// エミッシブ：自己発光 つまり表面から放出される光　RGBのみ使用
	std::string		m_emissiveTexture;			// エミッシブテクスチャ　RGBを使用
	float3			m_emissive = { 0,0,0 };		// 上記テクスチャのRGBのスケーリング要素

	//------------------------------
	// その他テクスチャ
	//------------------------------
	std::string		m_normalTexture;			// 法線マップテクスチャ
	std::string		m_occlusionTexture;			// 光の遮蔽度テクスチャ　赤成分のみ使用

};

//============================
// ノード １つのメッシュやマテリアルなど
//============================
struct KdGLTFNode
{
	//---------------------------
	// 基本情報
	//---------------------------
	std::string					m_name;// 名前

	// 子Indexリスト
	std::vector<int>			m_children;
	// 親Index
	int							m_parent = -1;
	// ボーンの場合のIndex
	int							m_boneNodeIndex = -1;

	// 行列
	mfloat4x4					m_localTransform;
	mfloat4x4					m_worldTransform;
	mfloat4x4					m_inverseBindMatrix; // ボーンオフセット行列

	//---------------------------
	// Mesh専用情報
	//---------------------------
	bool						m_isMesh = false;
	struct Mesh
	{
		// 頂点配列
		std::vector<MeshVertex> m_vertices;
		// 面情報配列
		std::vector<MeshFace>	m_faces;
		// サブセット情報配列
		std::vector<MeshSubset> m_subsets;

		bool					m_isSkinMesh = false;
	};
	Mesh						m_mesh;

};

//============================
// アニメーションデータ
//============================
struct KdGLTFAnimationData
{
	// アニメーション名
	std::string				m_name;
	// アニメの長さ
	float					m_maxLength = 0;
	// １ノードのアニメーションデータ
	struct Node
	{
		int								m_nodeOffset = -1;	// 対象ノードのOffset
		// 各チャンネル
		std::vector<AnimKeyVector3>		m_translations;	// 位置キーリスト
		std::vector<AnimKeyQuaternion>	m_rotations;	// 回転キーリスト
		std::vector<AnimKeyVector3>		m_scales;		// 拡大キーリスト
	};
	// 全ノード用アニメーションデータ
	std::vector<std::shared_ptr<Node>>	m_nodes;
};

//============================
// モデルデータ
//============================
struct KdGLTFModel
{
	// 全ノードデータ
	std::vector<KdGLTFNode>						m_nodes;

	// 全ノード中のルートノードのみのIndexリスト
	std::vector<int>							m_rootNodeIndices;

	// 全ノード中のボーンノードだけのIndexリスト
	std::vector<int>							m_boneNodeIndices;

	// マテリアル一覧
	std::vector<KdGLTFMaterial>					m_materials;

	// アニメーションデータリスト
	std::vector<std::shared_ptr<KdGLTFAnimationData>>	m_animations;
};


// @brief GLTF形式の3Dモデルを読み込む
// @note LoaderはTinygltf: https://github.com/syoyo/tinygltf
// @param path .glflファイルのパス
std::shared_ptr<KdGLTFModel> KdLoadGLTFModel(const std::string& path);
