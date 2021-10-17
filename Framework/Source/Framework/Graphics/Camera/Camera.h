//-----------------------------------------------------------------------------
// File: Camera.h
//
// カメラ基底クラス
// 投影方法, 2D->3D変換, カリング有効設定など
//-----------------------------------------------------------------------------
#pragma once
#include "../GraphicsDeviceChild.h"

// カメラクラス
class Camera : private GraphicsDeviceChild
{
public:

	// 投影方法
	enum class ProjectionMethod
	{
		ePerspective,	// 透視射影行列 パース
		eOrthographic,	// 平行投影
	};

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
	// 設定
	//--------------------------------------------------

	// @brief 投影方法を設定
	// @param type 投影方法
	void SetProjectionMethod(ProjectionMethod type) {
		m_projectionMethod = type;
		m_dirtyCamera = m_dirtyProj = true;
	}

	// @brief カメラ行列とビュー行列を設定
	// @param proj 設定するカメラ行列
	virtual void SetCameraMatrix(const mfloat4x4& camera);

	// @brief カメラ画角を設定
	// @param radians 設定するカメラ画角(Radians)
	void SetFovAngleY(float radians) { m_fovAngleY = radians; m_dirtyProj = true; }

	// @brief アスペクト比を設定
	// @param aspectRatio 設定するカメラアスペクト比
	void SetAspect(float aspectRatio) { m_aspectRatio = aspectRatio; m_dirtyProj = true; }

	// @brief 近平面を設定
	// @param nearZ 設定するカメラ近平面
	void SetNearZ(float nearZ) { m_nearZ = nearZ; m_dirtyProj = true; }

	// @brief 遠平面を設定
	// @param farZ 設定するカメラ遠平面
	void SetFarZ(float farZ) { m_farZ = farZ; m_dirtyProj = true; }

	// @brief 投影方法が平行投影の場合、幅を設定
	// @param viewWidth 設定する幅
	void SetViewWidth(float viewWidth) { m_viewWidth = viewWidth; m_dirtyProj = true; }

	// @brief 投影方法が平行投影の場合、高さを設定
	// @param viewHeight 設定する高さ
	void SetViewHeight(float viewHeight) { m_viewHeight = viewHeight; m_dirtyProj = true; }

	//--------------------------------------------------
	// 取得
	//--------------------------------------------------

	// @brief カメラ行列を返す
	// @return カメラ行列
	inline const mfloat4x4& GetCameraMatrix() const { return m_cameraMatrix; }

	// @brief ビュー行列を返す
	// @return ビュー行列
	inline const mfloat4x4& GetViewMatrix() const { return m_viewMatrix; }

	// @brief 射影行列を返す
	// @return 射影行列
	inline const mfloat4x4& GetProjMatrix() const { return m_projMatrix; }

	// @brief ビュープロジェクション行列を返す
	// @return ビュープロジェクション行列
	inline const mfloat4x4& GetViewProjMatrix() const { return m_viewProjMatrix; }

	// @brief 試錐台を返す
	// @return 試錐台
	inline const DirectX::BoundingFrustum& GetFrustum() const { return m_frustum; }

	// @brief 画角を返す
	// @return カメラ画角(Radians)
	float GetFovAngleY() const { return m_fovAngleY; }

	// @brief アスペクト比を返す
	// @return カメラアスペクト比
	float GetAspect() const { return m_aspectRatio; }

	// @brief 近平面を返す
	// @return カメラ近平面
	float GetNearZ() const { return m_nearZ; }

	// @brief 遠平面を返す
	// @return カメラ遠平面
	float GetFarZ() const { return m_farZ; }

	// @brief 平行投影行列の幅を返す
	// @return 平行投影行列の幅
	float GetViewWidth() const { return m_viewWidth; }

	// @brief 平行投影行列の高さを返す
	// @return　平行投影行列の高さ
	float GetViewHeight() const { return m_viewHeight; }

protected:

	// 投影方法
	ProjectionMethod m_projectionMethod;

	// 各行列
	mfloat4x4	m_cameraMatrix;	// カメラ行列
	mfloat4x4	m_viewMatrix;	// ビュー行列
	mfloat4x4	m_projMatrix;	// 射影行列

	mfloat4x4	m_viewProjMatrix;// ビュープロジェクション行列

	// 視錐台
	DirectX::BoundingFrustum m_frustum;

	// カメラ座標
	float3 m_position;

	// 射影関連
	float m_fovAngleY;		// 画角(ラジアン)
	float m_aspectRatio;	// アスペクト比率
	float m_nearZ;			// 近平面
	float m_farZ;			// 遠平面

	// 平行投影行列の場合
	float m_viewWidth;	// 幅
	float m_viewHeight; // 高さ

	// ダーティーフラグ
	bool m_dirtyCamera;	// カメラ行列が変更
	bool m_dirtyProj;	// 射影行列が変更

public:

	// 優先度
	float g_priority;

	// 有効？
	bool g_enable;

	// 視錐台カリング？
	bool g_isFrustumCull;

	// カメラ名
	std::string g_name;

};
