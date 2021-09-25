﻿//-----------------------------------------------------------------------------
// File: EffekseerDevice.h
//
// エフェクシアの管理
//-----------------------------------------------------------------------------
#pragma once
#include "../GraphicsDeviceChild.h"

class EffectWork;

// エフェクシア管理クラス
class EffekseerDevice : public GraphicsDeviceChild
{
public:

	// @brief コンストラクタ
	EffekseerDevice();

	// @brief 初期化
	// @return 成功...true
	bool Initialize();

	// @brief 終了
	void Finalize();

	// @brief 更新
	// @param deltaTime 前フレームからの経過時間
	void Update(float deltaTime);

	// 再生
	bool Play(const std::u16string& filepath, float3& position);

	// エフェクト読み込み
	std::shared_ptr<EffectWork> LoadEffect(const std::u16string& filepath);

	// エフェクト検索
	std::shared_ptr<EffectWork> FindEffect(const std::u16string& filepath);

	// 全体の再生を止める
	void StopAll();

	//--------------------------------------------------
	// static
	//--------------------------------------------------

	// 最大描画(スプライト)数
	static constexpr int MAX_EFFECT = 8000;

public:

	// Direct11用Renderer
	EffekseerRendererDX11::Renderer* g_renderer;

	// マネージャー
	Effekseer::Manager* g_manager;

private:

	// 各エフェクトのインスタンス管理
	std::map< std::u16string, std::shared_ptr<EffectWork> > m_instanceMap;

	// エフェクト再生速度
	float m_speed;

};
