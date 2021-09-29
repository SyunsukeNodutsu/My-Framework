#include "Camera.h"
#include "../../../Application/main.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Camera::Camera()
	: m_cameraMatrix(mfloat4x4::Identity)
	, m_viewMatrix(mfloat4x4::Identity)
	, m_projMatrix(mfloat4x4::Identity)
	, m_frustum()
	, g_priority()
	, g_enable(true)
	, g_name("empty")
	, m_dirtyCamera(true)
	, m_dirtyProj(true)
{

	m_projMatrix = DirectX::XMMatrixPerspectiveFovLH(60.0f * ToRadians, 16.0f / 9.0f, 0.01f, 2000.0f);
}

//-----------------------------------------------------------------------------
// カメラ情報(ビュー・射影行列など)をシェーダへセット
//-----------------------------------------------------------------------------
void Camera::SetToShader()
{
	// カメラが変更されていれば更新
	if (m_dirtyCamera)
	{
		RENDERER.Getcb9().Work().m_camera_matrix = m_cameraMatrix;
		RENDERER.Getcb9().Work().m_view_matrix = m_viewMatrix;
		RENDERER.Getcb9().Write();
	}

	// 射影行列が変更されていれば更新
	if (m_dirtyProj)
	{
		RENDERER.Getcb9().Work().m_proj_matrix = m_projMatrix;
		RENDERER.Getcb9().Write();
	}
}

//-----------------------------------------------------------------------------
// ワールド座標 スクリーン座標 変換
//-----------------------------------------------------------------------------
void Camera::ConvertWorldToScreen(const float3& pos, const mfloat4x4 matrix, float2& result)
{
	if (!g_graphicsDevice) return;
	if (!g_graphicsDevice->g_cpContext) return;

	// ビューポートを取得する
	UINT numV = 1;
	D3D11_VIEWPORT vp;
	g_graphicsDevice->g_cpContext->RSGetViewports(&numV, &vp);

	const float HalfViewportWidth = vp.Width * 0.5f;
	const float HalfViewportHeight = vp.Height * 0.5f;

	float3 Scale = DirectX::XMVectorSet(HalfViewportWidth, -HalfViewportHeight, vp.MaxDepth - vp.MinDepth, 0.0f);
	float3 Offset = DirectX::XMVectorSet(vp.TopLeftX + HalfViewportWidth, vp.TopLeftY + HalfViewportHeight, vp.MinDepth, 0.0f);

	mfloat4x4 Transform = DirectX::XMMatrixMultiply(matrix, GetViewMatrix());
	Transform = XMMatrixMultiply(Transform, GetProjMatrix());

	float3 Pos = { GetCameraMatrix()._41,GetCameraMatrix()._42 ,GetCameraMatrix()._43 };
	float3 Result = XMVector3TransformCoord(pos, Transform);

	Result = XMVectorMultiplyAdd(Result, Scale, Offset);
	result = float2(Result.x, Result.y);
}

//-----------------------------------------------------------------------------
// 試錐台をデバッグラインで描画
//-----------------------------------------------------------------------------
void Camera::DrawFrustum()
{
	const int CORNER_COUNT = m_frustum.CORNER_COUNT;

	// コーナーの所得
	float3 corners[CORNER_COUNT];
	m_frustum.GetCorners(corners);

	auto& gameSystem = APP.g_gameSystem;
	gameSystem->AddDebugLine(corners[0], corners[4]);
	gameSystem->AddDebugLine(corners[1], corners[5]);
	gameSystem->AddDebugLine(corners[2], corners[6]);
	gameSystem->AddDebugLine(corners[3], corners[7]);

	gameSystem->AddDebugLine(corners[4], corners[5]);
	gameSystem->AddDebugLine(corners[5], corners[6]);
	gameSystem->AddDebugLine(corners[6], corners[7]);
	gameSystem->AddDebugLine(corners[7], corners[4]);
}

//-----------------------------------------------------------------------------
// カメラ行列・ビュー行列セット
//-----------------------------------------------------------------------------
void Camera::SetCameraMatrix(const mfloat4x4& mCam)
{
	m_cameraMatrix = mCam;
	m_viewMatrix = m_cameraMatrix.Invert();
	m_dirtyCamera = true;

	// 試錐台作成
	qfloat4x4 quaternion = DirectX::XMQuaternionRotationMatrix(m_cameraMatrix);
	DirectX::BoundingFrustum::CreateFromMatrix(m_frustum, m_projMatrix);
	m_frustum.Origin = m_cameraMatrix.Translation();
	m_frustum.Orientation = quaternion;
}
