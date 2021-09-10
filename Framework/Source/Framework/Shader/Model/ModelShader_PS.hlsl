//-----------------------------------------------------------------------------
// File: ModelShader_PS.hlsl
//
// Model�`��p �s�N�Z���V�F�[�_�[
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "ModelShader.hlsli"

// �e�N�X�`��
Texture2D g_baseColorTexture    : register(t0); // ��{�F
Texture2D g_emissiveTexture     : register(t1); // �G�~�b�V�u�e�N�X�`��
Texture2D g_mrTexture           : register(t2); // ���^���b�N/���t�l�X�e�N�X�`��
Texture2D g_normalTexture       : register(t3); // �@���}�b�v(�e�N�X�`��)

// �T���v��
SamplerState g_samplerState : register(s0);

// �f�B�U�p�^�[��
// Bayer Matrix�ɓ��Ă͂܂���Ă��Ƃ��Ǝv��
static const int g_ditherPattern[4][4] = {
    {  0, 32,  8, 40 },
    { 48, 16, 56, 21 },
    { 12, 44,  4, 36 },
    { 60, 28, 52, 20 },
};



// @brief BlinnPhong NDF
// @param lightDir ���C�g�̕���
// @param vCam �s�N�Z������J�����ւ̕���
// @param normal �@��
// @param specPower ���˂̉s��
// @return ���ˌ��̋���
float BlinnPhong(float3 lightDir, float3 vCam, float3 normal, float specPower)
{
    float3 H = normalize(-lightDir + vCam);
	// �J�����̊p�x��(0�`1)
    float NdotH = saturate(dot(normal, H));
    float spec = pow(NdotH, specPower);

	// ���K��Blinn-Phong
    return spec * ((specPower + 2) / (2 * PI));
}

// @brief TrowBridge-Reitz(GGX) NDF
// @param lightDir ���C�g�̕���
// @param vCam �s�N�Z������J�����ւ̕���
// @param normal �@��
// @param roughness �ގ��̑e��
// @return ���ˌ��̋���
float GGX(float3 lightDir, float3 vCam, float3 normal, float roughness)
{
    float3 H = normalize(-lightDir + vCam);
    float NdotH = saturate(dot(normal, H));
	
    float alpha = roughness * roughness;
    float d = NdotH * NdotH * (alpha * alpha - 1) + 1;
    d = max(0.000001, PI * d * d);

    return (alpha * alpha) / d;
}

// @brief ����Fog ���Q�l�Fhttps://iquilezles.org/www/articles/fog/fog.htm
// @param rgb ���̐F
// @param distance �J�����Ƃ̋���
// @param rayOri �J�������W
// @param rayDir �J��������
float3 HeightFog(in float3 rgb, in float distance, in float3 rayOri, in float3 rayDir)
{
    // �V�[���̐��E�X�P�[���ɍ��킹�Čv�Z
    // ���ʏ�̓V�[���̍�������ɐF��t����
    float c = 0.2;
    float b = 0.08;
    
    float fogAmount = c * exp(-rayOri.y * b) * (1.0 - exp(-distance * rayDir.y * b)) / rayDir.y;
    
    // fog color�͋���fog�ƍ��킹���������R����
    return lerp(rgb, g_height_fog_color, fogAmount);
}



//-----------------------------------------------------------------------------
// �s�N�Z���V�F�[�_�[
//-----------------------------------------------------------------------------
float4 main( VertexOutput In ) : SV_TARGET
{
    //------------------------------------------
    // �f�B�U�p�^�[�����g�p�����f�B�U�����O
    //------------------------------------------
    if (g_dither_enable)
    {
        // ���̃s�N�Z���̃X�N���[�����W�ł�X���W Y���W��4�Ŋ������]����Z�o
        int dx = (int) fmod(In.position.x, 4.0f);
        int dy = (int) fmod(In.position.y, 4.0f);
        // ��L�𓥂܂�臒l���擾
        int dither = g_ditherPattern[dy][dx];
        
        // ���S�ɃN���b�v�����͈�
        float clipRange = 1.2f;
        // �f�B�U�����O���n�߂�͈�
        float ditherRange = 4.0f;

        // ���_�ƃN���b�v�͈͂܂ł̋������Z�o
        float eyeToClipRange = max(0.0f, g_dist_to_eye - clipRange);
        
        // �N���b�v�����Z�o
        // todo: ���`����Ȃ�������������
        float clipRate = 1.0f - min(1.0f, eyeToClipRange / ditherRange);

        // �s�N�Z���L��
        clip(dither - 64 * clipRate);
    }
    
    //------------------------------------------
    // �J�������
    //------------------------------------------
    
    // �J�������W
    float3 cameraPos = g_camera_matrix._41_42_43;
    
    // �J�����ւ̕���
    float3 vCam = cameraPos - In.wPosition.xyz;
    float camDist = length(vCam); // �J���� - �s�N�Z������
    vCam = normalize(vCam);
    
    //------------------------------------------
    // �ގ��F todo:
    //------------------------------------------
    
    // ���^���b�N/���t�l�X�e�N�X�`��
    float4 mrColor = 0;
    
    // ������
    float metallic = 0;
    
    // �e��
    float roughuness = 0;
    
    // �x�[�X�J���[�e�N�X�`��
    float4 baseColor = g_baseColorTexture.Sample(g_samplerState, In.uv) * g_baseColor;
    
    // �A���t�@�e�X�g
    if (baseColor.a <= 0.0)
        discard;
    
    //==========================================
	//
	// ���C�e�B���O
	//
	//==========================================
	
	// �ŏI�I�Ȋg�U�F(��)
    float3 diffuseColor = 0;
	
	// �ŏI�I�Ȕ��ːF(��)
    float3 specularColor = 0;
    
    // ���C�g�v�Z�H
    if (g_light_enable)
    {
        //------------------------------------------
        // ���s��
        //------------------------------------------
        
        // Diffuse(�g�U���ˌ�)
        {
            // �@��
            float3 vN = normalize(In.normal);
            float Dot = dot(vN, -g_directional_light_dir);
        
            // ���K��Lambert
            Dot = saturate(Dot);
            float diffusePower = saturate(Dot);
            diffusePower /= PI;
        
            // �F = �����̐F * ���̐F * �����̓����x * �g�U���̋���
            diffuseColor = baseColor.rgb * g_directional_light_color
                * baseColor.a * diffusePower;
        }
        
        // Specular(���ʔ��ˌ�)
        {
			// ���t�l�X����AGGX�p��SpecularPower�����߂�
            float smoothness = 1.0 - g_roughness; // ���t�l�X���t�]�����u���炩�v���ɂ���
            float specPower = pow(2, 13 * smoothness); // 1�`8192
	  
            // GGX
            float spec = GGX(g_directional_light_dir, vCam, In.normal, g_roughness);
            
			// Blinn-Phong NDF
            //float spec = BlinnPhong(g_directional_light_dir, vCam, In.normal, specPower);
            
            // ���̐F * ���ˌ��̋��� * �ގ��̔��ːF * ���K���W�� * ������
            specularColor += (g_directional_light_color * spec) * 0.06 * baseColor.a;
        }
    
        //------------------------------------------
        // ����
        //------------------------------------------
        diffuseColor += g_ambient_power * baseColor.rgb * baseColor.a;
        
        //------------------------------------------
		// �G�~�b�V�u
		//------------------------------------------
        //diffuseColor += g_emissiveTex.Sample(g_ss, In.UV).rgb * g_Material.Emissive;
        
        //------------------------------------------
        // IBL
        //------------------------------------------
        const float mipLevels = 10;     // IBL Mipmap���x����
        const float IBLIntensity = 0.8; // IBL���x
        
        // IBL�g�U���ˌ�
        
        // IBL���ʔ��ˌ�
        
    }
    else
    {
        // ���C�g�����̍ۂ̓x�[�X�J���[�����̂܂܎g�p
        diffuseColor = baseColor.rgb;
    }
    
    // �g�U�F�Ɣ��ːF��������
    float3 color = diffuseColor + specularColor;
    
    //==========================================
	//
	// ��C�̉e��
	//
	//==========================================
    
    //------------------------------------------
    // ����Fog
    //------------------------------------------
    if (g_distance_fog_enable)
    {
        // �s�N�Z���J�����Ԃ̋���
        float dist = distance(cameraPos, In.wPosition.xyz);
        
        float fog = saturate(1.0 / exp(dist * g_distance_fog_rate));
        color.rgb = lerp(g_distance_fog_color, color.rgb, fog);
    }
    
    //------------------------------------------
    // ����Fog
    //------------------------------------------
    if (g_height_fog_enable)
    {
        // �s�N�Z���J�����Ԃ̋���
        float dist = distance(cameraPos, In.wPosition.xyz);
        
        float3 rayDir = In.wPosition.xyz - cameraPos;
        rayDir = normalize(rayDir);
        
        float3 fog = HeightFog(color, dist, cameraPos, rayDir);
        color.rgb = fog;
    }
    
    //------------------------------------------
    // Mie�U��
    //------------------------------------------
    if (g_mie_streuung_enable)
    {
        
    }
    
    //------------------------------------------
    // Rayleigh�U��
    //------------------------------------------
    float g_rayleigh_streuung_enable = 0;// todo:
    if (g_rayleigh_streuung_enable)
    {
        
    }
    
    //------------------------------------------
	// �o��
	//------------------------------------------
    return float4(color, baseColor.a);
}
