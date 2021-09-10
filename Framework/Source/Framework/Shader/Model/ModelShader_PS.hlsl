//-----------------------------------------------------------------------------
// File: ModelShader_PS.hlsl
//
// Model描画用 ピクセルシェーダー
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

// ディザパターン
// Bayer Matrixに当てはまるってことだと思う
static const int g_ditherPattern[4][4] = {
    {  0, 32,  8, 40 },
    { 48, 16, 56, 21 },
    { 12, 44,  4, 36 },
    { 60, 28, 52, 20 },
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
    d = max(0.000001, PI * d * d);

    return (alpha * alpha) / d;
}

// @brief 高さFog ※参考：https://iquilezles.org/www/articles/fog/fog.htm
// @param rgb 元の色
// @param distance カメラとの距離
// @param rayOri カメラ座標
// @param rayDir カメラ方向
float3 HeightFog(in float3 rgb, in float distance, in float3 rayOri, in float3 rayDir)
{
    // シーンの世界スケールに合わせて計算
    // ※通常はシーンの高さを基準に色を付ける
    float c = 0.2;
    float b = 0.08;
    
    float fogAmount = c * exp(-rayOri.y * b) * (1.0 - exp(-distance * rayDir.y * b)) / rayDir.y;
    
    // fog colorは距離fogと合わせた方が自然かも
    return lerp(rgb, g_height_fog_color, fogAmount);
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
        // このピクセルのスクリーン座標でのX座標 Y座標を4で割った余りを算出
        int dx = (int) fmod(In.position.x, 4.0f);
        int dy = (int) fmod(In.position.y, 4.0f);
        // 上記を踏まえ閾値を取得
        int dither = g_ditherPattern[dy][dx];
        
        // 完全にクリップされる範囲
        float clipRange = 1.2f;
        // ディザリングを始める範囲
        float ditherRange = 4.0f;

        // 視点とクリップ範囲までの距離を算出
        float eyeToClipRange = max(0.0f, g_dist_to_eye - clipRange);
        
        // クリップ率を算出
        // todo: 線形じゃない方がいいかも
        float clipRate = 1.0f - min(1.0f, eyeToClipRange / ditherRange);

        // ピクセルキル
        clip(dither - 64 * clipRate);
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
    // 材質色 todo:
    //------------------------------------------
    
    // メタリック/ラフネステクスチャ
    float4 mrColor = 0;
    
    // 金属性
    float metallic = 0;
    
    // 粗さ
    float roughuness = 0;
    
    // ベースカラーテクスチャ
    float4 baseColor = g_baseColorTexture.Sample(g_samplerState, In.uv) * g_baseColor;
    
    // アルファテスト
    if (baseColor.a <= 0.0)
        discard;
    
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
        //------------------------------------------
        // 平行光
        //------------------------------------------
        
        // Diffuse(拡散反射光)
        {
            // 法線
            float3 vN = normalize(In.normal);
            float Dot = dot(vN, -g_directional_light_dir);
        
            // 正規化Lambert
            Dot = saturate(Dot);
            float diffusePower = saturate(Dot);
            diffusePower /= PI;
        
            // 色 = 物質の色 * 光の色 * 物質の透明度 * 拡散光の強さ
            diffuseColor = baseColor.rgb * g_directional_light_color
                * baseColor.a * diffusePower;
        }
        
        // Specular(鏡面反射光)
        {
			// ラフネスから、GGX用のSpecularPowerを求める
            float smoothness = 1.0 - g_roughness; // ラフネスを逆転させ「滑らか」さにする
            float specPower = pow(2, 13 * smoothness); // 1～8192
	  
            // GGX
            float spec = GGX(g_directional_light_dir, vCam, In.normal, g_roughness);
            
			// Blinn-Phong NDF
            //float spec = BlinnPhong(g_directional_light_dir, vCam, In.normal, specPower);
            
            // 光の色 * 反射光の強さ * 材質の反射色 * 正規化係数 * 透明率
            specularColor += (g_directional_light_color * spec) * 0.06 * baseColor.a;
        }
    
        //------------------------------------------
        // 環境光
        //------------------------------------------
        diffuseColor += g_ambient_power * baseColor.rgb * baseColor.a;
        
        //------------------------------------------
		// エミッシブ
		//------------------------------------------
        //diffuseColor += g_emissiveTex.Sample(g_ss, In.UV).rgb * g_Material.Emissive;
        
        //------------------------------------------
        // IBL
        //------------------------------------------
        const float mipLevels = 10;     // IBL Mipmapレベル数
        const float IBLIntensity = 0.8; // IBL強度
        
        // IBL拡散反射光
        
        // IBL鏡面反射光
        
    }
    else
    {
        // ライト無効の際はベースカラーをそのまま使用
        diffuseColor = baseColor.rgb;
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
        
        float fog = saturate(1.0 / exp(dist * g_distance_fog_rate));
        color.rgb = lerp(g_distance_fog_color, color.rgb, fog);
    }
    
    //------------------------------------------
    // 高さFog
    //------------------------------------------
    if (g_height_fog_enable)
    {
        // ピクセルカメラ間の距離
        float dist = distance(cameraPos, In.wPosition.xyz);
        
        float3 rayDir = In.wPosition.xyz - cameraPos;
        rayDir = normalize(rayDir);
        
        float3 fog = HeightFog(color, dist, cameraPos, rayDir);
        color.rgb = fog;
    }
    
    //------------------------------------------
    // Mie散乱
    //------------------------------------------
    if (g_mie_streuung_enable)
    {
        
    }
    
    //------------------------------------------
    // Rayleigh散乱
    //------------------------------------------
    float g_rayleigh_streuung_enable = 0;// todo:
    if (g_rayleigh_streuung_enable)
    {
        
    }
    
    //------------------------------------------
	// 出力
	//------------------------------------------
    return float4(color, baseColor.a);
}
