//-----------------------------------------------------------------------------
// File: CubeMapGenerator.h
//
// キューブマップの生成
//-----------------------------------------------------------------------------
#pragma once
#include "GraphicsDeviceChild.h"

// キューブマップ生成クラス
class CubeMapGenerator : public GraphicsDeviceChild
{
public:

	// @brief コンストラクタ
	CubeMapGenerator();

	// @brief 指定サイズのキューブマップを生成し 風景を描画する
	// @param size キューブマップテクスチャのサイズ。
	// @param position 撮影するワールド座標。
	// @param drawProc 実行する描画関数。
	void Generate(int size, const float3& position, std::function<void()> drawProc);
	
	// @brief ファイルから読み込む
	// @param filepath 読み込むファイルパス
	void LoadFromFile(const std::string& filepath);

	// @brief ファイルへ保存する
	// @param filepath 保存するファイルパス
	void SaveToFile(const std::string& filename) {}

	//--------------------------------------------------
	// 設定/取得
	//--------------------------------------------------

	// @brief 生成されたキューブマップを返す
	// @return 生成されたキューブマップテクスチャ
	std::shared_ptr<Texture> GetCubeMap() const { return m_cubeMap; }

private:

	// キューブマップ
	std::shared_ptr<Texture> m_cubeMap;

	// 描画時に使用するZバッファ
	std::shared_ptr<Texture> m_zBuffer;

};
