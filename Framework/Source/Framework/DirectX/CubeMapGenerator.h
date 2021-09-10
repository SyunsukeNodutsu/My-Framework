//-----------------------------------------------------------------------------
// File: CubeMapGenerator.h
//
// キューブマップ作成
// dds形式に保存/読み込み可能
//-----------------------------------------------------------------------------
#pragma once

// キューブマップ作成クラス
class CubeMapGenerator
{
public:

	// @brief コンストラクタ
	CubeMapGenerator();

	// @brief デストラクタ
	~CubeMapGenerator() = default;

	// 指定サイズのキューブマップを生成し、風景を描画する。
	// size		：キューブマップテクスチャのサイズ。
	// position	：撮影するワールド座標。
	// drawProc	：実行する描画関数。
	void Generate(int size, const float3& position, std::function<void()> drawProc);

	// ファイルから読み込む
	void LoadFromFile(const std::string& filename);

	// ファイルへ保存する
	void SaveToFile(const std::string& filename);

	// キューブマップを返す
	std::shared_ptr<Texture> GetCubeMap() const { return m_cubeMap; }

private:

	std::shared_ptr<Texture> m_cubeMap;	// キューブマップ
	std::shared_ptr<Texture> m_zBuffer;	// 描画時に使用するZバッファ

};
