#include "EffekseerEffect.h"

//=============================================================================
//
// EffectData
//
//=============================================================================

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
EffectData::EffectData()
	: m_pEffect(nullptr)
{
}

//-----------------------------------------------------------------------------
// 読み込み
//-----------------------------------------------------------------------------
bool EffectData::Load(const std::u16string& filepath)
{
	if (!g_effectDevice) return false;
	if (!g_effectDevice->g_manager) return false;
	
	m_pEffect = Effekseer::Effect::Create(g_effectDevice->g_manager, filepath.c_str());
	if (m_pEffect == nullptr) {
		assert(0 && "Effekseerファイルのパスが間違っています");
		return false;
	}
	return true;
}



//=============================================================================
//
// EffectWork
//
//=============================================================================

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
EffectWork::EffectWork()
	: m_handle()
	, m_effectData(nullptr)
{
}

//-----------------------------------------------------------------------------
// 初期化(データセット)
//-----------------------------------------------------------------------------
void EffectWork::Initialize(const std::shared_ptr<EffectData>& effectData)
{
	if (!effectData) return;

	m_effectData = effectData;
}

//-----------------------------------------------------------------------------
// 再生
//-----------------------------------------------------------------------------
void EffectWork::Play(const float3& position)
{
	if (!g_effectDevice) return;
	if (!g_effectDevice->g_manager) return;

	if (!m_effectData) return;

	m_handle = g_effectDevice->g_manager->Play(m_effectData->Get(), position.x, position.y, position.z);
}

//-----------------------------------------------------------------------------
// 再生中のEffectを移動
//-----------------------------------------------------------------------------
void EffectWork::Move(float3& addPosition)
{
	if (!g_effectDevice) return;
	if (!g_effectDevice->g_manager) return;

	g_effectDevice->g_manager->AddLocation(m_handle, ToE3D(addPosition));
}

//-----------------------------------------------------------------------------
// 再生停止
//-----------------------------------------------------------------------------
void EffectWork::Stop()
{
	if (!g_effectDevice) return;
	if (!g_effectDevice->g_manager) return;

	g_effectDevice->g_manager->StopEffect(m_handle);
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void EffectWork::Update(float deltaTime)
{
	if (!g_effectDevice) return;
	if (!g_effectDevice->g_manager) return;

	g_effectDevice->g_manager->SetSpeed(m_handle, deltaTime);
	g_effectDevice->g_manager->Flip();
}
