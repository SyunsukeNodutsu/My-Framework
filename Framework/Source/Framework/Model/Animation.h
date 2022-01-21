//-----------------------------------------------------------------------------
// File: Animation.h
//
// アニメーション管理
// データと操作を分割
//-----------------------------------------------------------------------------
#pragma once

// アニメーションキー(クォータニオン
struct AnimKeyQuaternion
{
	float m_time = 0;	//時間
	qfloat4x4 m_numRot;	//クォータニオンデータ
};

// アニメーションキー(ベクトル
struct AnimKeyVector3
{
	float m_time = 0;	//時間
	float3 m_numVec;	//3Dベクトルデータ
};

//==========================================================
// アニメーション(データ側)
//==========================================================
struct AnimationData
{
	// １ノードのアニメーションデータ
	struct Node
	{
		int m_nodeOffset = -1;//対象モデルノードのOffset値

		//各チャンネル
		std::vector<AnimKeyVector3> m_translations;	//位置キーリスト
		std::vector<AnimKeyQuaternion> m_rotations;	//回転キーリスト
		std::vector<AnimKeyVector3> m_scales;		//拡縮キーリスト

		//補完用関数群
		void Interpolate(mfloat4x4& rDst, float time);
		bool InterpolateTranslations(float3& result, float time);
		bool InterpolateRotations(qfloat4x4& result, float time);
		bool InterpolateScales(float3& result, float time);
	};

	std::vector<Node> m_nodes;	//全ノード用アニメーションデータ
	std::string m_name;			//アニメーション名
	float m_maxLength = 0;		//アニメーションの長さ

};



//==========================================================
// アニメーション(操作側)
//==========================================================
class Animator
{
public:

	//@brief アニメーションの更新
	//@param nodes 変化するノード
	//@param speed 速度(deltaTimeを考慮してください)
	void AdvanceTime(std::vector<ModelWork::Node>& nodes, float speed);

	//@brief アニメーションの設定
	//@param data 設定データ
	//@param isLoop ループ再生するかどうか
	inline void SetAnimation(const std::shared_ptr<AnimationData>& data, bool isLoop = true)
	{
		m_spAnimation = data;
		m_isLoop = isLoop;
		m_time = 0.0f;
	}

	//@brief アニメーションが終了しているかを返す
	//@return 終了...true
	bool IsAnimationEnd() const
	{
		if (m_spAnimation == nullptr) return true;
		if (m_time >= m_spAnimation->m_maxLength) return true;
		return false;
	}

private:

	std::shared_ptr<AnimationData>	m_spAnimation = nullptr;//再生するアニメーションデータ
	float m_time = 0.0f;
	bool m_isLoop = false;

};
