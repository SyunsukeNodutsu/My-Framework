#include "Camera.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Camera::Camera()
	: m_cameraMatrix(mfloat4x4::Identity)
	, m_viewMatrix(mfloat4x4::Identity)
	, m_projMatrix(mfloat4x4::Identity)
	, m_dirtyCamera(false)
	, m_dirtyProj(false)
{
}

//-----------------------------------------------------------------------------
// カメラ情報(ビュー・射影行列など)をシェーダへセット
//-----------------------------------------------------------------------------
void Camera::SetToShader()
{
	// カメラが変更されていれば更新
	if (m_dirtyCamera)
	{
		m_viewMatrix = m_cameraMatrix.Invert();
		RENDERER.SetViewMatrix(m_viewMatrix);
	}

	// 射影行列が変更されていれば更新
	if (m_dirtyProj)
		RENDERER.SetProjMatrix(m_projMatrix);
}

//-----------------------------------------------------------------------------
// ワールド座標 スクリーン座標 変換
//-----------------------------------------------------------------------------
void Camera::ConvertWorldToScreen(const float3& pos, const mfloat4x4 matrix, float& resultX, float& resultY)
{
	// ビューポートを取得する
	DirectX::SimpleMath::Viewport vp;
	//D3D.GetViewport(vp);
	const float HalfViewportWidth = vp.width * 0.5f;
	const float HalfViewportHeight = vp.height * 0.5f;

	float3 Scale = DirectX::XMVectorSet(HalfViewportWidth, -HalfViewportHeight, vp.maxDepth - vp.minDepth, 0.0f);
	float3 Offset = DirectX::XMVectorSet(vp.x + HalfViewportWidth, vp.y + HalfViewportHeight, vp.minDepth, 0.0f);

	mfloat4x4 Transform = DirectX::XMMatrixMultiply(matrix, GetViewMatrix());
	Transform = XMMatrixMultiply(Transform, GetProjMatrix());

	float3 Pos = { GetCameraMatrix()._41,GetCameraMatrix()._42 ,GetCameraMatrix()._43 };
	float3 Result = XMVector3TransformCoord(pos, Transform);

	Result = XMVectorMultiplyAdd(Result, Scale, Offset);

	resultX = Result.x;
	resultY = Result.y;

	/*
		const float HalfViewportWidth = ViewportWidth * 0.5f;
		const float HalfViewportHeight = ViewportHeight * 0.5f;

		XMVECTOR Scale = XMVectorSet(HalfViewportWidth, -HalfViewportHeight, ViewportMaxZ - ViewportMinZ, 0.0f);
		XMVECTOR Offset = XMVectorSet(ViewportX + HalfViewportWidth, ViewportY + HalfViewportHeight, ViewportMinZ, 0.0f);

		XMMATRIX Transform = XMMatrixMultiply(World, View);
		Transform = XMMatrixMultiply(Transform, Projection);

		XMVECTOR Result = XMVector3TransformCoord(V, Transform);

		Result = XMVectorMultiplyAdd(Result, Scale, Offset);
	*/
}

//-----------------------------------------------------------------------------
// カメラ行列・ビュー行列セット
//-----------------------------------------------------------------------------
void Camera::SetCameraMatrix(const mfloat4x4& mCam)
{
	m_cameraMatrix = mCam;
	m_dirtyCamera = true;
}
