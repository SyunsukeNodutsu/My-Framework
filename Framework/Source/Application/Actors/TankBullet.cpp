#include "TankBullet.h"
#include "Tank.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
TankBullet::TankBullet(Tank owner, float3 position, float3 axis)
	: m_owner(owner)
	, m_axisZ(axis)
	, m_speed(160.0f)
	, m_lifeSpan(20.0f)
	, m_prevPos(position)
	, m_expSound3D(nullptr)
{
	LoadModel("Resource/Model/CannonBall/Cannonball.gltf");
	m_transform.SetPosition(position);
	m_axisZ.Normalize();

	g_name = "TankBullet";

	g_tag = ACTOR_TAG::eWeapon | ACTOR_TAG::eBullet;
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void TankBullet::Update(float deltaTime)
{
	if ((m_lifeSpan -= deltaTime) <= 0) g_enable = false;

	float3 position = m_prevPos = m_transform.GetPosition();

	// Z軸(射出方向)に移動
	position += m_axisZ * m_speed * deltaTime;

	// 重力による落下
	//position.y += Physics::g_gravity * deltaTime;
	position.y -= 0.8f * deltaTime;
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

	// Sound and Effect
	APP.g_effectDevice->Play(u"Resource/Effect/Explosion.efk", pos);
	SOUND_DIRECTOR.Play3D("Resource/Audio/SE/Explosion02.wav", m_transform.GetPosition(), 0, 1.0f);

	g_enable = false;
}

//-----------------------------------------------------------------------------
// 衝突判定更新
//-----------------------------------------------------------------------------
void TankBullet::UpdateCollision()
{
	// TODO: 空間分割で最適化
	for (auto& actor : APP.g_gameSystem->GetActorList())
	{
		// 自身と弾同士は判定を飛ばす
		if (actor.get() == this) continue;
		//if (actor->GetTAG() & ACTOR_TAG::eBullet) continue;

		float range = float3::Distance(m_prevPos, m_transform.GetPosition());
		float3 dir = m_prevPos - m_transform.GetPosition();
		dir.Normalize();

		if (actor->CheckCollision(m_prevPos, dir, range))
			Explosion();
	}
}
