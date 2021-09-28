#include "CameraSystem.h"
#include "../main.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
CameraSystem::CameraSystem()
	: m_cameraList()
{
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void CameraSystem::Update(float deltaTime)
{
	CheckPriority();
}

//-----------------------------------------------------------------------------
// カメラ情報をGPUに転送
//-----------------------------------------------------------------------------
void CameraSystem::SetToDevice()
{
	if (!m_spCamera) return;
	m_spCamera->SetToShader();
}

//-----------------------------------------------------------------------------
// カメラ追加
//-----------------------------------------------------------------------------
void CameraSystem::SetCameraList(std::shared_ptr<Camera> camera)
{
	if (!camera) return;
	m_cameraList.push_back(camera);
	CheckPriority();
}

//-----------------------------------------------------------------------------
// カメラ検索
//-----------------------------------------------------------------------------
std::shared_ptr<Camera> CameraSystem::SearchCamera(std::string name) const
{
	for (auto& camera : m_cameraList)
	{
		if (camera->g_name == name)
			return camera;
	}
	APP.g_imGuiSystem->AddLog("Camera not found.");
	return nullptr;
}

//-----------------------------------------------------------------------------
// 全てのカメラに有効かどうかを設定する
//-----------------------------------------------------------------------------
void CameraSystem::AllSetEnable(bool enable)
{
	for (auto& camera : m_cameraList)
		camera->g_enable = enable;
}

//-----------------------------------------------------------------------------
// 優先度の確認
//-----------------------------------------------------------------------------
void CameraSystem::CheckPriority()
{
	float priorityTmp = FLT_MIN;
	for (auto& camera : m_cameraList)
	{
		camera->g_enable = false;

		// 優先度が高い方(以上)を設定
		if (camera->g_priority >= priorityTmp)
		{
			priorityTmp = camera->g_priority;
			m_spCamera = camera;
		}
	}

	// 最も優先度が高いカメラの有効FlgをON
	m_spCamera->g_enable = true;
}
