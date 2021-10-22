//-----------------------------------------------------------------------------
// File: GameProcess.h
//
// ゲームプロセス管理
//-----------------------------------------------------------------------------
#pragma once
#include "../Actor.h"

// ゲームプロセスの管理クラス
class GameProcess : public Actor
{
public:

	// @brief コンストラクタ
	GameProcess();

	// @brief 初期化
	void Initialize() override;

private:


};
