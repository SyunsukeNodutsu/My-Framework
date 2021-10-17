//-----------------------------------------------------------------------------
// File: TankParts.h
//
// 戦車の部品クラス
//-----------------------------------------------------------------------------
#pragma once
#include "Actor.h"

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

	// @brief 更新
	// @param deltaTime 前フレームからの経過時間
	void Update(float deltaTime, float moveSpeed, float rotSpeed);

	// @brief 描画
	// @param deltaTime 前フレームからの経過時間
	void Draw(float deltaTime);

	// @brief 主砲のワールド行列を返す
	// @return 主砲のワールド行列
	const mfloat4x4& GetMainGunMatrix() const { return m_mainGun.GetTransform().GetWorldMatrix(); }

private:

	// オーナー
	Tank& m_owner;

	// キャタピラ
	Actor m_trackR; mfloat4x4 m_trackOffsetR;
	Actor m_trackL; mfloat4x4 m_trackOffsetL;

	// 砲塔 主砲
	Actor m_turret; mfloat4x4 m_turretOffset;
	Actor m_mainGun; mfloat4x4 m_mainGunOffset;

	// 車輪 左右5個ずつ
	Actor m_tireR[5]; mfloat4x4 m_tireOffsetR[5];
	Actor m_tireL[5]; mfloat4x4 m_tireOffsetL[5];

	// 端の小さい車輪 左右2つずつ
	Actor m_miniTireR[2]; mfloat4x4 m_miniTireOffsetR[2];
	Actor m_miniTireL[2]; mfloat4x4 m_miniTireOffsetL[2];

};
