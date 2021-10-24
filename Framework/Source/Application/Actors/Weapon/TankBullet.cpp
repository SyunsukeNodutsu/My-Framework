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
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void TankBullet::Update(float deltaTime)
{
	if ((m_lifeSpan -= deltaTime) <= 0) g_enable = false;

	float3 position = m_prevPos = m_transform.GetPosition();

	// 重力の影響
	constexpr float gravity = 0.16f;
	m_axisZ.y -= gravity * deltaTime;
	// Z軸(射出方向)に移動
	position += m_axisZ * m_speed * deltaTime;
	
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

		// floatの桁落ち(すり抜け)防止
		// rangeに0.04fを加算, 発射座標を(方向 * 0.2)だけ減算
		
		float range = float3::Distance(m_prevPos, m_transform.GetPosition()) + 0.04f;
		float3 dir = m_prevPos - m_transform.GetPosition();
		dir.Normalize();

		RayResult result = {};
		if (actor->CheckCollision(m_prevPos - (dir * 0.2f), dir, range, result))
			Explosion();
	}
}
