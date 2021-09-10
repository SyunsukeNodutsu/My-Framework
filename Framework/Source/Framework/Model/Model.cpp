﻿#include "Model.h"
#include "../../../../Library/gltfLoader/KdGLTFLoader.h"

//=============================================================================
//
// ModelData
//
//=============================================================================

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ModelData::ModelData()
	: m_originalNodes()
	, m_rootNodeIndices()
	, m_boneNodeIndices()
	, m_meshNodeIndices()
	, m_materials()
	, m_spAnimations()
	, m_filepath("empty")
{
}

//-----------------------------------------------------------------------------
// 読み込み
//-----------------------------------------------------------------------------
bool ModelData::Load(const std::string& filepath)
{
	Release();
	m_filepath = filepath;// パス保存

	std::shared_ptr<KdGLTFModel> model = KdLoadGLTFModel(filepath);
	if (model == nullptr)
		return false;

	// モデルを構成する各要素を作成
	CreateNodes(model);
	CreateMaterials(model);
	CreateAnimations(model);

	return true;
}

//-----------------------------------------------------------------------------
// アニメーションデータ取得：文字列検索
//-----------------------------------------------------------------------------
const std::shared_ptr<AnimationData> ModelData::GetAnimation(const std::string& animName) const
{
	for (auto&& result : m_spAnimations)
		if (result->m_name == animName)
			return result;
	return nullptr;
}

//-----------------------------------------------------------------------------
// 解放
//-----------------------------------------------------------------------------
void ModelData::Release()
{
	m_materials.clear();
	m_originalNodes.clear();

	m_rootNodeIndices.clear();
	m_boneNodeIndices.clear();
	m_meshNodeIndices.clear();
}

//-----------------------------------------------------------------------------
// ノード作成
//-----------------------------------------------------------------------------
void ModelData::CreateNodes(std::shared_ptr<KdGLTFModel>& spGltfModel)
{
	m_originalNodes.resize(spGltfModel->m_nodes.size());

	for (UINT i = 0; i < spGltfModel->m_nodes.size(); i++)
	{
		// ソース元
		const KdGLTFNode& srcNode = spGltfModel->m_nodes[i];

		// 提供先 パラメータ設定
		Node& dstNode = m_originalNodes[i];
		dstNode.m_name				= srcNode.m_name;
		dstNode.m_localTransform	= srcNode.m_localTransform;
		dstNode.m_worldTransform	= srcNode.m_worldTransform;
		dstNode.m_boneInverseWorldMatrix = srcNode.m_inverseBindMatrix;
		dstNode.m_isSkinMesh		= srcNode.m_mesh.m_isSkinMesh;
		dstNode.m_boneIndex			= srcNode.m_boneNodeIndex;
		dstNode.m_parent			= srcNode.m_parent;
		dstNode.m_children			= srcNode.m_children;

		// メッシュ作成
		if (srcNode.m_isMesh)
		{
			dstNode.m_spMesh = std::make_shared<Mesh>();
			dstNode.m_spMesh->Create(srcNode.m_mesh.m_vertices, srcNode.m_mesh.m_faces, srcNode.m_mesh.m_subsets, srcNode.m_mesh.m_isSkinMesh);
			// メッシュノードリストにインデックス登録
			m_meshNodeIndices.push_back(i);
		}
	}

	for (UINT i = 0; i < spGltfModel->m_nodes.size(); i++)
	{
		// ルートノードのIndexリスト
		if (spGltfModel->m_nodes[i].m_parent == -1)
			m_rootNodeIndices.push_back(i);
		// ボーンノードのIndexリスト
		int boneIdx = spGltfModel->m_nodes[i].m_boneNodeIndex;

		if (boneIdx >= 0)
		{
			if (boneIdx >= (int)m_boneNodeIndices.size())
				m_boneNodeIndices.resize(boneIdx + 1);
			m_boneNodeIndices[boneIdx] = i;
		}
	}
}

//-----------------------------------------------------------------------------
// マテリアル作成
//-----------------------------------------------------------------------------
void ModelData::CreateMaterials(std::shared_ptr<KdGLTFModel>& model)
{
	m_materials.resize(model->m_materials.size());

	for (UINT i = 0; i < m_materials.size(); ++i)
	{
		// ソース元
		const KdGLTFMaterial& srcMaterial = model->m_materials[i];

		// 提供先 パラメータ設定
		Material& dstMaterial = m_materials[i];
		dstMaterial.m_name		= srcMaterial.m_name;
		dstMaterial.m_baseColor = srcMaterial.m_baseColor;
		dstMaterial.m_metallic	= srcMaterial.m_metallic;
		dstMaterial.m_roughness = srcMaterial.m_roughness;
		dstMaterial.m_emissive	= srcMaterial.m_emissive;

		//--------------------------------------------------
		// 各テクスチャ
		//--------------------------------------------------
		std::filesystem::path texturepath = m_filepath;
		// 基本色
		if (!srcMaterial.m_baseColorTexture.empty()) {
			dstMaterial.m_baseColorTexture = std::make_shared<Texture>();
			texturepath.replace_filename(srcMaterial.m_baseColorTexture);
			dstMaterial.m_baseColorTexture->Create(texturepath.string());
		}
		// 法線マップ
		if (!srcMaterial.m_normalTexture.empty()) {
			dstMaterial.m_normalTexture = std::make_shared<Texture>();
			texturepath.replace_filename(srcMaterial.m_normalTexture);
			dstMaterial.m_normalTexture->Create(texturepath.string());
		}
		// 金属性 粗さ
		if (!srcMaterial.m_metallicRoughnessTexture.empty()) {
			dstMaterial.m_metallicRoughnessTexture = std::make_shared<Texture>();
			texturepath.replace_filename(srcMaterial.m_metallicRoughnessTexture);
			dstMaterial.m_metallicRoughnessTexture->Create(texturepath.string());
		}
		// エミッシブ
		if (!srcMaterial.m_emissiveTexture.empty()) {
			dstMaterial.m_emissiveTexture = std::make_shared<Texture>();
			texturepath.replace_filename(srcMaterial.m_emissiveTexture);
			dstMaterial.m_emissiveTexture->Create(texturepath.string());
		}
	}
}

//-----------------------------------------------------------------------------
// アニメーション作成
//-----------------------------------------------------------------------------
void ModelData::CreateAnimations(std::shared_ptr<KdGLTFModel>& modeldata)
{
	m_spAnimations.resize(modeldata->m_animations.size());

	for (UINT i = 0; i < m_spAnimations.size(); ++i)
	{
		m_spAnimations[i] = std::make_shared<AnimationData>();

		// ソース元
		const KdGLTFAnimationData& srcAnimation = *modeldata->m_animations[i];

		// 提供先 パラメータ設定
		AnimationData& dstAnimation = *(m_spAnimations[i]);
		dstAnimation.m_name			= srcAnimation.m_name;
		dstAnimation.m_maxLength	= srcAnimation.m_maxLength;

		dstAnimation.m_nodes.resize(srcAnimation.m_nodes.size());
		for (UINT j = 0; j < dstAnimation.m_nodes.size(); ++j)
		{
			dstAnimation.m_nodes[j].m_nodeOffset	= srcAnimation.m_nodes[j]->m_nodeOffset;
			dstAnimation.m_nodes[j].m_translations	= srcAnimation.m_nodes[j]->m_translations;
			dstAnimation.m_nodes[j].m_rotations		= srcAnimation.m_nodes[j]->m_rotations;
			dstAnimation.m_nodes[j].m_scales		= srcAnimation.m_nodes[j]->m_scales;
		}
	}
}



//=============================================================================
//
// ModelWork
//
//=============================================================================

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ModelWork::ModelWork()
	: m_spModel(nullptr)
	, m_coppiedNodes()
	, m_enable(false)
{
}

//-----------------------------------------------------------------------------
// 読み込み
//-----------------------------------------------------------------------------
bool ModelWork::Load(const std::string& fileName)
{
	std::shared_ptr<ModelData> data = std::make_shared<ModelData>();
	if (!data->Load(fileName))
		return false;

	// セット
	SetModel(data);

	return true;
}

//-----------------------------------------------------------------------------
// ノード検索：文字列
//-----------------------------------------------------------------------------
const ModelWork::Node* ModelWork::FindNode(std::string name) const
{
	for (auto&& node : m_coppiedNodes)
	{
		if (node.m_name == name)
			return &node;
	}
	return nullptr;
}

//-----------------------------------------------------------------------------
// 可変ノード検索：文字列
//-----------------------------------------------------------------------------
ModelWork::Node* ModelWork::FindWorkNode(std::string name)
{
	for (auto&& node : m_coppiedNodes)
	{
		if (node.m_name == name)
			return &node;
	}
	return nullptr;
}

//-----------------------------------------------------------------------------
// モデル設定：コピーノードの生成
//-----------------------------------------------------------------------------
void ModelWork::SetModel(const std::shared_ptr<ModelData>& rModel)
{
	m_spModel = rModel;
	UINT nodeSize = rModel->GetOriginalNodes().size();

	// ノードのコピーを生成
	m_coppiedNodes.resize(nodeSize);
	for (UINT i = 0; i < nodeSize; ++i)
		m_coppiedNodes[i].copy(rModel->GetOriginalNodes()[i]);
}

//-----------------------------------------------------------------------------
// ルートノードから各ノードの行列を計算していく
//-----------------------------------------------------------------------------
void ModelWork::CalcNodeMatrices()
{
	if (m_spModel == nullptr) {
		DebugLog("モデルのない行列計算.\n");
		return;
	}

	// 全ボーン行列を書き込み
	for (auto&& nodeIdx : m_spModel->GetRootNodeIndices())
		recCalcNodeMatrices(nodeIdx);

	for (auto&& nodeIdx : m_spModel->GetBoneNodeIndices())
	{
		//if (nodeIdx >= StandardShader::maxBoneBufferSize) { assert(0 && "転送できるボーンの上限数を超えました"); return; }

		// ノード内からボーン情報を取得
		auto& data = m_spModel->GetOriginalNodes()[nodeIdx];
		auto& work = m_coppiedNodes[nodeIdx];

		work.m_worldTransform._41 += 0.01f;

		// ボーン情報からGPUに渡す行列の計算
		//SHADER->m_standardShader.BoneCB().Work().mBones[data.m_boneIndex] = data.m_boneInverseWorldMatrix * work.m_worldTransform;
	}
}

//-----------------------------------------------------------------------------
// ノード行列計算用の再起用関数
//-----------------------------------------------------------------------------
void ModelWork::recCalcNodeMatrices(int nodeIdx, int parentNodeIdx)
{
	auto& data = m_spModel->GetOriginalNodes()[nodeIdx];
	auto& work = m_coppiedNodes[nodeIdx];

	// 親との行列を合成
	if (parentNodeIdx >= 0)
	{
		auto& parent = m_coppiedNodes[data.m_parent];
		work.m_worldTransform = work.m_localTransform * parent.m_worldTransform;
	}
	// 親が居ない場合は親は自分自身とする
	else
	{
		work.m_worldTransform = work.m_localTransform;
	}

	for (auto childNodeIdx : data.m_children)
		recCalcNodeMatrices(childNodeIdx, nodeIdx);
}
