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

	// @brief 単一のインスタンスを返す
	// @return シングルトン・インスタンス
	static GameSystem& GetInstance() {
		static GameSystem instance; return instance;
	}

	// @brief 使用カメラの設定
	// @param camera 設定するカメラ
	void SetCamera(std::shared_ptr<Camera> camera) { m_spCamera = camera; }

	// @brief シーンのActorリストを返す
	// @return Actorリスト
	const std::list<std::shared_ptr<GameObject>>& GetActorList() const {
		return m_spObjectList;
	}

	// @brief 使用カメラを返す
	// @return 使用カメラ
	const std::shared_ptr<Camera>& GetCamera() const { return m_spCamera; }

public:

	FpsTimer				g_fpsTimer;	// FPS管理

private:

	std::shared_ptr<Camera> m_spCamera;	// カメラ

	// 登場GameObjectリスト
	std::list<std::shared_ptr<GameObject>>	m_spObjectList;

	std::shared_ptr<Texture> spTexture;
	std::shared_ptr<SoundWork> m_spSoundWork;

private:

	// @brief コンストラクタ
	GameSystem();

	// @brief GameObjectをシーンに追加
	// @param name 追加するGameObjectのクラス名
	void AddGameObject(const std::string& name);

};

//--------------------------------------------------
// #defines: インスタンスの取得
//--------------------------------------------------
#define GAMESYSTEM GameSystem::GetInstance()
