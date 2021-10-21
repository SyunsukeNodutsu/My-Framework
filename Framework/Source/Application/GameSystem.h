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

	// @brief シーンのActorリストに新規Actorを追加
	// @param actor 追加するActor
	void AddActor(std::shared_ptr<Actor> actor);

	// @brief デバッグ用の3D線を追加 TODO: 移動
	// @param pos01 開始座標
	// @param pos02 終了座標
	// @param color 色 デフォルトは白
	void AddDebugLine(const float3& pos01, const float3& pos02, const cfloat4x4 color = cfloat4x4::White);

	// @brief デバッグ用の3D球を追加 TODO: 移動
	// @param pos 座標
	// @param radius 半径(Degree)
	// @param color 色 デフォルトは白
	void AddDebugSphereLine(const float3& pos, const float radius, const cfloat4x4 color = cfloat4x4::White);

public:

	// カメラ管理システム
	CameraSystem g_cameraSystem;

	// シーンのjsonファイルパス
	std::string g_sceneFilepath;

	// 空間分割テスト
	CLiner8TreeManager<Actor> m_actorOctreeManager;
	std::shared_ptr<ObjectForTree<Actor>> m_spActorList_Tree;

private:

	// 登場Actorリスト
	std::list<std::shared_ptr<Actor>> m_spActorList;

	// デバッグライン用の頂点情報
	std::vector<EffectShader::Vertex> m_debugLines;

private:

	// @brief シーンの読み込み
	// @param filepath 読み込むjsonファイルのパス
	// @return 成功...true
	bool LoadScene(const std::string& filepath);

};
