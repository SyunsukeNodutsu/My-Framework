//-----------------------------------------------------------------------------
// File: Effect.h
//
// エフェクシアのデータ
// TODO: Effect.hじゃなくEffekseerEffect.hのほうがいいかも
//-----------------------------------------------------------------------------
#pragma once
#include "EffekseerDeviceChild.h"

// エフェクトデータ
class EffectData : public EffekseerDeviceChild
{
public:

	EffectData();

	~EffectData() { ES_SAFE_RELEASE(m_pEffect); }

	//読み込み
	bool Load(const std::u16string& filepath);

	//エフェクトデータ取得
	Effekseer::Effect* Get() { return m_pEffect; }

private:

	//Effekseerのエフェクトデータ
	Effekseer::Effect* m_pEffect;

	// コピー禁止用:単一のデータはコピーできない
	EffectData(const EffectData& src) = delete;
	void operator=(const EffectData& src) = delete;

};

// エフェクト操作
class EffectWork : public EffekseerDeviceChild
{
public:

	EffectWork();

	//初期化
	void Initialize(const std::shared_ptr<EffectData>& effectData);

	//エフェクトを再生
	void Play(const float3& position);

	// 再生中のエフェクトを移動
	void Move(float3& addPosition);

	// 再生停止
	void Stop();

	// 更新
	void Update(float deltaTime);

	// データ取得
	std::shared_ptr<EffectData> GetData() { return m_effectData; }

	void SetHandle(Effekseer::Handle handle) { m_handle = handle; }
	inline Effekseer::Handle GetHandle() { return m_handle; }

private:

	//エフェクトのハンドル(index)
	Effekseer::Handle m_handle;

	//Effekseerのエフェクトデータ
	std::shared_ptr<EffectData> m_effectData;

	// コピー禁止用:単一のデータはコピーできない
	EffectWork(const EffectWork& src) = delete;
	void operator=(const EffectWork& src) = delete;
};
