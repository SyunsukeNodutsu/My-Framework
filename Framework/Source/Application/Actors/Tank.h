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

	std::shared_ptr<TPSCamera> m_spCamera;
	Actor m_tire;
	mfloat4x4 m_tireOffset;

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
