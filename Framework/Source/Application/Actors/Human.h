﻿//-----------------------------------------------------------------------------
// File: Human.h
//
// 人型クラス
//-----------------------------------------------------------------------------
#pragma once
#include "../Camera/TPSCamera.h"

// 人型クラス
class Human : public Actor
{
public:

	// @brief コンストラクタ
	Human();

	// @brief 初期化
	void Initialize()override;

	// @brief 終了
	void Finalize() override;

	// @brief 更新
	void Update(float deltaTime) override;

	// @brief 描画
	void Draw(float deltaTime) override;

private:

	std::shared_ptr<TPSCamera> m_spCamera;
	Animator	m_animator;
	float		m_zoom;
	float3		m_rotation;
	float		m_operationStopTick;

private:

	// @brief 移動更新
	// @param deltaTime デルタタイム
	void UpdateMove(float deltaTime);

	// @brief 回転更新
	// @param deltaTime デルタタイム
	void UpdateRotate(float deltaTime);

	// @brief 衝突判定更新
	// @param
	void UpdateCollision();

	void CheckBump();

	//ステートマシン------------------------------------------------------------------
	class StateBase {
	public:
		virtual void Update(float deltaTime, Human& owner) = 0;
	};
	class StateNull : public StateBase {
	public:
		virtual void Update(float deltaTime, Human& owner) override {}
	};
	class StateWait : public StateBase{
	public:
		virtual void Update(float deltaTime, Human& owner) override;
	};
	class StateMove : public StateBase {
	public:
		virtual void Update(float deltaTime, Human& owner) override;
	};
	std::shared_ptr<StateBase> m_spState;

};
