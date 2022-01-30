//-----------------------------------------------------------------------------
// File: Camera.h
//
// カメラシステム
// 優先度から使用カメラを遷移させたり
//-----------------------------------------------------------------------------
#pragma once
#include "../Editor/EditorCamera.h"

// カメラの遷移モード
enum class ChangeMode
{
	eImmediate,	// 遷移アニメーションなし
	eLiner,		// 線形
};

// カメラシステムクラス
class CameraSystem
{
public:

	// @brief コンストラクタ
	CameraSystem();

	// @brief 更新
	// @param deltaTime 前フレームからの経過時間
	void Update(float deltaTime);

	// @brief カメラ情報をGPUに転送
	void SetToDevice();

	// @brief 編集カメラ以外のカメラ一覧をクリア
	void ResetCameraList();

	//--------------------------------------------------
	// 取得/設定
	//--------------------------------------------------

	// @brief カメラを管理リストに追加
	// @param camera 追加するカメラ
	void AddCameraList(std::shared_ptr<Camera> camera);

	// @brief カメラ管理リストを返す
	// @return カメラ管理リスト(読み取り専用)
	const std::list<std::shared_ptr<Camera>>& GetCameraList() const {
		return m_cameraList;
	}

	// @brief 現在使用しているカメラを返す
	// @return 使用カメラ ※最も優先度の高いカメラ(遷移中を除く)
	const std::shared_ptr<Camera>& GetCamera() const;

	// @brief カメラを名前検索で返す
	// @return 発見...カメラ 見つからない...nullptr
	std::shared_ptr<Camera> SearchCamera(std::string name) const;

	// @brief 全てのカメラに有効かどうかを設定する
	// @param enable 設定する値
	void AllSetEnable(bool enable);

	// @brief 編集モードかどうかを返す
	// @return 編集モード...true
	bool IsEditorMode() const { return m_editorMode; }

private:

	// 管理するカメラ一覧
	std::list<std::shared_ptr<Camera>> m_cameraList;

	// 使用カメラ
	std::shared_ptr<Camera> m_spCamera;

	// 編集カメラ
	std::shared_ptr<EditorCamera> m_spEditorCamera;

	// 編集モード
	bool m_editorMode;

private:

	// @brief 優先度の確認
	// @note 確認後 最優先のカメラを有効にします
	void CheckPriority();

};
