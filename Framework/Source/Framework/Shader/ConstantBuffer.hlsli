//-----------------------------------------------------------------------------
// File: ConstantBuffer.hlsli
//
// 全シェーダ共通
//-----------------------------------------------------------------------------

//--------------------------------------------------
// 定数バッファ
//--------------------------------------------------

// シャドウ パラメータ
cbuffer cdObject : register(b2)
{
    row_major float4x4 g_mLVPC[3]; // ライトビュープロジェクションクロップ行列
}

// キャラクターに付与予定
cbuffer cdObject : register(b8)
{
    row_major float4x4  g_world_matrix; // キャラクター用 ワールド行列
    float2              g_uv_offset;    // UVスクロール用
    float2              g_uv_tiling;    // UVタイリング用
    float               g_dist_to_eye;  // カメラとの距離
    float               g_dither_enable;// ディザリング有効？
}

// ビュー 射影 変換行列
cbuffer cdObject : register(b9)
{
    row_major float4x4 g_view_matrix;   // ビュー変換
    row_major float4x4 g_proj_matrix;   // 射影変換
    row_major float4x4 g_camera_matrix; // カメラ
}

// ライト
cbuffer cdObject : register(b10)
{
    float   g_light_enable;             // ライト有効？
    float3  g_directional_light_dir;    // 方向
    float3  g_directional_light_color;  // 色
    float   g_ambient_power;            // 環境光の強さ
    
    row_major float4x4 g_directional_light_vp; // ライトカメラのビュー行列*射影行列
}

// マテリアル
struct Material
{
    float4  m_baseColor; // 色
    float3  m_emissive;  // 自己発行
    float   m_metallic;  // 金属性
    float   m_roughness; // 粗さ
};
cbuffer cdObject : register(b11)
{
    Material g_material;
}

// 時間
cbuffer cdObject : register(b12)
{
    float g_total_time; // 総経過時間
    float g_delta_time; // デルタティック
}

// 大気
cbuffer cdObject : register(b13)
{
    float3  g_sun_position;         // 太陽の座標

    // 距離Fog
    float   g_distance_fog_enable;  // 距離フォグ有効？
    float3  g_distance_fog_color;   // 距離フォグ色
    float   g_distance_fog_rate;    // 距離フォグ減衰率
    
    float   g_height_fog_enable;    // 高さフォグ有効？
    float3  g_height_fog_color;     // 高さフォグ色
    
    // Mie散乱
    float   g_mie_streuung_enable;  // Mie散乱有効？
    float   g_mie_streuung_factor_coefficient; // Mie散乱因子係数(-0.75 ～ -0.999)
    
    // Rayleigh散乱
    
}

// Shaderデバッグ
cbuffer cdObject : register(b7)
{
    float g_show_base_color;    // PSの出力をそのままの色に(ライト無効)
    float g_show_normal;        // PSの出力を法線に
}



//--------------------------------------------------
// defines
//--------------------------------------------------

// 算術
#define PI 3.14159265359 // 円周率



//--------------------------------------------------
// Helpers
//--------------------------------------------------

// @brief 0～1のランダムな浮動小数点数を返します
// @note 注意.seedは0以上
// @param seed 生成シード値
// @param uv 通常はUVを設定するよ
// @return 0～1のランダムな浮動小数点数
inline float Rand(inout float seed, in float2 uv)
{
    float result = frac(sin(seed * dot(uv, float2(12.9898, 78.233))) * 43758.5453);
    seed += 1;
    return result;
}

// @brief 通常のデプスからリニアデプスを算出
// @param z 深度値
// @param near 最近点
// @param far 最望遠点
inline float GetLinearDepth(in float z, in float near, in float far)
{
    float z_n = 2 * z - 1;
    float lin = 2 * far * near / (near + far - z_n * (near - far));
    return lin;
}

// @brief ハッシュ関数
// @param n ハッシュ化する値
float hash(float n)
{
    return frac(sin(n) * 43758.5453);
}

// @brief 3次元ベクトルからシンプレックスノイズを生成
// @param x 3次元ベクトル ソース
float SimplexNoise(float3 x)
{
    // The noise function returns a value in the range -1.0f -> 1.0f
    float3 p = floor(x);
    float3 f = frac(x);

    f = f * f * (3.0 - 2.0 * f);
    float n = p.x + p.y * 57.0 + 113.0 * p.z;
    
    return lerp(lerp(lerp(hash(n + 0.0), hash(n + 1.0), f.x),
                     lerp(hash(n + 57.0), hash(n + 58.0), f.x), f.y),
                lerp(lerp(hash(n + 113.0), hash(n + 114.0), f.x),
                     lerp(hash(n + 170.0), hash(n + 171.0), f.x), f.y), f.z);
}