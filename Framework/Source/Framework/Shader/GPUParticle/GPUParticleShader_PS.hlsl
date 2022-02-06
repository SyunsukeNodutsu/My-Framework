//-----------------------------------------------------------------------------
// File: GPUParticleShader_PS.hlsl
//
// ピクセルシェーダ
//-----------------------------------------------------------------------------
#include "../ConstantBuffer.hlsli"
#include "GPUParticleShader.hlsli"

float4 main( VertexOutput In ) : SV_TARGET
{
    //if (color.a <= 0.0f) discard;
    
    return In.color;
}