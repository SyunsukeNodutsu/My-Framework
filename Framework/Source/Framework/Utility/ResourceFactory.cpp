#include "ResourceFactory.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ResourceFactory::ResourceFactory()
	: m_modelMap()
	, m_textureMap()
	, m_soundMap()
	, m_jsonMap()
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
const json11::Json ResourceFactory::GetJsonData(const std::string& filepath)
{
	auto foundItr = m_jsonMap.find(filepath);

	// リストにある
	if (foundItr != m_jsonMap.end())
		return (*foundItr).second;

	// Jsonファイル読み込み
	json11::Json json = LoadJson(filepath);
	if (!json.is_null())
	{
		// リストに登録
		m_jsonMap.insert(std::pair<std::string, json11::Json>(filepath, json));

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
	m_jsonMap.clear();
}

//-----------------------------------------------------------------------------
// jsonファイル読み込み 解析
//-----------------------------------------------------------------------------
const json11::Json ResourceFactory::LoadJson(const std::string& filepath)
{
	std::fstream ifs(filepath);
	if (ifs.fail()) {
		DebugLog(std::string("ERROR: Json file Not found. path: " + filepath).c_str());
		return nullptr;
	}

	std::string err, strjson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	// jsonの解析
	json11::Json result = json11::Json::parse(strjson, err);
	if (err.size() > 0) {
		DebugLog(std::string("ERROR: Json parse. Out error massage: " + err + "\n").c_str());
		return nullptr;
	}

	return result;
}
