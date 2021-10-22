//-----------------------------------------------------------------------------
// File: Actor.h
//
// C++はC#と違いReflectionがないのでGetComponentの際にはキャスト必須です
// Unity同様TransformはすべてのActorは最初から所持しています
//-----------------------------------------------------------------------------
#pragma once
#include "Transform.h"

// Actor の分別
enum ACTOR_TAG
{
	eUntagged	= 0,
	ePlayer		= 0 << 1,
	eEnemy		= 0 << 2,
	eWeapon		= 0 << 3,
	eBullet		= 0 << 4,
};

// アクタークラス
class Actor
{
public:

	// @brief コンストラクタ
	Actor();

	// @brief デストラクタ
	~Actor() = default;

	// @brief インスタンス生成直後
	virtual void Awake() {};

	// @brief 初期化
	virtual void Initialize() {};

	// @brief jsonファイルのシリアル
	// @param jsonArray
	virtual void Serialize(json11::Json::array& jsonArray);

	// @brief jsonファイルの逆シリアル
	// @param jsonObject 逆シリアルを行うjsonオブジェクト
	virtual void Deserialize(const json11::Json& jsonObject);

	// @brief 終了
	virtual void Finalize() {};

	// @brief 更新
	// @param deltaTime 前フレームからの経過時間
	virtual void Update(float deltaTime);

	// @brief 描画後更新
	// @param deltaTime 前フレームからの経過時間
	virtual void LateUpdate(float deltaTime) {};

	// @brief 描画
	// @param deltaTime 前フレームからの経過時間
	virtual void Draw(float deltaTime);

	// @brief シャドウマップ描画
	// @param deltaTime 前フレームからの経過時間
	virtual void DrawShadowMap(float deltaTime);

	// @brief スプライト描画
	// @param deltaTime 前フレームからの経過時間
	virtual void DrawSprite(float deltaTime) {}

	// @brief 自身を破棄
	void Destroy();

	// レイとメッシュの当たり判定
	bool CheckCollision(const float3& rayPos, const float3& rayDir, float hitRange);

	// モデル読み込み
	void LoadModel(const std::string& filepath);

	//--------------------------------------------------
	// 設定/取得
	//--------------------------------------------------

	// @brief トランスフォームを返す(読み取り専用)
	// @return トランスフォーム(読み取り専用)
	const Transform& GetTransform() const { return m_transform; }

	// @brief トランスフォームを返す
	// @return トランスフォーム
	Transform& GetTransform() { return m_transform; }

protected:

	ModelWork	m_modelWork;	// モデル管理
	Transform	m_transform;	// トランスフォーム

public:

	UINT		g_tag;			// TAG
	std::string g_name;			// オブジェクトの名前

	bool		g_activ;		// 動作？
	bool		g_enable;		// 有効？ ※falseになると消滅します
	bool		g_shadowCaster;	// 影を落とす？

	// UVスクロール
	float2		g_numUVOffset;
	bool		g_isUVScroll;

};

// @brief Actor生成
// @param name 生成したいActorのクラス名
// @return 生成したActor
std::shared_ptr<Actor> GenerateActor(const std::string& name);
