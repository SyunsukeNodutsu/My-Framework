﻿#pragma once

class FPSCamera : public Camera
{
public:

	// @brief コンストラクタ
	FPSCamera();

	// @brief 初期化
	void Initialize();

	// @brief 更新
	void Update();

	//--------------------------------------------------
	// 設定.取得
	//--------------------------------------------------

	// @brief 視線用のローカル座標の指定
	void SetLocalPos(float3 pos) { m_localPos = pos; }

	// @brief 有効かどうかを設定
	void SetEnable(bool enable) {
		m_enable = enable;
		RAW_INPUT.GetMouse()->SetCursorShow(!enable);
	}

	// @brief FPSカメラのカメラ行列を設定(override)
	// @param cameraMat 設定するカメラ行列
	virtual void SetCameraMatrix(const mfloat4x4& cameraMat) override;

	// @brief カメラのX軸の制限を設定
	void SetClampAngleX(float minAng, float maxAng) {
		m_minAngleX = minAng; m_maxAngleX = maxAng;
	}

	// @brief 角度の設定
	void SetAngle(float angX, float angY) {
		m_degAngle.x = angX;
		m_degAngle.y = angY;
	}

	// @brief 回転行列を返す
	// @return
	const mfloat4x4 GetRotationMatrix();

	// @brief を返す
	// @return
	const mfloat4x4 GetRotationYMatrix();

	// @brief 回転ベクトルを返す
	// @return
	const float3& GetRotationAngles() const { return m_degAngle; }

	// @brief を返す
	// @return
	const float GetRotationYAngle() const { return 0; }

	//--------------------------------------------------
	// static
	//--------------------------------------------------

	// 画面中央に留まるカーソル位置
	static const POINT s_fixMousePos;

protected:

	// 視線用のローカル座標
	//(キャラクターの原点から指定の位置へ移動する座標ベクトル)
	float3 m_localPos;

private:

	// 各軸の回転情報
	float3 m_degAngle;

	// X軸の回転を制限する
	float m_minAngleX; // 上向きの制限
	float m_maxAngleX; // 下向きの制限

	bool m_enable;

};
