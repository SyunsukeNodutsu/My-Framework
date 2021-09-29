//-----------------------------------------------------------------------------
// File: TankBullet.h
// 
// 戦車の弾
//-----------------------------------------------------------------------------
#pragma once
#include "../Application.h"

class Tank;

// 戦車の弾クラス
class TankBullet : Actor
{
public:

	// @brief コンストラクタ
	TankBullet(Tank owner);

	// @brief 爆発
	void Explosion();

private:

	// オーナー
	Tank& m_owner;

	// 着弾時の音
	std::shared_ptr<SoundWork3D> m_expSound3D;

};
