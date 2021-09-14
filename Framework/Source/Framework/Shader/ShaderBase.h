//-----------------------------------------------------------------------------
// File: ShaderBase.h
//
// シェーダー基底クラス
// 共通に持つべき「VS layout PS」を所持
//-----------------------------------------------------------------------------
#pragma once

// シェーダ基底クラス
class ShaderBase
{
public:

	// @brief コンストラクタ
	ShaderBase();

	// 初期化タイミング
	virtual void OnInitialize() {}
	// 終了タイミング
	virtual void OnFinalize() {}
	// GPUに転送タイミング
	virtual void OnSetToDevice() {}

private:

	ComPtr<ID3D11VertexShader>	m_cpVS;
	ComPtr<ID3D11PixelShader>	m_cpPS;
	ComPtr<ID3D11InputLayout>	m_cpInputLayout;

};
