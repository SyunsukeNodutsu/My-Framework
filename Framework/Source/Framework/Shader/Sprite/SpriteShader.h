//-----------------------------------------------------------------------------
//File: SpriteShader.h
//
//画像描画用シェーダ
//描画関数は Begin～End間で実行しないと描画しません
//-----------------------------------------------------------------------------
#pragma once
#include "../Shader.h"

class SpriteShader : public Shader
{
	struct Vertex
	{
		float3 m_position;
		float2 m_uv;
	};

	struct cbSprite
	{
		float4 m_color = float4::One;
	};

public:

	//@brief コンストラクタ
	SpriteShader();

	//@brief 初期化
	bool Initialize();

	//@brief 描画開始
	//@param linear 線形補完？
	//@param disableZBuffer Z判定書き込み？
	void Begin(ID3D11DeviceContext* pd3dContext, bool linear = false, bool disableZBuffer = true);

	//@brief 描画終了 ※Begin()で記憶していたステートを復元
	void End(ID3D11DeviceContext* pd3dContext);

	//@brief 2D画像描画
	//@param texture テクスチャソース
	//@param position 2D座標
	//@param color 色
	void DrawTexture(const Texture* texture, float2 position, const cfloat4x4 color = cfloat4x4::White);
	//@param worldMatrix 設定するワールド行列
	void DrawTexture(const Texture* texture, ID3D11DeviceContext* pd3dContext, mfloat4x4 worldMatrix, const cfloat4x4 color = cfloat4x4::White);

private:

	ConstantBuffer<cbSprite> m_cb4Sprite;	//画像描画用 定数バッファ
	mfloat4x4 m_prevProjMat;				//保存用 射影行列
	bool m_isBegin;

};
