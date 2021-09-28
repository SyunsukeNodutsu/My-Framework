#include "ResourceFactory.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ResourceFactory::ResourceFactory()
	: m_modelMap()
	, m_textureMap()
	, m_soundMap()
{
}

//-----------------------------------------------------------------------------
// マップ確認の後モデルデータを返す
//-----------------------------------------------------------------------------
const std::shared_ptr<ModelData> ResourceFactory::GetModelData(const std::string& filepath)
{
	auto foundItr = m_modelMap.find(filepath);

	// リストにある
	if (foundItr != m_modelMap.end())
		return (*foundItr).second;

	// リストに無い
	std::shared_ptr<ModelData> newModelData = std::make_shared<ModelData>();
	// モデルを読み込む
	if (newModelData->Load(filepath))
	{
		// リストに追加
		m_modelMap.insert(std::pair<
			std::string, std::shared_ptr<ModelData>>(filepath, newModelData));

		return m_modelMap[filepath];
	}

	return nullptr;
}

//-----------------------------------------------------------------------------
// マップ確認の後テクスチャを返す
//-----------------------------------------------------------------------------
const std::shared_ptr<Texture> ResourceFactory::GetTexture(const std::string& filepath)
{
	auto foundItr = m_textureMap.find(filepath);

	// リストにある
	if (foundItr != m_textureMap.end())
		return (*foundItr).second;

	// リストに無い
	std::shared_ptr<Texture> newTexture = std::make_shared<Texture>();
	// モデルを読み込む
	if (newTexture->Create(filepath))
	{
		// リストに追加
		m_textureMap.insert(std::pair<
			std::string, std::shared_ptr<Texture>>(filepath, newTexture));

		return m_textureMap[filepath];
	}

	return nullptr;
}

//-----------------------------------------------------------------------------
// マップ確認の後サウンドデータを返す
//-----------------------------------------------------------------------------
const std::shared_ptr<SoundData> ResourceFactory::GetSoundData(const std::string& filepath)
{

	return nullptr;
}

//-----------------------------------------------------------------------------
// 解放
//-----------------------------------------------------------------------------
void ResourceFactory::Release()
{
	m_modelMap.clear();
	m_textureMap.clear();
	m_soundMap.clear();
}
