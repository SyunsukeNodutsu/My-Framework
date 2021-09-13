//-----------------------------------------------------------------------------
// File: Animation.h
//
// アニメーション管理
//-----------------------------------------------------------------------------
#pragma once

// アニメーションキー(クォータニオン
struct AnimKeyQuaternion
{
	float		m_time = 0;		// 時間
	qfloat4x4	m_quat;			// クォータニオンデータ
};

// アニメーションキー(ベクトル
struct AnimKeyVector3
{
	float		m_time = 0;		// 時間
	float3		m_vec;			// 3Dベクトルデータ
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
	bool IsAnimationEnd() const
	{
		if (m_spAnimation == nullptr || m_time >= m_spAnimation->m_maxLength)
			return true;

		return false;
	}

	// @brief アニメーションの更新
	// @param nodes モデルノード
	// @param speed 再生速度
	void AdvanceTime(std::vector<ModelWork::Node>& nodes, float speed = 1.0f);

private:

	std::shared_ptr<AnimationData>	m_spAnimation;	// 再生するアニメーションデータ
	float							m_time;
	bool							m_isLoop;

};
