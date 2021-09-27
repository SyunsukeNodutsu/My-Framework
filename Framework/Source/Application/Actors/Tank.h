//-----------------------------------------------------------------------------
// File: Tank.h
// 
// 戦車クラス 親子構造
//-----------------------------------------------------------------------------
#pragma once
#include "Actor.h"
#include "../Camera/TPSCamera.h"

// 戦車クラス
class Tank : public Actor
{
public:

	// @brief コンストラクタ
	Tank();

	// @brief インスタンス生成直後
	void Awake() override;

	// @brief 更新
	// @param deltaTime 前フレームからの経過時間
	void Update(float deltaTime) override;

	// @brief 描画
	// @param deltaTime 前フレームからの経過時間
	void Draw(float deltaTime) override;

private:

	std::shared_ptr<TPSCamera> m_spCamera;// 使用カメラ
	float m_moveSpeed;// 移動速度
	float m_rotateSpeed;// 回転速度

	// 親子
	Actor m_trackR; mfloat4x4 m_trackOffsetR;
	Actor m_trackL; mfloat4x4 m_trackOffsetL;

	Actor m_turret; mfloat4x4 m_turretOffset;
	Actor m_mainGun; mfloat4x4 m_mainGunOffset;

private:

	// @brief 移動更新
	void UpdateMove(float deltaTime);

	// @brief 回転更新
	void UpdateRotate(float deltaTime);

private:

	class StateBase {
	public:
		virtual void Update(float deltaTime, Tank& owner) = 0;
	};
	class StateNull : public StateBase {
	public:
		virtual void Update(float deltaTime, Tank& owner) override {}
	};
	class StateWait : public StateBase {
	public:
		virtual void Update(float deltaTime, Tank& owner) override;
	};
	class StateMove : public StateBase {
	public:
		virtual void Update(float deltaTime, Tank& owner) override;
	};
	std::shared_ptr<StateBase> m_spState;

};
