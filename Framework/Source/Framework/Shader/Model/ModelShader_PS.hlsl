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

// �f�B�U�p�^�[��(Bayer Matrix)
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
    d = max(0.000001f, PI * d * d);

    return (alpha * alpha) / d;
}

//-----------------------------------------------------------------------------
// �s�N�Z���V�F�[�_�[
//-----------------------------------------------------------------------------
float4 main(VertexOutput In) : SV_TARGET
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
    // �@��
    //------------------------------------------
    // 3x3�s��(��]�s��)
    row_major float3x3 mTBN = {
        normalize(In.wTangent),  // X��
        normalize(In.wBinormal), // Y��
        normalize(In.wNormal),   // Z��
    };
    float3 wN = g_normalTexture.Sample(g_samplerState, In.uv).rgb;
    wN = wN * 2.0f - 1.0f; // (0�`1)->(-1�`1)
    
	// �ʂ̌������l�����������֕ϊ�
    wN = normalize(mul(wN, mTBN));
    wN = normalize(wN);
    
    //return float4(wN, 1);
    
    //------------------------------------------
    // �ގ��F
    //------------------------------------------
    
    // �ގ��F
    float4 baseColor = g_baseColorTexture.Sample(g_samplerState, In.uv) * g_material.m_baseColor * In.color;
    
    // ���^���b�N/���t�l�X �e�N�X�`��
    float4 mrColor = g_mrTexture.Sample(g_samplerState, In.uv);
    float metallic   = mrColor.b * g_material.m_metallic;   // ������
    float roughuness = mrColor.g * g_material.m_roughness;  // �e��
    
    // �A���t�@�e�X�g
    if (baseColor.a <= 0.0f)
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
            float Dot = dot(wN, -g_directional_light_dir);
        
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
            float smoothness = 1.0f - roughuness; // ���t�l�X���t�]�����u���炩�v���ɂ���
            float specPower = pow(2, 13 * smoothness); // 1�`8192
	  
            // GGX
            float spec = GGX(g_directional_light_dir, vCam, wN, roughuness);
            
            // ���̐F * ���ˌ��̋��� * �ގ��̔��ːF * ���K���W�� * ������
            specularColor += (g_directional_light_color * spec) * 0.06f * baseColor.a;
        }
    
        //------------------------------------------
        // ����
        //------------------------------------------
        diffuseColor += 0.8f * baseColor.rgb * baseColor.a;
        
        //------------------------------------------
		// �G�~�b�V�u
		//------------------------------------------
        diffuseColor += g_emissiveTexture.Sample(g_samplerState, In.uv).rgb * g_material.m_emissive;
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
        
        float fog = saturate(1.0f / exp(dist * g_distance_fog_rate));
        color.rgb = lerp(g_distance_fog_color, color.rgb, fog);
    }
    
    //------------------------------------------
	// �o��
	//------------------------------------------
    return float4(color, baseColor.a);
}