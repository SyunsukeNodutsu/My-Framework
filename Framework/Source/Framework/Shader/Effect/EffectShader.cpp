﻿#include "EffectShader.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
EffectShader::EffectShader()
	: m_cpVS(nullptr)
	, m_cpPS(nullptr)
	, m_cpInputLayout(nullptr)
	, m_cpInputLayout_model(nullptr)
	, m_vertexBuffers()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool EffectShader::Initialize()
{
	HRESULT hr = S_OK;

	//--------------------------------------------------
	// 頂点シェーダ
	//--------------------------------------------------
	{
		#include "EffectShader_VS.shaderinc"

		hr = D3D.GetDevice()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpVS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "エラー：頂点シェーダ作成失敗.");
			return false;
		}

		// Effect用
		{
			// 1頂点の詳細な情報
			std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			hr = D3D.GetDevice()->CreateInputLayout(&layout[0], (UINT)layout.size(), compiledBuffer, sizeof(compiledBuffer), m_cpInputLayout.GetAddressOf());
			if (FAILED(hr)) {
				assert(0 && "エラー：頂点入力レイアウト作成失敗.");
				return false;
			}
		}

		// DrawModel用
		{
			// １頂点の詳細な情報
			std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			// 頂点入力レイアウト作成
			hr = D3D.GetDevice()->CreateInputLayout(&layout[0], (UINT)layout.size(), compiledBuffer, sizeof(compiledBuffer), m_cpInputLayout_model.GetAddressOf());
			if (FAILED(hr)) {
				assert(0 && "エラー：頂点入力レイアウト作成失敗.");
				return false;
			}
		}
	}

	//--------------------------------------------------
	// ピクセルシェーダ
	//--------------------------------------------------
	{
		#include "EffectShader_PS.shaderinc"

		hr = D3D.GetDevice()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpPS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "エラー：ピクセルシェーダ作成失敗.");
			return false;
		}
	}

	//--------------------------------------------------
	// DrawVertices用頂点バッファ作成
	//--------------------------------------------------
	UINT bufferSize = sizeof(Vertex) * 2;
	Buffer tmpBuff = {};
	for (int i = 0; i < 6; i++)
	{
		//tmpBuff.Create(D3D11_BIND_VERTEX_BUFFER, bufferSize, nullptr);
		//m_vertexBuffers.push_back(tmpBuff);

		bufferSize *= 2;// 容量を倍にしていく
	}

	return true;
}

//-----------------------------------------------------------------------------
// 開始
//-----------------------------------------------------------------------------
void EffectShader::Begin()
{
	D3D.GetDeviceContext()->VSSetShader(m_cpVS.Get(), 0, 0);
	D3D.GetDeviceContext()->IASetInputLayout(m_cpInputLayout.Get());

	D3D.GetDeviceContext()->PSSetShader(m_cpPS.Get(), 0, 0);
}

//-----------------------------------------------------------------------------
// 複数頂点の描画
// TODO: バッファの書き込みについて考える
//-----------------------------------------------------------------------------
void EffectShader::DrawVertices(const std::vector<Vertex>& vertices, D3D_PRIMITIVE_TOPOLOGY topology)
{
	if (vertices.size() <= 2)
		return;

	// 1頂点のサイズ
	UINT oneSize = sizeof(Vertex);
	// 合計サイズ
	UINT totalSize = static_cast<UINT>(vertices.size()) * oneSize;

	// 最適な固定長バッファを検索
	Buffer* buffer = nullptr;
	for (auto&& buf : m_vertexBuffers)
	{
		if (totalSize < buf.GetSize())
		{
			buffer = &buf;
			break;
		}
	}

	// TODO: ここ可変長にして対応
	if (buffer == nullptr) {
		assert(0 && "エラー：サイズ超過.");
		return;
	}

	// 全頂点書き込み
	// TODO: ここなんか無駄っぽい
	buffer->WriteData(&vertices[0], totalSize);

	// デバイスに頂点情報設定
	UINT offset = 0;
	D3D.GetDeviceContext()->IASetVertexBuffers(0, 1, buffer->GetAddress(), &oneSize, &offset);

	// プリミティブトポロジー(ポリゴンの描画方法)の設定
	D3D.GetDeviceContext()->IASetPrimitiveTopology(topology);

	// 描画
	D3D.GetDeviceContext()->Draw(static_cast<UINT>(vertices.size()), 0);
}

//-----------------------------------------------------------------------------
// モデル描画
//-----------------------------------------------------------------------------
void EffectShader::DrawModel(ModelWork* modelWork)
{
	if (modelWork == nullptr)
		return;

	if (modelWork->GetData() == nullptr)
		return;

	modelWork->CalcNodeMatrices();

	// 全ノードのメッシュ描画
	for (UINT nodeIndex = 0; nodeIndex < modelWork->GetData()->GetOriginalNodes().size(); nodeIndex++)
	{

	}
}
