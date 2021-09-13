//-----------------------------------------------------------------------------
// File: Animation.h
//
// データ   ... AnimationData
// ロジック ... Animator
//-----------------------------------------------------------------------------
#pragma once
#include "../Utility/Timer.h"

// アニメーションキー(クォータニオン
struct AnimKeyQuaternion
{
	float		m_time = 0;		// 時間
	qfloat4x4	m_numRot;		// クォータニオンデータ
};

// アニメーションキー(ベクトル
struct AnimKeyVector3
{
	float		m_time = 0;		// 時間
	float3		m_numVec;		// 数ベクトルデータ
};

// アニメーション データ
struct AnimationData
{
	// １ノードのアニメーションデータ
	struct Node
	{
		int			m_nodeOffset = -1;	// 対象モデルノードのOffset値

		// 各チャンネル
		std::vector<AnimKeyVector3>		m_translations;	// 位置キーリスト
		std::vector<AnimKeyQuaternion>	m_rotations;	// 回転キーリスト
		std::vector<AnimKeyVector3>		m_scales;		// 拡縮キーリスト

		void Interpolate(mfloat4x4& rDst, float time);
		bool InterpolateTranslations(float3& result, float time);
		bool InterpolateRotations(qfloat4x4& result, float time);
		bool InterpolateScales(float3& result, float time);
	};

	std::vector<Node>	m_nodes;		// 全ノード用アニメーションデータ
	std::string			m_name;			// アニメーション名
	float				m_maxLength = 0;// アニメの長さ

};

// アニメーション 操作.ロジック
class Animator
{
public:

	// @brief コンストラクタ
	Animator();

	// @brief デストラクタ
	~Animator() = default;

	// @brief アニメーションの設定
	// @param data アニメーションデータ
	// @param loop ループ再生？
	inline void SetAnimation(const std::shared_ptr<AnimationData>& data, bool loop = true)
	{
		m_spAnimation = data;
		m_isLoop = loop;
		m_time = 0.0f;// 再生offset
	}

	// @brief アニメーション再生が終了かどうかを返す
	// @return 終了...true
	bool IsAnimationEnd() const {
		if (m_spAnimation == nullptr || m_time >= m_spAnimation->m_maxLength)
			return true;
		return false;
	}

	// @brief アニメーションの更新
	// @param nodes モデルノード
	// @param speed 再生速度
	// @param brendWeight ブレンド重み
	void AdvanceTime(std::vector<ModelWork::Node>& nodes, float speed = 1.0f, float brendWeight = 1.0f);

private:

	std::shared_ptr<AnimationData>	m_spAnimation;		// アニメーションデータ
	float							m_time;
	bool							m_isLoop;

};

// ブレンドアニメーター
class BlendAnimator
{
public:

	// @brief コンストラクタ
	BlendAnimator();

	// @brief アニメーション更新
	// @param nodes モデルノード
	// @param speed 速度
	void AdvanceTime(std::vector<ModelWork::Node>& nodes, float speed = 1.0f);

	// @brief アニメーション登録
	// @param data 設定するアニメーションデータ
	// @param changeSec 切り替えのtick秒
	// @param loop ループ再生？
	void SetAnimation(std::shared_ptr<AnimationData>& data, float changeTick = 0.5f, bool loop = true);

	// @brief 終了しているかを返す
	// @return 終了...true
	bool IsAnimationEnd() const {
		if (m_pNowAnimator && !m_pNextAnimator->IsAnimationEnd())
			return false;
		return true;
	}

private:

	std::unique_ptr<Animator>	m_pNowAnimator;		// 現在再生中のアニメーション
	std::unique_ptr<Animator>	m_pNextAnimator;	// 次に遷移するアニメーション
	CommonTimer					m_timer;			// 時間計測
	float						m_changeTick;		// 遷移に要するTick秒
};
