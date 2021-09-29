#include "TankBullet.h"
#include "Tank.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
TankBullet::TankBullet(Tank owner)
	: m_owner(owner)
	, m_expSound3D(nullptr)
{
}

//-----------------------------------------------------------------------------
// 爆発
//-----------------------------------------------------------------------------
void TankBullet::Explosion()
{
	auto pos = m_transform.GetPosition();

	// Effect
	APP.g_effectDevice->Play(u"Resource/Effect/Explosion.efk", pos);

	// Sound
	auto exp = SOUND_DIRECTOR.CreateSoundWork3D("Resource/Audio/SE/Explosion02.wav", false, true);
	exp->Play3D(m_transform.GetPosition());
	exp->SetVolume(2.0f);
}
