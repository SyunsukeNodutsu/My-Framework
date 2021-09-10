//-----------------------------------------------------------------------------
// File: Renderer.h
//
// 描画にかかわる各種設定の管理
// TODO: スタッツは分けた方がいいかも
//-----------------------------------------------------------------------------
#pragma once
#include "GraphicsDeviceChild.h"

enum SS_FilterMode; enum SS_AddressMode;
enum RS_CullMode;	enum RS_FillMode;
enum BlendMode;

// 描画各種管理クラス
class Renderer : public GraphicsDeviceChild
{
	struct SaveState;
public:

	// @brief コンストラクタ
	Renderer();

	// @brief デストラクタ
	~Renderer() = default;

	// @brief 初期化
	// @return　成功...終了 失敗...false
	bool Initialize();

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------

	// @brief デフォルト設定
	void SetDefaultState();

	// @brief ワールド変換行列の設定 定数バッファにセット
	// @param matrix 設定する行列
	void SetWorldMatrix(const mfloat4x4& matrix);

	// @brief ビュー変換行列の設定 定数バッファにセット
	// @param matrix 設定する行列
	void SetViewMatrix(const mfloat4x4& matrix);

	// @brief 射影変換行列の設定 定数バッファにセット
	// @param matrix 設定する行列
	void SetProjMatrix(const mfloat4x4& matrix);

	// @brief テクスチャ設定
	// @param texture テクスチャ
	// @param slot 使用スロット
	void SetTexture(Texture* texture, int slot = 0);

	// @brief デフォルトテクスチャ設定
	// ※テクスチャが設定されていなかった際などに使用
	void SetDefaultTexture() { SetTexture(m_spWhiteTexture.get(), 0); }

	// @brief Zバッファ使用設定
	// @param zUse
	// @param zWrite
	void SetDepthStencil(bool zUse, bool zWrite);

	// @brief サンプラーステート設定
	// @param filter
	// @param address
	// @return 成功...true 失敗...false
	bool SetSampler(SS_FilterMode filter, SS_AddressMode address);

	// @brief ブレンドモード設定
	// @param flag
	// @return 成功...true 失敗...false
	bool SetBlend(BlendMode flag);

	// @brief ラスタライザーステート設定
	// @param cull
	// @param fill
	// @return 成功...true 失敗...false
	bool SetRasterize(RS_CullMode cull, RS_FillMode fill);

	// @brief ライトのON.OFFを設定
	// @param onoff ONかOFF
	inline void SetLightEnable(bool onoff) { m_cd10Light.Work().m_enable = onoff; }

	// @brief ライトの方向を設定
	// @param v 方向 関数内で正規化します
	inline void SetDirectionalLightDir(const float3& v)
	{
		m_cd10Light.Work().m_directional_light_dir = v;
		m_cd10Light.Work().m_directional_light_dir.Normalize();
	}

	// @brief ライトの色を設定
	// @param color 色
	inline void SetDirectionalLightColor(const float3& color) {
		m_cd10Light.Work().m_directional_light_color = color;
	}

	// @brief ライトの強さを設定
	// @param pow ライトの強さ
	inline void SetAmbientPower(const float& pow) { m_cd10Light.Work().m_ambient_power = pow; }

	// @brief ライトの設定を書き込み
	inline void LightCommit() { m_cd10Light.Write(); }

	// @brief 記憶/復元用のステートを返す
	// @return 記憶/復元用のステート構造体
	const SaveState& GetSaveState() const { return m_saveState; }

	// @brief 時間系を設定 TODO: FpsTimerを使用するのでprivateで書いた方がいい
	// @param totalTime 経過時間
	// @param deltaTime デルタティック
	void SetTime(const float& totalTime, const float& deltaTime)
	{
		m_cd12Time.Work().m_totalTime = totalTime;
		m_cd12Time.Work().m_deltaTime = deltaTime;
		m_cd12Time.Write();
	}

	//
	void SetDitherEnable(bool onoff) {
		m_cd8WorldMatrix.Work().m_dither_enable = onoff;
		m_cd8WorldMatrix.Write();
	}
	//
	void SetDistToEye(float dist) {
		m_cd8WorldMatrix.Work().m_dist_to_eye = dist;
		m_cd8WorldMatrix.Write();
	}

	// カメラ座標を返す
	float3 GetCameraPos() const {
		mfloat4x4 cam = m_cd9ViewProjMatrix.Get().m_camera_matrix;
		return cam.Translation();
	}

	//--------------------------------------------------
	// static
	//--------------------------------------------------

	// ワールド変換行列が使用するバッファのスロット番号
	static const int use_slot_world_matrix		=  8;
	// ビュー 射影 変換行列が使用するバッファのスロット番号
	static const int use_slot_view_proj_matrix	=  9;
	// ライトが使用するバッファのスロット番号
	static const int use_slot_light				= 10;
	// マテリアルが使用するバッファのスロット番号
	static const int use_slot_material			= 11;
	// 時間が使用するバッファのスロット番号
	static const int use_slot_time				= 12;
	// 大気が使用するバッファのスロット番号
	static const int use_slot_atmosphere		= 13;

private:

	// ワールド変換行列
	struct cdWorldMatrix
	{
		mfloat4x4 m_world_matrix;
		float m_dist_to_eye;  // カメラとの距離
		float m_dither_enable;// ディザリング有効？
		float tmp[2];
	};

	// ビュー 射影 変換行列
	struct cdViwProjMatrix
	{
		mfloat4x4 m_view_matrix;
		mfloat4x4 m_proj_matrix;
		mfloat4x4 m_camera_matrix;
	};

	// ライト
	struct cdLight
	{
		float m_enable = 1;									// 有効？
		float3 m_directional_light_dir = float3::Down;		// 方向
		float3 m_directional_light_color = float3(1, 1, 1);	// 色
		float m_ambient_power = 0.4f;						// 環境光の強さ
	};

	// 時間系
	struct cdTime
	{
		float m_totalTime = 0.0f; // 総経過時間
		float m_deltaTime = 0.0f; // デルタティック
		float tmp[2];
	};

	// 大気
	struct cdAtmosphere
	{
		float3 m_sun_position;			// 太陽の座標

		// 距離Fog
		float m_distance_fog_enable;	// 距離フォグ有効？
		float3 m_distance_fog_color;	// 距離フォグ色
		float m_distance_fog_rate;		// 距離フォグ減衰率

		// 高さFog
		float m_height_fog_enable;		// 高さフォグ有効？
		float3 m_height_fog_color;		// 高さフォグ色

		// Mie散乱
		float m_mie_streuung_enable;	// Mie散乱有効？
		float m_mie_streuung_factor_coefficient;// Mie散乱因子係数(-0.75 ～ -0.999)
		float tmp[2];
	};

	// ステート記憶/復元用
	struct SaveState
	{
		ComPtr<ID3D11SamplerState>		SS = nullptr;
		ComPtr<ID3D11RasterizerState>	RS = nullptr;
		ComPtr<ID3D11DepthStencilState> DS = nullptr;
		ComPtr<ID3D11BlendState>		BS = nullptr;
		UINT StencilRef = 0;
		UINT SampleMask = 0;
		float BlendFactor[4] = { 0,0,0,0 };
		mfloat4x4 mProj;
	};

	std::map<int, ComPtr<ID3D11SamplerState>>		m_samplerStates;		// サンプラーステートMap
	std::map<int, ComPtr<ID3D11RasterizerState>>	m_rasterizerState;		// ラスタライザーステートMap
	ComPtr<ID3D11DepthStencilState>					m_depthStencilStates[4];// デプスステンシルステート
	ComPtr<ID3D11BlendState>						m_blendStates[3];		// ブレンドステート
	ConstantBuffer<cdWorldMatrix>					m_cd8WorldMatrix;		// ワールド変換行列
	ConstantBuffer<cdViwProjMatrix>					m_cd9ViewProjMatrix;	// ビュー 射影 変換行列
	ConstantBuffer<cdLight>							m_cd10Light;			// ライト
	// 11はマテリアル
	ConstantBuffer<cdTime>							m_cd12Time;				// ゲーム内 時間関連
	ConstantBuffer<cdAtmosphere>					m_cd13Atmosphere;		// 大気
	std::shared_ptr<Texture>						m_spWhiteTexture;		// 1x1のデフォルト設定用テクスチャ
	SaveState										m_saveState;			// ステート 保存/復元用

private:

	// @brief サンプラーステート作成
	// @param filter フィルタリング設定
	// @param address アドレッシング設定
	// @return 成功...true 失敗...false
	bool CreateSampler(SS_FilterMode filter, SS_AddressMode address);

	// @brief ラスタライザーステート作成
	// @param cull カリング設定
	// @param fill 塗りつぶし設定
	// @return 成功...true 失敗...false
	bool CreateRasterrize(RS_CullMode cull, RS_FillMode fill);

	// @brief デプスステンシルステート作成
	// @param zUse 深度テストを行う？
	// @param zWrite 深度書き込みを行う？
	// @return 作成されたデプスステンシルステート
	ComPtr<ID3D11DepthStencilState> CreateDepthStencil(bool zUse, bool zWrite);

	// @brief ブレンドステート作成
	// @param flag ブレンド方法
	// @return 作成されたブレンドステート
	ComPtr<ID3D11BlendState> CreateBlend(BlendMode flag);

public:

	// TODO: こっちのがええやろ
	const ConstantBuffer<cdWorldMatrix>& Getcd8() const { return m_cd8WorldMatrix; }
	const ConstantBuffer<cdViwProjMatrix>& Getcd9() const { return m_cd9ViewProjMatrix; }
	const ConstantBuffer<cdLight>& Getcd10()const { return m_cd10Light; }
	const ConstantBuffer<cdTime>& Getcd12() const { return m_cd12Time; }
	const ConstantBuffer<cdAtmosphere>& Getcd13() const { return m_cd13Atmosphere; }
};

#pragma region enums
// サンプラーステート
enum SS_FilterMode
{
	ePoint	= 0x00000001, // ドット調
	eLinear = 0x00000002, // 線形補完
	eAniso	= 0x00000004, // 異方性
};
enum SS_AddressMode
{
	eWrap	= 0x00010000, // 繰り返し
	eClamp	= 0x00020000, // 端を無視
	eMirror = 0x00040000, // 反転
};

// ブレンドステート
enum BlendMode
{
	eBlendNone = 0,	// ブレンドしない
	eAdd,			// 加算合成
	eAlpha,			// 色合成
};

// ラスタライザーステート
enum RS_CullMode
{
	eCullNone	= 0x00000001,	// カリングなし
	eBack		= 0x00000002,	// 裏面カリング
	eFront		= 0x00000004,	// 表面カリング
};
enum RS_FillMode
{
	eWire	= 0x00000001,	// ワイヤーフレーム表示
	eSolid	= 0x00000002,	// 塗りつぶし
};
#pragma endregion
