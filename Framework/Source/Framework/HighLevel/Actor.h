//-----------------------------------------------------------------------------
// File: Actor.h
//
// C++はC#と違いReflectionがないのでGetComponentの際にはキャスト必須です
// Unity同様TransformはすべてのActorは最初から所持しています
// 
// TODO: データとロジックで分ける
//-----------------------------------------------------------------------------
#pragma once
#include "Transform.h"
#include "ApplicationChilled.h"

// Actor の分別
enum ACTOR_TAG
{
	UNTAGGED	= 0,
	PLAYER		= 1,
	ENEMY		= 1 << 1,
	WEPON		= 1 << 2,
	GROUND		= 1 << 3,
};

// アクタークラス
class Actor : public ApplicationChilled
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

	// @brief データを逆シリアル化
	// @param jsonObject 逆シリアルの出力先 jsonオブジェクト
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

	// @brief レイとメッシュの当たり判定
	// @param rayPos レイの座標
	// @param rayDir レイの方向
	// @param hitRange 判定を行う距離
	// @param result レイ判定の結果
	// @return 衝突...true
	bool CheckCollision(const float3& rayPos, const float3& rayDir, float hitRange, RayResult& result);

	// @brief 球とメッシュの当たり判定
	// @param centerPos 中心座標
	// @param radius 球の半径
	// @param resultPos 移動した球の座標
	// @return 衝突...true
	bool CheckCollision(const float3& centerPos, const float radius, float3& resultPos);

	// @brief モデル読み込み
	// @param filepath 読み込むモデルのファイルパス
	void LoadModel(const std::string& filepath);

	//--------------------------------------------------
	// 各イベント
	//--------------------------------------------------

	// @brief 破棄されたタイミングで呼ばれる
	// @note 即座にeraseされます スレッド関連は動作を保証されません
	virtual void OnDestroy() {}

	// @brief 一時停止のタイミングで呼ばれる
	virtual void OnPause() {}

	// @brief 再開のタイミングで呼ばれる
	virtual void OnResume() {}

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

	ModelWork m_modelWork;				//モデル管理
	Transform m_transform;				//トランスフォーム
	std::string m_modelFilepath;		//モデルのファイルパス
	std::string m_prefabFilepath;		//プレハブのファイルパス

public:

	UINT g_tag;							//判別用TAG
	std::string g_name;					//オブジェクトの名前
	bool g_activ;						//動作するかどうか
	bool g_enable;						//有効かどうか ※falseになると消滅
	bool g_shadowCaster;				//影を落とすかどうか

	//UVスクロール ※TODO:ファイル移動
	float2 g_numUVOffset;
	bool g_isUVScroll;

};

//@brief Actor生成
//@param name 生成したいActorのクラス名
//@return 生成したActor
std::shared_ptr<Actor> GenerateActor(const std::string& name);
