//-----------------------------------------------------------------------------
// File: ModelShader_VS_Skin.hlsl
//
// SkinMeshModel描画用 頂点シェーダー
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "ModelShader.hlsli"

//-----------------------------------------------------------------------------
// 頂点シェーダー
//-----------------------------------------------------------------------------
VertexOutput main(
    float4 position     : POSITION,
    float2 uv           : TEXCOORD0,
    float3 normal       : NORMAL,
    float4 color        : COLOR,
    float3 tangent      : TANGENT,
    //スキンメッシュ用
    uint4  skinIndex    : SKININDEX,    //boneインデックス
    float4 skinWeight   : SKINWEIGHT    //影響度
)
{
    VertexOutput ret = (VertexOutput) 0;
    
    //スキニング処理
    row_major float4x4 bones = 0;
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        bones += g_bones[skinIndex[i]] * skinWeight[i];
    }
    
    //座標と法線に適応
    position = mul(position, bones);
    normal = mul(normal, (float3x3) bones);
    
    //以下 通常のVertexShaderと同じ
    ret.position = mul(position, g_world_matrix);
    ret.wPosition = ret.position.xyz;
    ret.position = mul(ret.position, g_view_matrix);
    ret.position = mul(ret.position, g_proj_matrix);
	
    ret.uv = uv * g_uv_tiling + g_uv_offset;
    
    ret.wNormal = normalize(mul(normal, (float3x3) g_world_matrix));
    //ret.wTangent = normalize(mul(tangent, (float3x3) g_world_matrix));
    
    float3 binormal = cross(normal, tangent);
    //ret.wBinormal = normalize(mul(binormal, (float3x3) g_world_matrix));
    
    ret.color = color;
    
    // ライトビュースクリーン空間の座標を計算
    float4 worldPos = mul(g_world_matrix, position);
    ret.posInLVP[0] = mul(g_mLVPC[0], worldPos);
    ret.posInLVP[1] = mul(g_mLVPC[1], worldPos);
    ret.posInLVP[2] = mul(g_mLVPC[2], worldPos);

    return ret;
}