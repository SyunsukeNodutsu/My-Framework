//-----------------------------------------------------------------------------
// File: Camera.h
//
// カメラ
//-----------------------------------------------------------------------------
#pragma once
#include "../GraphicsDeviceChild.h"

// カメラクラス
class Camera : private GraphicsDeviceChild
{
public:

	// @brief コンストラクタ
	Camera();

	// @brief GPUに転送
	void SetToShader();

	// @brief ワールド座標をスクリーン座標に変換
	// @param pos 変換したい座標
	// @param matri 座標変換行列
	// @param result 結果格納用
	void ConvertWorldToScreen(const float3& pos, const mfloat4x4 matrix, float2& result);

	//--------------------------------------------------
	// 設定.取得
	//--------------------------------------------------

	// @brief カメラ行列とビュー行列を設定
	// @param proj 設定するカメラ行列
	virtual void SetCameraMatrix(const mfloat4x4& camera);

	// @brief 射影行列を設定
	// @param proj 設定する射影行列
	void SetProjectionMatrix(const mfloat4x4& proj) {
		m_projMatrix = proj; m_dirtyProj = true;
	}

	// @brief カメラ行列を返す
	// @return カメラ行列
	inline const mfloat4x4& GetCameraMatrix() const { return m_cameraMatrix; }

	// @brief ビュー行列を返す
	// @return ビュー行列
	inline const mfloat4x4& GetViewMatrix() const { return m_viewMatrix; }

	// @brief 射影行列を返す
	// @return 射影行列
	inline const mfloat4x4& GetProjMatrix() const { return m_projMatrix; }

	// @brief 試錐台を返す
	// @return 試錐台
	inline const DirectX::BoundingFrustum& GetFrustum() const { return m_frustum; }

protected:

	// 各行列
	mfloat4x4	m_cameraMatrix;	// カメラ行列
	mfloat4x4	m_viewMatrix;	// ビュー行列
	mfloat4x4	m_projMatrix;	// 射影行列

	// 視錐台
	DirectX::BoundingFrustum m_frustum;

public:

	// 優先度
	float g_priority;

	// 有効？
	bool g_enable;

	// カメラ名
	std::string g_name;

private:

	// ダーティーフラグ
	bool m_dirtyCamera;	// カメラ行列
	bool m_dirtyProj;	// 射影行列

};
