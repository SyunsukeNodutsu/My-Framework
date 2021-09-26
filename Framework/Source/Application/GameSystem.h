//-----------------------------------------------------------------------------
// File: GameSystem.h
//
// ゲームの実行管理
// TODO: Visualstdio側でProjectを分ける
//-----------------------------------------------------------------------------
#pragma once
#include "Camera/CameraSystem.h"

class Actor;

// ゲームの実行管理クラス
class GameSystem
{
public:

	// @brief コンストラクタ
	GameSystem();

	// @brief 初期化
	void Initialize();

	// @brief 終了
	void Finalize();

	// @brief 更新
	void Update();

	// @brief 描画後更新
	void LateUpdate();

	// @brief 3D描画
	void Draw();

	// @brief 2D描画
	void Draw2D();

	//--------------------------------------------------
	// 取得・設定
	//--------------------------------------------------
	
	// @brief シーンのActorリストを返す
	// @return Actorリスト
	const std::list<std::shared_ptr<Actor>>& GetActorList() const {
		return m_spActorList;
	}

public:

	// カメラ管理システム
	CameraSystem g_cameraSystem;

private:

	// 登場Actorリスト
	std::list<std::shared_ptr<Actor>> m_spActorList;

private:

	// @brief Actorをシーンに追加
	// @param name 追加するActorのクラス名
	void AddActor(const std::string& name);

};
