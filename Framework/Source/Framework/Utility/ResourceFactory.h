﻿//-----------------------------------------------------------------------------
// File: ResourceFactory.h
//
// リソース管理クラス
// フライウェイトパターン
//-----------------------------------------------------------------------------
#pragma once

// リソース管理クラス
class ResourceFactory
{
public:

	// @brief コンストラクタ
	ResourceFactory();

	//--------------------------------------------------
	// 取得・設定
	//--------------------------------------------------

	// @brief 単一のインスタンスを返す
	// @return シングルトン・インスタンス
	static ResourceFactory& GetInstance() {
		static ResourceFactory instance; return instance;
	}

	// @brief マップ確認の後モデルデータを返す
	// @param filepath ファイルのパス
	// @return モデルデータ
	const std::shared_ptr<ModelData> GetModelData(const std::string& filepath);

	// @brief マップ確認の後テクスチャデータを返す
	// @param filepath ファイルのパス
	// @return テクスチャデータ
	const std::shared_ptr<Texture> GetTexture(const std::string& filepath);

	// @brief マップ確認の後サウンドデータを返す
	// @param filepath ファイルのパス
	// @return サウンドデータ
	const std::shared_ptr<SoundData> GetSoundData(const std::string& filepath);

private:

	// モデルデータ管理マップ
	std::unordered_map<std::string, std::shared_ptr<ModelData>> m_modelMap;

	// テクスチャ管理マップ
	std::unordered_map<std::string, std::shared_ptr<Texture>> m_textureMap;

	// サウンド管理マップ
	std::unordered_map<std::string, std::shared_ptr<SoundData>> m_soundMap;

private:

	// @brief 解放
	void Release();

};

//--------------------------------------------------
// #defines: インスタンスの取得
//--------------------------------------------------
#define RES_FAC ResourceFactory::GetInstance()
