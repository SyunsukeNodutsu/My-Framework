//-----------------------------------------------------------------------------
// File: Tank.h
// 
// 戦車クラス 親子構造
//-----------------------------------------------------------------------------
#pragma once
#include "../Application.h"

class TankParts;

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

	//--------------------------------------------------
	// 設定.取得
	//--------------------------------------------------

	// @brief カメラのY回転を返す
	// @return カメラのY回転
	float GetCameraAngleY() const { return m_cameraAngleY; }

private:

	std::shared_ptr<TPSCamera>	m_spCamera3rd;	// 三人称
	std::shared_ptr<FPSCamera>	m_spCamera1st;	// 一人称
	std::shared_ptr<TankParts>	m_spTankParts;	// 部品

	std::shared_ptr<SoundWork3D> m_runSound3D;	// 走行時の音
	std::shared_ptr<SoundWork3D> m_shotSound3D;	// 射撃時の音

	float m_moveSpeed;	// 移動速度
	float m_rotateSpeed;// 回転速度

	float m_cameraAngleY;// カメラのY角度

private:

	// @brief 移動更新
	// @param deltaTime 前フレームからの経過時間
	void UpdateMove(float deltaTime);

	// @brief 回転更新
	// @param deltaTime 前フレームからの経過時間
	void UpdateRotate(float deltaTime);

	// @brief 走行の音更新
	// @param state1st 一人称かどうか ※車内の方が音が大きいはず
	void UpdateRunSound(bool state1st);

	// @brief 射撃更新
	// @param state1st 一人称かどうか ※射撃音のフィルターに使用
	void UpdateShot(bool state1st);

private:

	class StateBase {
	public:
		virtual void Update(Tank& owner, float deltaTime) = 0;
	};
	class StateNull : public StateBase {
	public:
		virtual void Update(Tank& owner, float deltaTime) override {}
	};
	class State3rd : public StateBase {
	public:
		virtual void Update(Tank& owner, float deltaTime) override;
	};
	class State1st : public StateBase {
	public:
		virtual void Update(Tank& owner, float deltaTime) override;
	};
	std::shared_ptr<StateBase> m_spState;

};
