#include "Actor.h"
#include "../../Application/main.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Actor::Actor()
	: m_modelWork()
	, m_transform()
	, m_modelFilepath()
	, m_prefabFilepath()
	, g_tag(ACTOR_TAG::eUntagged)
	, g_name("empty")
	, g_activ(true)
	, g_enable(true)
	, g_shadowCaster(true)
	, g_numUVOffset(float2::Zero)
	, g_isUVScroll(false)
{
}

//-----------------------------------------------------------------------------
// jsonファイルのシリアル
//-----------------------------------------------------------------------------
void Actor::Serialize(json11::Json::array& jsonArray)
{
	json11::Json::object object = {};

	object["class_name"] = g_name;
	object["tag"] = static_cast<int>(g_tag);
	object["model_filepath"] = m_modelFilepath;
	object["prefab_filepath"] = m_prefabFilepath;

	// 座標
	json11::Json::array position(3);
	position[0] = m_transform.GetPosition().x;
	position[1] = m_transform.GetPosition().y;
	position[2] = m_transform.GetPosition().z;
	object["position"] = position;
	// 回転
	json11::Json::array rotation(3);
	rotation[0] = m_transform.GetAngle().x * ToDegrees;
	rotation[1] = m_transform.GetAngle().y * ToDegrees;
	rotation[2] = m_transform.GetAngle().z * ToDegrees;
	object["rotation"] = rotation;
	// 拡縮
	json11::Json::array scaling(3);
	scaling[0] = m_transform.GetScale().x;
	scaling[1] = m_transform.GetScale().y;
	scaling[2] = m_transform.GetScale().z;
	object["scaling"] = scaling;

	// json arrayに追加
	jsonArray.push_back(object);
}

//-----------------------------------------------------------------------------
// jsonファイルの逆シリアル
//-----------------------------------------------------------------------------
void Actor::Deserialize(const json11::Json& jsonObject)
{
	if (jsonObject.is_null()) return;

	if (!jsonObject["name"].is_null()) {
		g_name = jsonObject["name"].string_value();
	}
	if (!jsonObject["tag"].is_null()) {
		g_tag = jsonObject["tag"].int_value();
	}
	if (!jsonObject["model_filepath"].is_null()) {
		m_modelFilepath = jsonObject["model_filepath"].string_value();
		LoadModel(m_modelFilepath);
	}
	if (!jsonObject["prefab_filepath"].is_null()) {
		m_prefabFilepath = jsonObject["prefab_filepath"].string_value();
	}

	// 座標
	const std::vector<json11::Json>& position = jsonObject["position"].array_items();
	if (position.size() == 3) {
		m_transform.SetPosition(float3(
			(float)position[0].number_value(),
			(float)position[1].number_value(),
			(float)position[2].number_value()
		));
	}
	// 回転
	const std::vector<json11::Json>& rotation = jsonObject["rotation"].array_items();
	if (rotation.size() == 3) {
		m_transform.SetAngle(float3(
			(float)rotation[0].number_value(),
			(float)rotation[1].number_value(),
			(float)rotation[2].number_value()
		));
	}
	// 拡縮
	const std::vector<json11::Json>& scaling = jsonObject["scaling"].array_items();
	if (scaling.size() == 3) {
		m_transform.SetScale(float3(
			(float)scaling[0].number_value(),
			(float)scaling[1].number_value(),
			(float)scaling[2].number_value()
		));
	}

	m_modelWork.CalcNodeMatrices();
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void Actor::Update(float deltaTime)
{

}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void Actor::Draw(float deltaTime)
{
	// 定数バッファ
	{
		// カメラ座標
		float3 cameraPos = RENDERER.Getcb9().Get().m_camera_matrix.Translation();
		// カメラとの距離
		float dist = float2::Distance(
			float2(cameraPos.x, cameraPos.z),
			float2(m_transform.GetPosition().x, m_transform.GetPosition().z)
		);
		RENDERER.Getcb8().Work().m_dist_to_eye = dist;

		// UVスクロール
		RENDERER.Getcb8().Work().m_uv_offset = float2::Zero;
		if (g_isUVScroll)
			RENDERER.Getcb8().Work().m_uv_offset += g_numUVOffset;
		else
			RENDERER.Getcb8().Work().m_uv_offset = float2::Zero;

		RENDERER.Getcb8().Write();
	}

	// もうShaderは固定でいいかも
	SHADER.GetModelShader().DrawModel(m_modelWork, m_transform.GetWorldMatrix());
}

//-----------------------------------------------------------------------------
// シャドウマップ描画
//-----------------------------------------------------------------------------
void Actor::DrawShadowMap(float deltaTime)
{
	if (!g_shadowCaster) return;

	SHADER.GetShadowMapShader().DrawModel(m_modelWork, m_transform.GetWorldMatrix());
}

//-----------------------------------------------------------------------------
// レイとメッシュの当たり判定
//-----------------------------------------------------------------------------
bool Actor::CheckCollision(const float3& rayPos, const float3& rayDir, float hitRange, RayResult& result)
{
	const auto& data = m_modelWork.GetData();
	if (data == nullptr) return false;

	for (const auto& index : data->GetMeshNodeIndices())
	{
		const auto& mesh = m_modelWork.GetMesh(index);
		const auto& node = m_modelWork.GetNodes()[index];

		RayResult tmpResult = {};
		Collision::RayToMesh(rayPos, rayDir, hitRange, *(mesh.get()), node.m_localTransform * m_transform.GetWorldMatrix(), &tmpResult);

		// 最も近いnodeを優先
		if (tmpResult.m_distance < result.m_distance)
			result = tmpResult;
	}

	return result.m_hit;
}

//-----------------------------------------------------------------------------
// 球とメッシュの当たり判定
//-----------------------------------------------------------------------------
bool Actor::CheckCollision(const float3& centerPos, const float radius, float3& resultPos)
{
	const auto& data = m_modelWork.GetData();
	if (data == nullptr) return false;

	bool hit = false;
	float3 pushedFromNodesPos = centerPos;

	for (const auto& index : data->GetMeshNodeIndices())
	{
		const auto& mesh = m_modelWork.GetMesh(index);
		const auto& node = m_modelWork.GetNodes()[index];

		if (mesh == nullptr) continue;
		hit = Collision::SphereToMesh(pushedFromNodesPos, radius, *(mesh.get()), node.m_localTransform * m_transform.GetWorldMatrix(), pushedFromNodesPos);
	}

	if (hit)
	{
		resultPos = pushedFromNodesPos - centerPos;
		ApplicationChilled::GetApplication()->g_imGuiSystem->AddLog("Hit. CheckCollision");
	}

	return hit;
}

//-----------------------------------------------------------------------------
// モデル読み込み
//-----------------------------------------------------------------------------
void Actor::LoadModel(const std::string& filepath)
{
	const auto& model = RES_FAC.GetModelData(filepath);
	if (model) m_modelWork.SetModel(model);
}
