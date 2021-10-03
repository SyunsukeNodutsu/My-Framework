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

Texture2D g_shadowMap : register(t10);

// �T���v��
SamplerState g_samplerState : register(s0);
SamplerComparisonState g_shadowSamplerState : register(s10);

// �f�B�U�p�^�[��(Bayer Matrix)
// https://en.wikipedia.org/wiki/Ordered_dithering
static const int g_ditherPattern[4][4] = {
    {  0,  8,  2, 10 },
    { 12,  4, 14,  6 },
    {  3, 11,  1,  9 },
    { 15,  7, 13,  5 },
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

//
float CheckShadow( VertexOutput In )
{
	// �ŏI�I�Ȓl
    float shadow = 1.0f;

	// �s�N�Z����3D���W����A�V���h�E�}�b�v��Ԃ֕ϊ�
    float4 liPos = mul(float4(In.wPosition, 1), g_directional_light_vp);

	// ���g�Ŏˉe���W�ɕϊ�����ꍇ�͉��s�ŕϊ����Ă��(���������܂�)
    liPos.xyz /= liPos.w;
    float shadowX = abs(liPos.x);
    float shadowY = abs(liPos.y);

	// �[�x�}�b�v�͈͓̔����m�F
    if (shadowX <= 1 && shadowY <= 1 && liPos.z <= 1)
    {
		// �ˉe���W -> UV���W�֕ϊ� ���E�����g�傷��΂�������(�C���[�W)
        float2 uv = liPos.xy * float2(1, -1) * 0.5 + 0.5;

		// ���C�g�J��������̋��� ���V���h�E�A�N�l�΍�(ImGui�ŕύX�ł���悤�ɏC������)
        float z = liPos.z - 0.002f;

		// �ڂ���
        float width, height;
        g_shadowMap.GetDimensions(width, height);
        float tw = 1.0f / width;
        float th = 1.0f / height;

		// UV�̎���3x3���l�����A���ϒl���Z�o
        shadow = 0;
        for (int y = -1; y <= 1; y++)
        {
            for (int x = -1; x <= 1; x++)
            {
				// �e����
                shadow += g_shadowMap.SampleCmpLevelZero(g_shadowSamplerState, uv + float2(x * tw, y * th), z);
            }
        }
        shadow *= 0.11f;

		// UV���W���痣��Ă���Ɣ���
        shadow = min(1.0f, shadow + pow(shadowX, 3.0f) + pow(shadowY, 3.0f));
    }

    return shadow;
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
        int dx = (int) fmod(In.position.x, 4.0f);
        int dy = (int) fmod(In.position.y, 4.0f);
        // 臒l���擾
        int dither = g_ditherPattern[dy][dx];
        
        // ���S�ɃN���b�v�����͈�
        float clipRange = 1.2f;
        // �f�B�U�����O���n�߂�͈�
        float ditherRange = 4.0f;
        
        float eyeToClipRange = max(0.0f, g_dist_to_eye - clipRange);
        
        // �N���b�v�����Z�o
        // todo: ���`����Ȃ�������������
        float clipRate = 1.0f - min(1.0f, eyeToClipRange / ditherRange);
        
        clip(dither - 15 * clipRate);
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
    // �^���W�F���g�X�y�[�X
    row_major float3x3 mTBN = {
        normalize(In.wTangent),  // X��
        normalize(In.wBinormal), // Y��
        normalize(In.wNormal),   // Z��
    };
    float3 normal = g_normalTexture.Sample(g_samplerState, In.uv).rgb;
    normal = normal * 2.0f - 1.0f; // (0�`1)->(-1�`1)
    
	// �ʂ̌������l�����������֕ϊ�
    normal = normalize(mul(normal, mTBN));
    normal = normalize(normal);
    
    if (g_show_normal)
        return float4(normal, 1);
    
    //------------------------------------------
    // �ގ��F
    //------------------------------------------
    
    // �ގ��F
    float4 albedo = g_baseColorTexture.Sample(g_samplerState, In.uv) * g_material.m_baseColor * In.color;
    if (g_show_base_color)
        return float4(albedo.rgb, 1);
    
    // ���^���b�N/���t�l�X �e�N�X�`��
    float4 mrColor = g_mrTexture.Sample(g_samplerState, In.uv);
    float metallic   = mrColor.b * g_material.m_metallic;   // ������
    float roughuness = mrColor.g * g_material.m_roughness;  // �e��
    
    // �A���t�@�e�X�g
    if (albedo.a <= 0.0f) discard;
    
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
        // �V���h�E�C���O
        //------------------------------------------
        
        float shadow = 1.0f;

	    // �s�N�Z����3D���W����A�V���h�E�}�b�v��Ԃ֕ϊ�
        float4 liPos = mul(float4(In.wPosition, 1), g_directional_light_vp);

	    // �ˉe���W�ɕϊ�
        liPos.xyz /= liPos.w;
        
        if (abs(liPos.x) <= 1 && abs(liPos.y) <= 1 && liPos.z <= 1)
        {
		    // �ˉe���W -> UV���W�֕ϊ�
            float2 uv = liPos.xy * float2(1, -1) * 0.5f + 0.5f;
            
		    // ���C�g�J��������̋���
            float z = liPos.z - 0.002f;
            
		    // �e����
            shadow = g_shadowMap.Sample(g_samplerState, uv).r < z ? 0 : 1;
        }
        
        //------------------------------------------
        // ���s��
        //------------------------------------------
        
        // Diffuse(�g�U���ˌ�)
        {
            float Dot = dot(normal, -g_directional_light_dir);
        
            // ���K��Lambert
            Dot = saturate(Dot);
            float diffusePower = saturate(Dot);
            diffusePower /= PI;
        
            // �����̐F * ���̐F * �����̓����x * �g�U���̋���
            diffuseColor += albedo.rgb * g_directional_light_color
                * albedo.a * diffusePower * shadow;
        }
        
        // Specular(���ʔ��ˌ�)
        {
			// ���t�l�X����AGGX�p��SpecularPower�����߂�
            float smoothness = 1.0f - roughuness; // ���t�l�X���t�]�����u���炩�v���ɂ���
            float specPower = pow(2, 13 * smoothness); // 1�`8192
	  
            // GGX
            float spec = GGX(g_directional_light_dir, vCam, normal, roughuness);
            
            // ���̐F * ���ˌ��̋��� * �ގ��̔��ːF * ���K���W�� * ������
            specularColor += (g_directional_light_color * spec) * 0.06f
                * albedo.a * shadow;
        }
    
        //------------------------------------------
        // ����
        //------------------------------------------
        diffuseColor += 0.8f * albedo.rgb * albedo.a;
        
        //------------------------------------------
		// �G�~�b�V�u
		//------------------------------------------
        diffuseColor += g_emissiveTexture.Sample(g_samplerState, In.uv).rgb * g_material.m_emissive;
    }
    else
    {
        // ���C�g�����̍ۂ̓x�[�X�J���[�����̂܂܎g�p
        return albedo;
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
    return float4(color, albedo.a);
}