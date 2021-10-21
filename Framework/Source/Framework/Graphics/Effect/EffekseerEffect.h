//-----------------------------------------------------------------------------
// File: Effect.h
//
// エフェクシアのエフェクト管理
// データ ... EffectData
// 操作   ... EffectWork
//-----------------------------------------------------------------------------
#pragma once
#include "EffekseerDeviceChild.h"

// エフェクトデータ
class EffectData : public EffekseerDeviceChild
{
public:

	// @brief コンストラクタ
	EffectData();

	// @brief デストラクタ
	~EffectData() { ES_SAFE_RELEASE(m_pEffect); }

	// @brief 読み込み
	// @param filepath 読み込むエフェクトファイルのパス(UTF-16)
	// @return 成功...true
	bool Load(const std::u16string& filepath);

	// @brief エフェクトデータを返す
	// @return エフェクトデータ
	Effekseer::Effect* Get() { return m_pEffect; }

private:

	// Effekseerのエフェクトデータ
	Effekseer::Effect* m_pEffect;

private:

	// コピー禁止用:単一のデータはコピーできない
	EffectData(const EffectData& src) = delete;
	void operator=(const EffectData& src) = delete;

};

// エフェクト操作
class EffectWork : public EffekseerDeviceChild
{
public:

	// @brief コンストラクタ
	EffectWork();

	// @brief 初期化(データセット)
	// @param effectData 管理/操作するEffectデータ
	void Initialize(const std::shared_ptr<EffectData>& effectData);

	// @brief 再生
	// @param position 再生する座標
	void Play(const float3& position);

	// @brief 再生中のEffectを移動
	// @param addPosition 移動するベクトル
	void Move(float3& addPosition);

	// @brief 再生停止
	void Stop();

	// @brief 更新
	// @param deltaTime 前フレームからの経過時間
	void Update(float deltaTime);

	// @brief Effectデータを返す
	// @return Effectデータ
	std::shared_ptr<EffectData> GetData() { return m_effectData; }

	// @brief Effectハンドルを設定する
	// @param handle 設定するハンドル
	void SetHandle(Effekseer::Handle handle) { m_handle = handle; }

	// @brief Effectハンドルを返す
	// @return Effectハンドル
	inline Effekseer::Handle GetHandle() { return m_handle; }

private:

	// エフェクトのハンドル(index)
	Effekseer::Handle m_handle;

	// Effekseerのエフェクトデータ
	std::shared_ptr<EffectData> m_effectData;

private:

	// コピー禁止用:単一のデータはコピーできない
	EffectWork(const EffectWork& src) = delete;
	void operator=(const EffectWork& src) = delete;

};
