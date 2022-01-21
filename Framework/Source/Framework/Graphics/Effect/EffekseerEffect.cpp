#include "EffekseerEffect.h"

//-----------------------------------------------------------------------------
//コンストラクタ
//-----------------------------------------------------------------------------
EffekseerEffect::EffekseerEffect()
	: m_effect()
	, m_handle()
{
}

//-----------------------------------------------------------------------------
//初期化
//-----------------------------------------------------------------------------
void EffekseerEffect::Initialize(const std::u16string& filepath)
{
	if (g_effectDevice == nullptr) return;
	m_effect = Effekseer::Effect::Create(g_effectDevice->g_manager, filepath.c_str());
}

//-----------------------------------------------------------------------------
//再生
//-----------------------------------------------------------------------------
void EffekseerEffect::Play(const float3& position, float speed)
{
	if (g_effectDevice == nullptr) return;
	m_handle = g_effectDevice->g_manager->Play(m_effect, position.x, position.y, position.z);
	g_effectDevice->g_manager->SetSpeed(m_handle, speed);
	g_effectDevice->g_manager->Flip();
}

//-----------------------------------------------------------------------------
//停止
//-----------------------------------------------------------------------------
void EffekseerEffect::Stop()
{
	if (g_effectDevice == nullptr) return;
	g_effectDevice->g_manager->StopEffect(m_handle);
}

//-----------------------------------------------------------------------------
//再生中のEffectを移動
//-----------------------------------------------------------------------------
void EffekseerEffect::AddLocation(const float3& addPosition)
{
	if (g_effectDevice == nullptr) return;
	g_effectDevice->g_manager->AddLocation(m_handle, ToE3D(addPosition));
}
