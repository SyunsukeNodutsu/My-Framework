//-----------------------------------------------------------------------------
// File: Model.h
//
// モデル管理
// データ ... ModelData
// ロジック 操作 ... ModelWork
//-----------------------------------------------------------------------------
#pragma once

struct AnimationData;
struct KdGLTFModel;

//==========================================================
// モデル(データ側)
//==========================================================
class ModelData
{
public:

	// ノード：モデルを形成するメッシュを扱うための最小単位
	struct Node
	{
		std::string m_name;					// ノード名

		std::shared_ptr<Mesh> m_spMesh;		// メッシュ

		mfloat4x4 m_localTransform;			// 直属の親ボーンからの行列
		mfloat4x4 m_worldTransform;			// 原点からの行列
		mfloat4x4 m_boneInverseWorldMatrix;	// 原点からの逆行列

		int m_parent = -1;					// 親インデックス
		std::vector<int> m_children;		// 子供へのインデックスリスト

		//int m_nodeIndex = -1;				// 先頭から何番目のノード？
		int m_boneIndex = -1;				// ボーンノードの時、先頭から何番目のボーン？

		bool m_isSkinMesh = false;			// スキンメッシュ？
	};

	// @brief コンストラクタ
	ModelData();

	// @brief デストラクタ
	~ModelData() { Release(); }

	// @brief 読み込み
	// @param filepath モデルのファイルパス
	// @return 成功...true
	bool Load(const std::string& filepath);

	// @brief メッシュを返す
	// @param index 取得したいメッシュ番号
	// @return 成功...メッシュ 失敗...null
	const std::shared_ptr<Mesh> GetMesh(UINT index) const {
		return index < m_originalNodes.size() ? m_originalNodes[index].m_spMesh : nullptr;
	}

	// @brief ノード名前検索
	// @param name 名前
	// @return 成功...node 失敗...null
	Node* FindNode(std::string name) {
		for (auto&& node : m_originalNodes)
			if (node.m_name == name) return &node;
		return nullptr;
	}

	// @brief マテリアル配列を返す
	// @return モデルのマテリアル配列
	const std::vector<Material>& GetMaterials() const { return m_materials; }

	// @brief ノード配列を返す
	// @return モデルのノード配列
	const std::vector<Node>& GetOriginalNodes() const { return m_originalNodes; }

	// @brief アニメーションデータ取得
	// @param name アニメーションの名前
	// @return 成功...モデルのアニメーションデータ 失敗...null
	const std::shared_ptr<AnimationData> GetAnimation(const std::string& name) const;

	// @brief アニメーションデータ取得
	// @param index アニメーションのデータ番号(Blenderで確認)
	// @return 成功...モデルのアニメーションデータ 失敗...null
	const std::shared_ptr<AnimationData> GetAnimation(UINT index) const {
		return index >= m_spAnimations.size() ? nullptr : m_spAnimations[index];
	}

	// @brief RootノードのみのIndex配列を返す
	// @return ノードのIndex配列
	const std::vector<int>& GetRootNodeIndices() const { return m_rootNodeIndices; }

	// @brief ボーンノードのみのIndex配列を返す
	// @return ノードのIndex配列
	const std::vector<int>& GetBoneNodeIndices() const { return m_boneNodeIndices; }

	// @brief メッシュが存在するノードのみのIndex配列を返す
	// @return ノードのIndex配列
	const std::vector<int>& GetMeshNodeIndices() const { return m_meshNodeIndices; }

private:

	// ノード
	std::vector<Node>	m_originalNodes;	// 全ノード配列
	std::vector<int>	m_rootNodeIndices;	// 全ノード中 RootノードのみのIndex配列
	std::vector<int>	m_boneNodeIndices;	// 全ノード中 ボーンノードのみのIndex配列
	std::vector<int>	m_meshNodeIndices;	// 全ノード中 メッシュが存在するノードのみのIndexn配列

	//マテリアル配列
	std::vector<Material> m_materials;

	// アニメーションデータリスト
	std::vector<std::shared_ptr<AnimationData>>	m_spAnimations;

	// ファイルパス
	std::string	m_filepath;

private:

	// @brief 解放
	void Release();

	// @brief ノード作成
	// @param model 作成元 モデルデータ
	void CreateNodes(std::shared_ptr<KdGLTFModel>& model);

	// @brief マテリアル作成
	// @param model 作成元 モデルデータ
	void CreateMaterials(std::shared_ptr<KdGLTFModel>& model);

	// @brief アニメーション作成
	// @param model 作成元 モデルデータ
	void CreateAnimations(std::shared_ptr<KdGLTFModel>& model);

};



//==========================================================
// モデル(ロジック.操作側)
//==========================================================
class ModelWork
{
public:

	// ノード：活動中変化する可能性のあるデータ、検索用の名前
	struct Node
	{
		std::string	m_name;			// ノード名

		mfloat4x4 m_localTransform;	// 直属の親ボーンからの行列
		mfloat4x4 m_worldTransform;	// 原点からの行列

		void copy(const ModelData::Node& node)
		{
			m_name = node.m_name;
			m_localTransform = node.m_localTransform;
			m_worldTransform = node.m_worldTransform;
		}
	};

	// @brief コンストラクタ
	ModelWork();

	// @brief デストラクタ
	~ModelWork() = default;

	// @brief モデルの読み込み
	// @param filepath モデルのファイルパス
	// @return 成功...true
	bool Load(const std::string& filepath);

	// @brief 全身のボーンの行列を計算
	void CalcNodeMatrices();

	//--------------------------------------------------
	// 設定 取得
	//--------------------------------------------------

	// @brief モデル設定
	// @note コピーノードの生成も同時に行います
	// @param model モデル ソース
	void SetModel(const std::shared_ptr<ModelData>& model);

	// @brief 有効フラグを返す
	// @return 有効フラグ
	bool IsEnable() const { return m_enable; }

	// @brief 有効フラグを設定
	// @param enable 設定する値
	void SetEnable(bool enable) { m_enable = enable; }

	// @brief ノード検索：文字列
	// @param name 取得したいノードの名前
	// @return 成功...ノード 失敗...null
	const ModelData::Node* FindDataNode(std::string& name) const {
		return (m_spModel) ? m_spModel->FindNode(name) : nullptr;
	}
	const Node* FindNode(std::string name) const;
	Node* FindWorkNode(std::string name);

	// @brief モデルデータを返す
	// @return モデルデータ
	inline const std::shared_ptr<ModelData> GetData() const { return m_spModel; }

	// @brief メッシュを返す
	// @param index 取得するメッシュ番号
	// @return メッシュ
	inline const std::shared_ptr<Mesh> GetMesh(UINT index) const {
		return index >= m_coppiedNodes.size() ? nullptr : GetDataNodes()[index].m_spMesh;
	}

	// @brief データノードリストを返す
	//
	const std::vector<ModelData::Node>& GetDataNodes() const {
		assert(m_spModel && "モデルデータが存在しません");
		return m_spModel->GetOriginalNodes();
	}
	// @brief コピーノードリストを返す
	const std::vector<Node>& GetNodes() const { return m_coppiedNodes; }

	// @brief コピーノードリストを返す
	std::vector<Node>& WorkNodes() { return m_coppiedNodes; }

	// @brief アニメーションデータを返す
	// @param name アニメーションの名前
	// @return アニメーションデータ
	const std::shared_ptr<AnimationData> GetAnimation(const std::string& name) const {
		return !m_spModel ? nullptr : m_spModel->GetAnimation(name);
	}

	// @brief アニメーションデータを返す
	// @param index アニメーションの番号
	// @return アニメーションデータ
	const std::shared_ptr<AnimationData> GetAnimation(int index) const {
		return !m_spModel ? nullptr : m_spModel->GetAnimation(index);
	}

private:

	std::shared_ptr<ModelData>	m_spModel;		// モデルデータ
	std::vector<Node>			m_coppiedNodes;	// 活動中変化する可能性のある全ノードデータのコピー配列
	bool						m_enable;		// 有効フラグ

private:

	// @brief 再帰呼び出し用計算関数
	// @param nodeIndex
	// @param parentNodeIndex
	void recCalcNodeMatrices(int nodeIndex, int parentNodeIndex = -1);

};
