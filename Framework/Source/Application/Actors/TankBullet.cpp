#include "TankBullet.h"
#include "Tank.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
TankBullet::TankBullet(Tank owner, float3 position, float3 axis)
	: m_owner(owner)
	, m_axisZ(axis)
	, m_speed(60.0f)
	, m_lifeSpan(60.0f)
	, m_prevPos(position)
	, m_expSound3D(nullptr)
{
	LoadModel("Resource/Model/CannonBall/Cannonball.gltf");
	m_transform.SetPosition(position);
	m_axisZ.Normalize();

	m_name = "TankBullet";
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void TankBullet::Update(float deltaTime)
{
	if ((m_lifeSpan -= deltaTime) <= 0) m_isEnable = false;

	float3 position = m_prevPos = m_transform.GetPosition();

	// Z軸(射出方向)に移動
	position += m_axisZ * m_speed * deltaTime;

	// 重力による落下
	//positino.y += Physics::g_gravity * deltaTime; TODO: 落下させる場合、衝突判定の際の軸も変更
	m_transform.SetPosition(position);

	// 座標更新後 衝突判定
	UpdateCollision();
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
	auto sound = SOUND_DIRECTOR.CreateSoundWork3D("Resource/Audio/SE/Explosion02.wav", false, true);
	sound->Play3D(m_transform.GetPosition());
	sound->SetVolume(1.0f);

	m_isEnable = false;
}

//-----------------------------------------------------------------------------
// 衝突判定更新
//-----------------------------------------------------------------------------
void TankBullet::UpdateCollision()
{
	for (auto& actor : APP.g_gameSystem->GetActorList())
	{
		if (actor.get() == this) continue;

		float range = float3::Distance(m_prevPos, m_transform.GetPosition());
		bool hit = actor->CheckCollision(m_prevPos, m_axisZ, range);
		if (hit)
			Explosion();
	}
}
