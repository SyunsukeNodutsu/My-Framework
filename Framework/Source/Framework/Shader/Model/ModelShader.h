//-----------------------------------------------------------------------------
// File: ModelShader.h
//
// モデル描画用のShader
//-----------------------------------------------------------------------------
#pragma once
#include "../Shader.h"

// モデル描画用 PBR
class ModelShader : public Shader
{
public:

	// コンストラクタ
	ModelShader();

	// デストラクタ
	~ModelShader() = default;

	// @brief 初期化
	// @return 成功...true 失敗...false
	bool Initialize();

	// @brief 開始
	void Begin();

	// @brief モデル描画
	// @param model 描画するモデル
	// @param worldMatrix モデルのワールド行列
	void DrawModel(const ModelWork& model, const mfloat4x4& worldMatrix = mfloat4x4::Identity);

	// @brief メッシュ描画
	// @param mesh 描画するメッシュ
	// @param materials 使用する材質配列
	void DrawMesh(const Mesh* mesh, const std::vector<Material>& materials);

private:

	// マテリアル
	struct cdMaterial
	{
		float4	m_baseColor;
		float3	m_emissive;
		float	m_metallic;
		float	m_roughness;
		float	tmp[3];
	};
	ConstantBuffer<cdMaterial> m_cd11Material;

private:

	// @brief OBBを3D線でデバッグ描画
	// @param obb
	void DrawOBB(const DirectX::BoundingOrientedBox& obb);

};
