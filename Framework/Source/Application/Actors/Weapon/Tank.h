//-----------------------------------------------------------------------------
// File: Tank.h
// 
// 戦車クラス 親子構造
// キャタピラx2, タイヤx10, ミニタイヤx4, 砲塔, 主砲
// TODO: Bodyも部品として扱う
//-----------------------------------------------------------------------------
#pragma once
#include "../../Application.h"

class TankParts;

// 戦車クラス
class Tank : public Actor
{
public:

	// @brief コンストラクタ
	Tank();

	// @brief インスタンス生成直後
	void Awake() override;

	// @brief jsonファイルの逆シリアル
	// @param jsonObject 逆シリアルを行うjsonオブジェクト
	void Deserialize(const json11::Json& jsonObject) override;

	// @brief 更新
	// @param deltaTime 前フレームからの経過時間
	void Update(float deltaTime) override;

	// @brief 描画
	// @param deltaTime 前フレームからの経過時間
	void Draw(float deltaTime) override;

	// @brief スプライト描画
	// @param deltaTime 前フレームからの経過時間
	void DrawSprite(float deltaTime) override;

	//--------------------------------------------------
	// 設定/取得
	//--------------------------------------------------

	// @brief カメラの回転ベクトル(Degree)を返す
	// @return カメラの回転ベクトル(Degree)
	const float2& GetCameraAngle() const { return m_cameraAngle; }

private:

	std::shared_ptr<TPSCamera>	m_spCamera3rd;	// 三人称カメラ
	std::shared_ptr<FPSCamera>	m_spCamera1st;	// 一人称カメラ
	std::shared_ptr<TankParts>	m_spTankParts;	// 戦車の部品

	std::shared_ptr<SoundWork3D> m_runSound3D;	// 走行時の音
	std::shared_ptr<SoundWork3D> m_shotSound3D;	// 射撃時の音

	float m_moveSpeed;	// 移動速度
	float m_rotateSpeed;// 回転速度

	float2 m_cameraAngle;// カメラの回転角度

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
		virtual void Draw(Tank& owner, float deltaTime) = 0;
		virtual void DrawSprite(Tank& owner, float deltaTime) = 0;
	};
	class StateNull : public StateBase {
	public:
		virtual void Update(Tank& owner, float deltaTime) override {}
		virtual void Draw(Tank& owner, float deltaTime) override {}
		virtual void DrawSprite(Tank& owner, float deltaTime) override {}
	};
	class State3rd : public StateBase {
	public:
		virtual void Update(Tank& owner, float deltaTime) override;
		virtual void Draw(Tank& owner, float deltaTime) override;
		virtual void DrawSprite(Tank& owner, float deltaTime) override;
	};
	class State1st : public StateBase {
	public:
		virtual void Update(Tank& owner, float deltaTime) override;
		virtual void Draw(Tank& owner, float deltaTime) override {}
		virtual void DrawSprite(Tank& owner, float deltaTime) override;
	};
	std::shared_ptr<StateBase> m_spState;

};
