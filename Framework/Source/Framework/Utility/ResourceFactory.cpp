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
const std::shared_ptr<SoundData> ResourceFactory::GetSoundData(const std::string& filepath, bool loop, bool useFilter)
{
	auto foundItr = m_soundMap.find(filepath);

	// リストにある
	if (foundItr != m_soundMap.end())
		return (*foundItr).second;

	// リストに無い
	std::shared_ptr<SoundData> newSound = std::make_shared<SoundData>();
	// 音を読み込む
	if (newSound->Load(filepath, loop, useFilter))
	{
		// リストに追加
		m_soundMap.insert(std::pair<
			std::string, std::shared_ptr<SoundData>>(filepath, newSound));

		return m_soundMap[filepath];
	}

	return nullptr;
}

//-----------------------------------------------------------------------------
// マップ確認の後Jsonデータを返す
//-----------------------------------------------------------------------------
const json ResourceFactory::GetJsonData(const std::string& filepath)
{
	auto foundItr = m_jsonMap.find(filepath);

	// リストにある
	if (foundItr != m_jsonMap.end())
		return (*foundItr).second;

	// リストに無い
	json newJson;
	if (newJson = LoadJson(filepath))
	{
		// リストに追加
		m_soundMap.insert(std::pair<std::string, json>(filepath, newJson));

		return m_jsonMap[filepath];
	}

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

//-----------------------------------------------------------------------------
// jsonファイル読み込み 解析
//-----------------------------------------------------------------------------
const json ResourceFactory::LoadJson(const std::string& filepath)
{
	// jsonファイルを開く
	std::fstream ifs(filepath);
	if (ifs.fail()) {
		assert(0 && "Error json filepath.");
		return nullptr;
	}

	// 文字列として読み込み
	std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	// 文字列のJSONを解析(パース)する
	json jsonObject = json::parse(strJson);
	if (jsonObject.is_null()) {
		assert(0 && "Error json parse.");
		return nullptr;
	}

	return jsonObject;
}
