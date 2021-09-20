//-----------------------------------------------------------------------------
// File: Shader.h
//
// シェーダー基底クラス
//-----------------------------------------------------------------------------
#pragma once
#include "../Graphics/GraphicsDeviceChild.h"

// シェーダ基底クラス
class Shader : public GraphicsDeviceChild
{
public:

	// @brief コンストラクタ
	Shader();

protected:

	ComPtr<ID3D11VertexShader>	m_cpVS;
	ComPtr<ID3D11PixelShader>	m_cpPS;
	ComPtr<ID3D11InputLayout>	m_cpInputLayout;

};
