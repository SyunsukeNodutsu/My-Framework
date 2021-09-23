﻿//-----------------------------------------------------------------------------
// File: SpriteShader.h
//
// 画像描画用シェーダ
// 描画関数は Begin～End間で実行しないと描画しません
//-----------------------------------------------------------------------------
#pragma once
#include "../Shader.h"

class SpriteShader : public Shader
{
public:

	// @brief コンストラクタ
	SpriteShader();

	// @brief 初期化
	bool Initialize();

	// @brief 描画開始
	// @param linear 線形補完？
	// @param disableZBuffer Z判定書き込み？
	void Begin(bool linear = false, bool disableZBuffer = true);

	// @brief 描画終了 ※Begin()で記憶していたステートを復元
	void End();

	// @brief 2D画像描画
	// @param texture テクスチャソース
	// @param position 2D座標
	// @param color 色
	void DrawTexture(const Texture* texture, float2 position, const cfloat4x4 color = cfloat4x4::White);

private:

	// 1頂点
	struct Vertex {
		float3 m_pos;
		float2 m_uv;
	};

	// 定数バッファ
	struct cbSprite {
		float4 m_color = { 1, 1, 1, 1 };
	};

private:

	ConstantBuffer<cbSprite>	m_cb4Sprite;	// 画像描画用 定数バッファ
	mfloat4x4					m_prevProjMat;	// 保存用 射影行列
	bool						m_isBegin;		// Flag

	Buffer m_tempFixedVertexBuffer[10];			// 固定長 頂点バッファ
	Buffer m_tempVertexBuffer;					// 可変長 頂点バッファ

private:

	// @brief 最終的な描画 TODO: のちに移動
	// @param topology
	// @param vertexCount
	// @param pVertexStream
	// @param stride
	void DrawVertices(D3D_PRIMITIVE_TOPOLOGY topology, int vertexCount, const void* pVertexStream, UINT stride);

};
