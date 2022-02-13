//-----------------------------------------------------------------------------
//File: GameProcess.h
//
//ゲームプロセス管理
//-----------------------------------------------------------------------------
#pragma once

//ゲームプロセスの管理クラス
class GameProcess : public Actor
{
public:

	//@brief コンストラクタ
	GameProcess();

	//@brief 初期化
	void Initialize() override;

	//@brief 更新
	void Update(float) override;

private:

	EmitData m_deta;

};
