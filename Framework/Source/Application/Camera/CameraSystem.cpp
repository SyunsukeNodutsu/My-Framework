#include "CameraSystem.h"
#include "EditorCamera.h"
#include "../main.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
CameraSystem::CameraSystem()
	: m_cameraList()
	, m_spCamera(nullptr)
	, m_spEditorCamera(nullptr)
	, m_editorMode(false)
{
	m_spEditorCamera = std::make_shared<EditorCamera>();
	m_spEditorCamera->g_priority = FLT_MIN;
	m_spEditorCamera->g_enable = false;
	m_spEditorCamera->g_name = "EditorCamera";
	m_cameraList.push_back(m_spEditorCamera);

	m_spEditorCamera->SetCameraMatrix(mfloat4x4::CreateTranslation(float3(0, 4, -4)));
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void CameraSystem::Update(float deltaTime)
{
	if (m_editorMode)
	{
		m_spEditorCamera->Update(deltaTime);

		// 通常モードに移行
		if (ApplicationChilled::GetApplication()->g_inputDevice->IsKeyPressed(KeyCode::Escape))
		{
			m_spEditorCamera->g_priority = FLT_MIN;

			m_editorMode = false;
			AllSetEnable(true);
			ApplicationChilled::GetApplication()->g_inputDevice->SetCursorShow(false);
		}
	}
	else
	{
		// 編集モードに移行
		if (ApplicationChilled::GetApplication()->g_inputDevice->IsKeyPressed(KeyCode::Escape))
		{
			m_spEditorCamera->g_priority = FLT_MAX;

			m_editorMode = true;
			AllSetEnable(false);
			ApplicationChilled::GetApplication()->g_inputDevice->SetCursorShow(true);
		}
	}

	CheckPriority();
}

//-----------------------------------------------------------------------------
// カメラ情報をGPUに転送
//-----------------------------------------------------------------------------
void CameraSystem::SetToDevice()
{
	if (m_spCamera == nullptr) return;
	m_spCamera->SetToShader();
}

//-----------------------------------------------------------------------------
// 編集カメラ以外のカメラ一覧をクリア
//-----------------------------------------------------------------------------
void CameraSystem::ResetCameraList()
{
	m_cameraList.clear();
	m_cameraList.push_back(m_spEditorCamera);// 編集カメラは常に存在
}

//-----------------------------------------------------------------------------
// カメラ追加
//-----------------------------------------------------------------------------
void CameraSystem::AddCameraList(std::shared_ptr<Camera> camera)
{
	if (!camera) return;
	m_cameraList.push_back(camera);
	CheckPriority();
}

//-----------------------------------------------------------------------------
// 現在使用中の(最も優先度が高い)カメラを返す
//-----------------------------------------------------------------------------
const std::shared_ptr<Camera>& CameraSystem::GetCamera() const
{
	return m_spCamera;
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
	ApplicationChilled::GetApplication()->g_imGuiSystem->AddLog("ERROR: Camera not found. -CameraSystem.SearchCamera()");
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
