#include "EffekseerEffect.h"

static Effekseer::Vector3D ToE3D(float3 vector)
{
	return Effekseer::Vector3D(vector.x, vector.y, vector.z);
}

//=============================================================================
//
// EffectData
//
//=============================================================================

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
EffectData::EffectData()
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
{
}

void EffectWork::Initialize(const std::shared_ptr<EffectData>& effectData)
{
	m_effectData = effectData;
}

void EffectWork::Play(const float3& position)
{
	m_handle = g_effectDevice->g_manager->Play(m_effectData->Get(), position.x, position.y, position.z);
}

void EffectWork::Move(float3& addPosition)
{
	g_effectDevice->g_manager->AddLocation(m_handle, ToE3D(addPosition));
}

void EffectWork::Stop()
{
	g_effectDevice->g_manager->StopEffect(m_handle);
}

void EffectWork::Update(float deltaTime)
{
	g_effectDevice->g_manager->SetSpeed(m_handle, deltaTime);
	g_effectDevice->g_manager->Flip();
}
