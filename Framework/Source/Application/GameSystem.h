//-----------------------------------------------------------------------------
// File: GameSystem.h
//
// ゲームの実行管理
// TODO: Visualstdio側でProjectを分ける
//-----------------------------------------------------------------------------
#pragma once

class GameObject;

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
	
	// @brief 使用カメラの設定
	// @param camera 設定するカメラ
	void SetCamera(std::shared_ptr<Camera> camera) { m_spCamera = camera; }

	// @brief シーンのActorリストを返す
	// @return Actorリスト
	const std::list<std::shared_ptr<GameObject>>& GetActorList() const {
		return m_spActorList;
	}

	// @brief 使用カメラを返す
	// @return 使用カメラ
	const std::shared_ptr<Camera>& GetCamera() const { return m_spCamera; }

private:

	// 使用カメラ
	std::shared_ptr<Camera> m_spCamera;

	// 登場Actorリスト
	std::list<std::shared_ptr<GameObject>>	m_spActorList;

private:

	// @brief Actorをシーンに追加
	// @param name 追加するActorのクラス名
	void AddActor(const std::string& name);

};
