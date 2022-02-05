//-----------------------------------------------------------------------------
// File: CesiumMan.h
//
// 人型クラス スキンメッシュ用にテスト
//-----------------------------------------------------------------------------
#pragma once

class CesiumMan : public Actor
{
public:

	//初期化
	void Initialize() override;
	//更新
	void Update(float deltaTime) override;
	//描画
	void Draw(float deltaTime) override;

private:

	Animator m_animator;

};
