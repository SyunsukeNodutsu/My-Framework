//-----------------------------------------------------------------------------
// File: ConstantBuffer.hlsli
//
// �S�V�F�[�_����
//-----------------------------------------------------------------------------

//--------------------------------------------------
// �萔�o�b�t�@
//--------------------------------------------------

// �V���h�E �p�����[�^
cbuffer cdObject : register(b2)
{
    row_major float4x4 g_mLVPC[3]; // ���C�g�r���[�v���W�F�N�V�����N���b�v�s��
}

// �L�����N�^�[�ɕt�^�\��
cbuffer cdObject : register(b8)
{
    row_major float4x4  g_world_matrix; // �L�����N�^�[�p ���[���h�s��
    float2              g_uv_offset;    // UV�X�N���[���p
    float2              g_uv_tiling;    // UV�^�C�����O�p
    float               g_dist_to_eye;  // �J�����Ƃ̋���
    float               g_dither_enable;// �f�B�U�����O�L���H
}

// �r���[ �ˉe �ϊ��s��
cbuffer cdObject : register(b9)
{
    row_major float4x4 g_view_matrix;   // �r���[�ϊ�
    row_major float4x4 g_proj_matrix;   // �ˉe�ϊ�
    row_major float4x4 g_camera_matrix; // �J����
}

// ���C�g
cbuffer cdObject : register(b10)
{
    float   g_light_enable;             // ���C�g�L���H
    float3  g_directional_light_dir;    // ����
    float3  g_directional_light_color;  // �F
    float   g_ambient_power;            // �����̋���
    
    row_major float4x4 g_directional_light_vp; // ���C�g�J�����̃r���[�s��*�ˉe�s��
}

// �}�e���A��
struct Material
{
    float4  m_baseColor; // �F
    float3  m_emissive;  // ���Ȕ��s
    float   m_metallic;  // ������
    float   m_roughness; // �e��
};
cbuffer cdObject : register(b11)
{
    Material g_material;
}

// ����
cbuffer cdObject : register(b12)
{
    float g_total_time; // ���o�ߎ���
    float g_delta_time; // �f���^�e�B�b�N
}

// ��C
cbuffer cdObject : register(b13)
{
    float3  g_sun_position;         // ���z�̍��W

    // ����Fog
    float   g_distance_fog_enable;  // �����t�H�O�L���H
    float3  g_distance_fog_color;   // �����t�H�O�F
    float   g_distance_fog_rate;    // �����t�H�O������
    
    float   g_height_fog_enable;    // �����t�H�O�L���H
    float3  g_height_fog_color;     // �����t�H�O�F
    
    // Mie�U��
    float   g_mie_streuung_enable;  // Mie�U���L���H
    float   g_mie_streuung_factor_coefficient; // Mie�U�����q�W��(-0.75 �` -0.999)
    
    // Rayleigh�U��
    
}

// Shader�f�o�b�O
cbuffer cdObject : register(b7)
{
    float g_show_base_color;    // PS�̏o�͂����̂܂܂̐F��(���C�g����)
    float g_show_normal;        // PS�̏o�͂�@����
}



//--------------------------------------------------
// defines
//--------------------------------------------------

// �Z�p
#define PI 3.14159265359 // �~����



//--------------------------------------------------
// Helpers
//--------------------------------------------------

// @brief 0�`1�̃����_���ȕ��������_����Ԃ��܂�
// @note ����.seed��0�ȏ�
// @param seed �����V�[�h�l
// @param uv �ʏ��UV��ݒ肷���
// @return 0�`1�̃����_���ȕ��������_��
inline float Rand(inout float seed, in float2 uv)
{
    float result = frac(sin(seed * dot(uv, float2(12.9898, 78.233))) * 43758.5453);
    seed += 1;
    return result;
}

// @brief �ʏ�̃f�v�X���烊�j�A�f�v�X���Z�o
// @param z �[�x�l
// @param near �ŋߓ_
// @param far �Ŗ]���_
inline float GetLinearDepth(in float z, in float near, in float far)
{
    float z_n = 2 * z - 1;
    float lin = 2 * far * near / (near + far - z_n * (near - far));
    return lin;
}

// @brief �n�b�V���֐�
// @param n �n�b�V��������l
float hash(float n)
{
    return frac(sin(n) * 43758.5453);
}

// @brief 3�����x�N�g������V���v���b�N�X�m�C�Y�𐶐�
// @param x 3�����x�N�g�� �\�[�X
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