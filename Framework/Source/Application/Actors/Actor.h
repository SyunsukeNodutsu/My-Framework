//-----------------------------------------------------------------------------
// File: Actor.h
//
// C++はC#と違いReflectionがないのでGetComponentの際にはキャスト必須です
// Unity同様TransformはすべてのActorは最初から所持しています
//-----------------------------------------------------------------------------
#pragma once

// オブジェクトの位置 回転 スケールを扱う
class Transform
{
public:

	// @brief コンストラクタ
	Transform();

	// @brief デストラクタ
	~Transform() = default;

	//--------------------------------------------------
	// 取得
	//--------------------------------------------------

	// @brief 行列を返す
	// @return ワールド行列
	const mfloat4x4& GetWorldMatrix() const;

	// @brief 座標を返す
	// @return ワールド行列を構成する座標ベクトル
	const float3 GetPosition() const { return m_worldPosition; }

	// @brief 回転を返す
	// @return ワールド行列を構成する回転ベクトル
	const float3 GetAngle() const { return m_worldAngle; }

	// @brief拡縮を返す
	// @return ワールド行列を構成する拡縮ベクトル
	const float3 GetScale() const { return m_worldScale; }

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------

	// @brief 行列の設定
	// @param matrix 設定する行列
	void SetWorldMatrix(const mfloat4x4& matrix);

	// @brief 座標の設定
	// @param position
	void SetPosition(const float3& position) {
		m_worldPosition = position;
		m_needUpdateMatrix = true;
	}

	// @brief 回転の設定
	// @param angle 設定するDegree度
	void SetAngle(const float3& angle) {
		m_worldAngle = angle;
		m_needUpdateMatrix = true;
	}

	// @brief 拡縮の設定
	// @param scale
	void SetScale(const float3& scale) {
		m_worldScale = scale;
		m_needUpdateMatrix = true;
	}

private:

	float3 m_worldPosition;	// ワールド行列を構成する座標ベクトル
	float3 m_worldAngle;	// ワールド行列を構成する回転ベクトル
	float3 m_worldScale;	// ワールド行列を構成する拡大

	// 行列保存用
	mutable mfloat4x4	m_mWorkWorld;
	mutable bool		m_needUpdateMatrix;	// 行列を更新する必要があるか？
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

	// @brief 終了
	virtual void Finalize() {};

	// @brief 更新
	virtual void Update(float deltaTime);

	// @brief 描画後更新
	virtual void LateUpdate(float deltaTime) {};

	// @brief 描画
	virtual void Draw(float deltaTime);

	// @brief 自身を破棄
	void Destroy();

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------

	// @brief 動作させるかを設定
	// @param activ 設定する値
	void SetActive(bool activ) { m_isActiv = activ; }

	// モデル読み込み
	void LoadModel(const std::string& filepath) {
		auto model = std::make_shared<ModelData>();
		if (model->Load(filepath))
			m_modelWork.SetModel(model);
	}

	//--------------------------------------------------
	// 取得
	//--------------------------------------------------

	// @brief トランスフォームを返す(読み取り専用)
	// @return トランスフォーム(読み取り専用)
	const Transform& GetTransform() const { return m_transform; }

	// @brief トランスフォームの取得
	// @return トランスフォーム
	Transform& GetTransform() { return m_transform; }

	// @brief 動作中かどうかを返す
	// @return 動作中...true
	bool IsActiv() const { return m_isActiv; }

	// @brief 有効かどうかを返す
	// @return 有効...true 無効...false
	bool IsEnable() const { return m_isEnable; }

	// @brief 設定された名前を返す
	// @return Actor name
	const std::string& GetName() const { return m_name; }

protected:

	ModelWork	m_modelWork;	// モデル管理
	Transform	m_transform;	// トランスフォーム
	bool		m_isActiv;		// 動作？
	bool		m_isEnable;		// 有効？ ※falseになると消滅します
	std::string m_name;			// オブジェクトの名前

};

// @brief Actor生成
// @param name 生成したいActorのクラス名
// @return 生成したActor
std::shared_ptr<Actor> GenerateActor(const std::string& name);
