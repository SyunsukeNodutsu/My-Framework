//-----------------------------------------------------------------------------
// File: TankBullet.h
// 
// 戦車の弾
//-----------------------------------------------------------------------------
#pragma once
#include "../Application.h"

class Tank;

// 戦車の弾クラス
class TankBullet : public Actor
{
public:

	// @brief コンストラクタ
	TankBullet(Tank owner, float3 position, float3 axis);

	// @brief 更新
	// @param deltaTime 前フレームからの経過時間
	virtual void Update(float deltaTime) override;

private:

	Tank&	m_owner;	// オーナー
	float3	m_axisZ;	// 移動方向
	float	m_speed;	// 移動速度
	float	m_lifeSpan;	// 生存期間(deltaTime減算)

	float3 m_prevPos;	// 前フレームの座標 ※衝突判定で使用

	// 着弾時の音
	std::shared_ptr<SoundWork3D> m_expSound3D;

private:

	// @brief 爆発
	void Explosion();

	// @brief 衝突判定更新
	void UpdateCollision();

};
