﻿//-----------------------------------------------------------------------------
// File: TankParts.h
//
// 戦車の部品クラス
//-----------------------------------------------------------------------------
#pragma once

class Tank;

// 戦車の部品クラス
class TankParts
{
public:

	// @brief コンストラクタ
	// @param owner 部品オーナー
	TankParts(Tank& owner);

	// @brief 初期化
	void Initialize();

	// @brief jsonファイルの逆シリアル
	// @param jsonObject 逆シリアルを行うjsonオブジェクト
	void Deserialize(const json11::Json& jsonObject);

	// @brief 更新
	// @param deltaTime 前フレームからの経過時間
	// @param moveSpeed オーナーの移動速度
	// @param rotSpeed オーナーの回転速度
	void Update(float deltaTime, float moveSpeed, float rotSpeed);

	// @brief 描画
	// @param deltaTime 前フレームからの経過時間
	void Draw(float deltaTime);

	//--------------------------------------------------
	// 設定/取得
	//--------------------------------------------------

	// @brief 砲塔のワールド行列を返す
	// @return 砲塔のワールド行列
	const mfloat4x4& GetTurretMatrix() const {
		return m_turret.GetTransform().GetWorldMatrix();
	}

	// @brief 主砲のワールド行列を返す
	// @return 主砲のワールド行列
	const mfloat4x4& GetMainGunMatrix() const {
		return m_mainGun.GetTransform().GetWorldMatrix();
	}

private:

	// オーナー
	Tank& m_owner;

	// キャタピラ
	float m_uvoffsetR, m_uvoffsetL;
	Actor m_trackR; mfloat4x4 m_trackOffsetR;
	Actor m_trackL; mfloat4x4 m_trackOffsetL;

	// 砲塔 主砲
	Actor m_turret; mfloat4x4 m_turretOffset;
	Actor m_mainGun; mfloat4x4 m_mainGunOffset;

	// 車輪 左右5個ずつ
	float m_tireRotR, m_tireRotL;
	Actor m_tireR[5]; mfloat4x4 m_tireOffsetR[5];
	Actor m_tireL[5]; mfloat4x4 m_tireOffsetL[5];

	// 端の小さい車輪 左右2つずつ
	Actor m_miniTireR[2]; mfloat4x4 m_miniTireOffsetR[2];
	Actor m_miniTireL[2]; mfloat4x4 m_miniTireOffsetL[2];

private:

	// @brief キャタピラ更新
	// @param deltaTime 前フレームからの経過時間
	// @param moveSpeed オーナーの移動速度
	// @param rotSpeed オーナーの回転速度
	void UpdateTrack(float deltaTime, float moveSpeed, float rotSpeed);

	// @brief 砲塔更新
	// @param deltaTime 前フレームからの経過時間
	void UpdateTurret(float deltaTime);

	// @brief 主砲更新
	// @param deltaTime 前フレームからの経過時間
	void UpdateMainGun(float deltaTime);

	// @brief タイヤ更新
	// @param moveSpeed オーナーの移動速度
	// @param rotSpeed オーナーの回転速度
	void UpdateTire(float deltaTime, float moveSpeed, float rotSpeed);

};
