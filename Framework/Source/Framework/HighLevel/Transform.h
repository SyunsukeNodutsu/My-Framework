//-----------------------------------------------------------------------------
// File: Transform.h
//
// オブジェクトの位置 回転 スケールを扱う
// Actorに最初から所持させる
//-----------------------------------------------------------------------------
#pragma once

// オブジェクトの位置 回転 スケールを扱うクラス
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
	float3 GetPosition() const { return m_worldPosition; }

	// @brief 回転を返す
	// @return ワールド行列を構成する回転ベクトル
	float3 GetAngle() const { return m_worldAngle; }

	// @brief拡縮を返す
	// @return ワールド行列を構成する拡縮ベクトル
	float3 GetScale() const { return m_worldScale; }

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
