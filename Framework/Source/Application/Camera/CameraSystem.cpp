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
	if (APP.g_rawInputDevice->g_spKeyboard->IsPressed(KeyCode::I))
	{
		auto cameraH = SearchCamera("HumanTPS");
		if (cameraH) cameraH->g_priority = 1.0f;

		auto cameraE = SearchCamera("EditorFPS");
		if (cameraE) cameraE->g_priority = 0.0f;
	}
	if (APP.g_rawInputDevice->g_spKeyboard->IsPressed(KeyCode::O))
	{
		auto cameraH = SearchCamera("HumanTPS");
		if (cameraH) cameraH->g_priority = 0.0f;

		auto cameraE = SearchCamera("EditorFPS");
		if (cameraE) cameraE->g_priority = 1.0f;
	}

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
// 優先度の確認
//-----------------------------------------------------------------------------
void CameraSystem::CheckPriority()
{
	float priorityTmp = FLT_MIN;
	for (auto& camera : m_cameraList)
	{
		camera->g_use = false;

		// 優先度が高い方(以上)を設定
		if (camera->g_priority >= priorityTmp)
		{
			priorityTmp = camera->g_priority;
			m_spCamera = camera;
		}
	}

	m_spCamera->g_use = true;
}
