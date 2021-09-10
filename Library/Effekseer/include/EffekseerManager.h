#pragma once

class EffekseerEffectInstance;

// Effekseerを管理するクラス
class EffekseerManager
{
public:

	// 初期化
	void Initialize();

	// 解放
	void Finalize();

	// 更新
	void Update(float deltaTime);

	// 再生
	bool Play(const std::u16string& filePath, Math::Vector3& position);

	// エフェクト読み込み
	std::shared_ptr<EffekseerEffectInstance> LoadEffect(const std::u16string& filepath);

	// エフェクト検索
	std::shared_ptr<EffekseerEffectInstance> FindEffect(const std::u16string& filepath);

	// 全体の再生を止める
	void StopAll();

	// カメラ行列設定
	void SetCameraMatrix(Math::Matrix& matrix);

	static EffekseerManager& GetInstance()
	{
		static EffekseerManager instance;
		return instance;
	}

	// Effekseer本体のManager取得
	Effekseer::Manager* GetManager() { return m_pEffekseerManager; }

private:

	// Effeseer本体
	Effekseer::Manager*	m_pEffekseerManager;

	// Direct11用Renderer
	EffekseerRendererDX11::Renderer* m_pEffekseerRenderer;

	// 各エフェクトのインスタンス管理
	std::map< std::u16string, std::shared_ptr<EffekseerEffectInstance> > m_instanceMap;

	//エフェクシア用のカメラ行列.
	Math::Matrix m_mCameraMatrix;

	//最大描画数
	const int m_maxDrawSpriteCount = 8000;

	EffekseerManager() {}
};

//お手軽アクセス
#define EFFEKSEER EffekseerManager::GetInstance()

//エフェクトデータ
class EffekseerEffect
{
public:

	EffekseerEffect() {}
	~EffekseerEffect() { ES_SAFE_RELEASE(m_pEffect); }

	//読み込み
	bool Load(const std::u16string& filepath);

	//エフェクトデータ取得
	Effekseer::Effect* Get() { return m_pEffect; }

private:

	//Effekseerのエフェクトデータ
	Effekseer::Effect* m_pEffect;

	// コピー禁止用:単一のデータはコピーできない
	EffekseerEffect(const EffekseerEffect& src) = delete;
	void operator=(const EffekseerEffect& src) = delete;
};

//エフェクト再生用のインスタンス
class EffekseerEffectInstance : public std::enable_shared_from_this<EffekseerEffectInstance>
{
public:

	EffekseerEffectInstance() {}
	~EffekseerEffectInstance() {}

	//初期化
	void Initialize(const std::shared_ptr<EffekseerEffect>& effectData);

	//エフェクトを再生
	void Play(const Math::Vector3& position);

	//再生中のエフェクトを移動
	void Move(Math::Vector3& addPosition);

	//再生停止
	void Stop();

	//データ取得
	std::shared_ptr<EffekseerEffect> GetData() { return m_effectData; }

	void SetHandle(Effekseer::Handle handle) { m_handle = handle; }
	inline Effekseer::Handle GetHandle() { return m_handle; }

private:

	//エフェクトのハンドル(index)
	Effekseer::Handle m_handle;

	//Effekseerのエフェクトデータ
	std::shared_ptr<EffekseerEffect> m_effectData;

	// コピー禁止用:単一のデータはコピーできない
	EffekseerEffectInstance(const EffekseerEffectInstance& src) = delete;
	void operator=(const EffekseerEffectInstance& src) = delete;
};
