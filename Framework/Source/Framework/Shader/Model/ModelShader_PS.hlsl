//-----------------------------------------------------------------------------
// File: ModelShader_PS.hlsl
//
// Model描画用 ピクセルシェーダー
// 材質の取得 -> ライティング -> 大気の影響
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "ModelShader.hlsli"

// テクスチャ
Texture2D g_baseColorTexture    : register(t0); // 基本色
Texture2D g_emissiveTexture     : register(t1); // エミッシブテクスチャ
Texture2D g_mrTexture           : register(t2); // メタリック/ラフネステクスチャ
Texture2D g_normalTexture       : register(t3); // 法線マップ(テクスチャ)

// サンプラ
SamplerState g_samplerState : register(s0);

// ディザパターン(Bayer Matrix)
// https://en.wikipedia.org/wiki/Ordered_dithering
static const int g_ditherPattern[4][4] = {
    {  0,  8,  2, 10 },
    { 12,  4, 14,  6 },
    {  3, 11,  1,  9 },
    { 15,  7, 13,  5 },
};

// @brief BlinnPhong NDF
// @param lightDir ライトの方向
// @param vCam ピクセルからカメラへの方向
// @param normal 法線
// @param specPower 反射の鋭さ
// @return 反射光の強さ
float BlinnPhong(float3 lightDir, float3 vCam, float3 normal, float specPower)
{
    float3 H = normalize(-lightDir + vCam);
	// カメラの角度差(0～1)
    float NdotH = saturate(dot(normal, H));
    float spec = pow(NdotH, specPower);

	// 正規化Blinn-Phong
    return spec * ((specPower + 2) / (2 * PI));
}

// @brief TrowBridge-Reitz(GGX) NDF
// @param lightDir ライトの方向
// @param vCam ピクセルからカメラへの方向
// @param normal 法線
// @param roughness 材質の粗さ
// @return 反射光の強さ
float GGX(float3 lightDir, float3 vCam, float3 normal, float roughness)
{
    float3 H = normalize(-lightDir + vCam);
    float NdotH = saturate(dot(normal, H));
	
    float alpha = roughness * roughness;
    float d = NdotH * NdotH * (alpha * alpha - 1) + 1;
    d = max(0.000001f, PI * d * d);

    return (alpha * alpha) / d;
}

//-----------------------------------------------------------------------------
// ピクセルシェーダー
//-----------------------------------------------------------------------------
float4 main( VertexOutput In ) : SV_TARGET
{
    //------------------------------------------
    // ディザパターンを使用したディザリング
    //------------------------------------------
    if (g_dither_enable)
    {
        int dx = (int) fmod(In.position.x, 4.0f);
        int dy = (int) fmod(In.position.y, 4.0f);
        // 閾値を取得
        int dither = g_ditherPattern[dy][dx];
        
        // 完全にクリップされる範囲
        float clipRange = 1.2f;
        // ディザリングを始める範囲
        float ditherRange = 4.0f;
        
        float eyeToClipRange = max(0.0f, g_dist_to_eye - clipRange);
        
        // クリップ率を算出
        // todo: 線形じゃない方がいいかも
        float clipRate = 1.0f - min(1.0f, eyeToClipRange / ditherRange);
        
        float ditherMax = 15;
        clip(dither - ditherMax * clipRate);
    }
    
    //------------------------------------------
    // カメラ情報
    //------------------------------------------
    
    // カメラ座標
    float3 cameraPos = g_camera_matrix._41_42_43;
    
    // カメラへの方向
    float3 vCam = cameraPos - In.wPosition.xyz;
    float camDist = length(vCam); // カメラ - ピクセル距離
    vCam = normalize(vCam);
    
    //------------------------------------------
    // 法線
    //------------------------------------------
    // タンジェントスペース
    row_major float3x3 mTBN = {
        normalize(In.wTangent),  // X軸
        normalize(In.wBinormal), // Y軸
        normalize(In.wNormal),   // Z軸
    };
    float3 normal = g_normalTexture.Sample(g_samplerState, In.uv).rgb;
    normal = normal * 2.0f - 1.0f; // (0～1)->(-1～1)
    
	// 面の向きを考慮した方向へ変換
    normal = normalize(mul(normal, mTBN));
    normal = normalize(normal);
    
    if (g_show_normal)
        return float4(normal, 1);
    
    //------------------------------------------
    // 材質色
    //------------------------------------------
    
    // 材質色
    float4 albedo = g_baseColorTexture.Sample(g_samplerState, In.uv) * g_material.m_baseColor * In.color;
    if (g_show_base_color)
        return float4(albedo.rgb, 1);
    
    // メタリック/ラフネス テクスチャ
    float4 colorMR   = g_mrTexture.Sample(g_samplerState, In.uv);
    float metallic   = colorMR.b * g_material.m_metallic;   // 金属性
    float roughuness = colorMR.g * g_material.m_roughness;  // 粗さ
    
    // アルファテスト
    if (albedo.a <= 0.0f) discard;
    
    //==========================================
	//
	// ライティング
	//
	//==========================================
	
	// 最終的な拡散色(光)
    float3 diffuseColor = 0;
	
	// 最終的な反射色(光)
    float3 specularColor = 0;
    
    // ライト計算？
    if (g_light_enable)
    {
        //TODO: ここにシャドウイング追加
        
        //------------------------------------------
        // 平行光
        //------------------------------------------
        
        // Diffuse(拡散反射光)
        {
            float Dot = dot(normal, -g_directional_light_dir);
        
            // 正規化Lambert
            Dot = saturate(Dot);
            float diffusePower = saturate(Dot);
            diffusePower /= PI;
        
            // 物質の色 * 光の色 * 物質の透明度 * 拡散光の強さ
            diffuseColor += albedo.rgb * g_directional_light_color
                * albedo.a * diffusePower/* * shadow*/;
        }
        
        // Specular(鏡面反射光)
        {
			// ラフネスから、GGX用のSpecularPowerを求める
            float smoothness = 1.0f - roughuness; // ラフネスを逆転させ「滑らか」さにする
            float specPower = pow(2, 13 * smoothness); // 1～8192
	  
            // GGX
            float spec = GGX(g_directional_light_dir, vCam, normal, roughuness);
            
            // 光の色 * 反射光の強さ * 材質の反射色 * 正規化係数 * 透明率
            specularColor += (g_directional_light_color * spec) * 0.06f
                * albedo.a/* * shadow*/;
        }
    
        //------------------------------------------
        // 環境光
        //------------------------------------------
        diffuseColor += 0.8f * albedo.rgb * albedo.a;
        
        //------------------------------------------
		// エミッシブ
		//------------------------------------------
        diffuseColor += g_emissiveTexture.Sample(g_samplerState, In.uv).rgb * g_material.m_emissive;
    }
    else
    {
        // ライト無効の際はベースカラーをそのまま使用
        return albedo;
    }
    
    // 拡散色と反射色を混ぜる
    float3 color = diffuseColor + specularColor;
    
    //==========================================
	//
	// 大気の影響
	//
	//==========================================
    
    //------------------------------------------
    // 距離Fog
    //------------------------------------------
    if (g_distance_fog_enable)
    {
        // ピクセルカメラ間の距離
        float dist = distance(cameraPos, In.wPosition.xyz);
        
        float fog = saturate(1.0f / exp(dist * g_distance_fog_rate));
        color.rgb = lerp(g_distance_fog_color, color.rgb, fog);
    }
    
    //------------------------------------------
	// 出力
	//------------------------------------------
    return float4(color, albedo.a);
}