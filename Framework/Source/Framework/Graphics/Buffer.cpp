#include "Buffer.h"

//-----------------------------------------------------------------------------
// 作成
//-----------------------------------------------------------------------------
bool Buffer::Create(UINT bindFlags, UINT bufferSize, D3D11_USAGE bufferUsage, const D3D11_SUBRESOURCE_DATA* initData)
{
	// アライメント チェック
	if (bindFlags == D3D11_BIND_CONSTANT_BUFFER)
	{
		if (bufferSize % 16 != 0) {
			assert(0 && "エラー：定数バッファ アライメント.");
			return false;
		}
		constexpr int alignmentSize = 16;
		//bufferSize = ((bufferSize + alignmentSize - 1) / alignmentSize) * alignmentSize;
	}

	// バッファ作成のための詳細データ
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = bindFlags;		// デバイスにバインドする際の種類
	desc.ByteWidth = bufferSize;	// 作成するバッファのバイトサイズ
	desc.MiscFlags = 0;				// その他のフラグ
	desc.StructureByteStride = 0;	// 構造化バッファの場合、その構造体のサイズ
	desc.Usage = bufferUsage;		// 作成するバッファの使用法

	// CPUアクセスレベル C++(CPU)側から書き換え可能
	switch (desc.Usage)
	{
		// 動的ビデオメモリバッファ
	case D3D11_USAGE_DYNAMIC:
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;

		// 静的ビデオメモリバッファ
	case D3D11_USAGE_DEFAULT:
		desc.CPUAccessFlags = 0;
		break;

		// ステージングバッファ
	case D3D11_USAGE_STAGING:
		desc.BindFlags = 0;// ステージングの場合は0
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		break;
	}

	// 作成
	if (FAILED(g_graphicsDevice->g_cpDevice.Get()->CreateBuffer(&desc, initData, &m_cpBuffer))) {
		assert(0 && "エラー：バッファ作成失敗.");
		return false;
	}

	m_usage = bufferUsage;
	m_size	= bufferSize;

	return true;
}

//-----------------------------------------------------------------------------
// 構造化バッファを作成
//-----------------------------------------------------------------------------
bool Buffer::CreateStructured(UINT elementSize, UINT count, bool isUAV, const D3D11_SUBRESOURCE_DATA* initData)
{
	// バッファ作成のための詳細データ
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth				= elementSize * count;
	desc.MiscFlags				= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride	= elementSize;
	desc.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;

	if (isUAV) {
		//順不同アクセスビューの場合
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		m_usage = desc.Usage = D3D11_USAGE_DEFAULT;
	}
	else {
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		m_usage = desc.Usage = D3D11_USAGE_DYNAMIC;
	}

	// 作成
	if (FAILED(g_graphicsDevice->g_cpDevice.Get()->CreateBuffer(&desc, initData, &m_cpBuffer))) {
		assert(0 && "エラー：バッファ作成失敗.");
		return false;
	}

	m_size = elementSize * count;

	return true;
}

//-----------------------------------------------------------------------------
// 書き込み
//-----------------------------------------------------------------------------
void Buffer::WriteData(const void* srcData, UINT size)
{
	// 動的バッファの場合
	if (m_usage == D3D11_USAGE_DYNAMIC)
	{
		D3D11_MAPPED_SUBRESOURCE pData;
		if (SUCCEEDED(g_graphicsDevice->g_cpContext.Get()->Map(m_cpBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
		{
			memcpy_s(pData.pData, m_size, srcData, size);
			g_graphicsDevice->g_cpContext.Get()->Unmap(m_cpBuffer.Get(), 0);
		}
	}
	// 静的バッファの場合
	else if (m_usage == D3D11_USAGE_DEFAULT)
	{
		g_graphicsDevice->g_cpContext.Get()->UpdateSubresource(m_cpBuffer.Get(), 0, 0, srcData, 0, 0);
	}
	// ステージングバッファの場合(読み書き両方)
	else if (m_usage == D3D11_USAGE_STAGING)
	{
		D3D11_MAPPED_SUBRESOURCE pData;
		if (SUCCEEDED(g_graphicsDevice->g_cpContext.Get()->Map(m_cpBuffer.Get(), 0, D3D11_MAP_READ_WRITE, 0, &pData)))
		{
			memcpy_s(pData.pData, m_size, srcData, size);
			g_graphicsDevice->g_cpContext.Get()->Unmap(m_cpBuffer.Get(), 0);
		}
	}
}

//-----------------------------------------------------------------------------
// コピー
//-----------------------------------------------------------------------------
void Buffer::CopyFrom(const Buffer& srcBuffer)
{
	if (m_cpBuffer.Get() == nullptr) return;
	if (srcBuffer.Get() == nullptr) return;

	g_graphicsDevice->g_cpContext.Get()->CopyResource(m_cpBuffer.Get(), srcBuffer.Get());
}

//-----------------------------------------------------------------------------
// UAVのバッファの内容を CPU から読み込み可能なバッファへコピーする
//-----------------------------------------------------------------------------
ID3D11Buffer* Buffer::CreateAndCopyToDebugBuf(ID3D11Buffer* pBuffer)
{
	ID3D11Buffer* debugbuf = nullptr;

	D3D11_BUFFER_DESC desc = {};
	pBuffer->GetDesc(&desc);

	//下記変更点
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	if (SUCCEEDED(g_graphicsDevice->g_cpDevice.Get()->CreateBuffer(&desc, nullptr, &debugbuf)))
		g_graphicsDevice->g_cpContext.Get()->CopyResource(debugbuf, pBuffer);
	return debugbuf;
}

/*
■bit演算
アライメント済みサイズ = 元のサイズ + 255 & ~255;

計算してみる(4byte幅だけど2進表記のときは不要な値は省いてるよ)
result = 64 + 255 & ~255(1111`1111)
result = 64 + 255 & 1`0000`0000
result = 312 & 1`0000`0000
result = 1`0011`1000 & 1`0000`0000
result = 1`0000`0000
result = 256

■四則演算
アライメント済みサイズ = 元のサイズ + (256 - 元のサイズ % 256)

アライメント済みサイズ = 元のサイズ + (256 - [元のサイズの256からはみ出る部分がでる])
アライメント済みサイズ = 元のサイズ + (元のサイズと足すと256になるサイズになる)
アライメント済みサイズ = 256でアラインメントされたサイズになる

計算してみる
result = 64 + (256 - 64 % 256)
result = 64 + (256 - 64)
result = 64 + 192
result = 256
*/
