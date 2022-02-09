//-----------------------------------------------------------------------------
// File: Buffer.h
//
// ID3D11Bufferをラップ.扱いやすく
// バッファは基本的に一度作成したものを書き換えながら使いまわす
//-----------------------------------------------------------------------------
#pragma once
#include "GraphicsDeviceChild.h"

// バッファクラス
class Buffer : public GraphicsDeviceChild
{
public:

	// @brief コンストラクタ
	Buffer() {}

	// @brief デストラクタ
	~Buffer() = default;

	// @brief バッファを作成
	// @param bindFlags どのバッファとしてDirect3Dへバインドするかのフラグ　D3D11_BIND_FLAG定数を指定する
	// @param bufferSize 作成するバッファのサイズ(byte)
	// @param bufferUsage バッファの使用法　D3D11_USAGE定数を指定する
	// @param initData 作成時に書き込むデータ nullptrだと何も書き込まない
	bool Create(UINT bindFlags, UINT bufferSize, D3D11_USAGE bufferUsage, const D3D11_SUBRESOURCE_DATA* initData);

	// @brief 構造化バッファを作成
	// @param uElementSize 構造体のサイズ
	// @param count 要素数
	// @param isUAV 順不同アクセス使用想定
	bool CreateStructured(UINT elementSize, UINT count, bool isUAV, const D3D11_SUBRESOURCE_DATA* initData = nullptr);

	// @brief バッファへ指定データを書き込み
	// @param pSrcData 書き込みたいデータの先頭アドレス
	// @param size 書き込むサイズ(byte)
	void WriteData(const void* srcData, UINT size);

	// @brief GPU上でバッファのコピーを実行する
	// @param srcBuffer コピー元バッファ
	void CopyFrom(const Buffer& srcBuffer);

	// UAVのバッファの内容を CPU から読み込み可能なバッファへコピーして返す
	static ID3D11Buffer* CreateAndCopyToDebugBuf(ID3D11Buffer* pBuffer);

	//--------------------------------------------------
	// 取得
	//--------------------------------------------------

	// @brief バッファインターフェースを返す
	// @return バッファインターフェース(ポインタ)
	ID3D11Buffer* Get() const { return m_cpBuffer.Get(); }

	// @brief バッファインターフェースを返す
	// @return バッファインターフェース(ダブルポインタ)
	ID3D11Buffer* const* GetAddress() const { return m_cpBuffer.GetAddressOf(); }

	// @brief バッファサイズを返す
	// @return バッファのサイズ
	UINT GetSize() const { return m_size; }

protected:

	ComPtr<ID3D11Buffer>	m_cpBuffer	= nullptr;				// バッファ本体
	UINT					m_size		= 0;					// バッファのサイズ(byte)
	D3D11_USAGE				m_usage		= D3D11_USAGE_DEFAULT;	// バッファの使用法

private:

	Buffer(const Buffer& src) = delete;
	void operator=(const Buffer& src) = delete;

};



// 作業データ付き 定数バッファクラス
// TODO: Buffer継承でいい
template<class DataType>
class ConstantBuffer : public GraphicsDeviceChild
{
public:

	// @brief コンストラクタ
	ConstantBuffer() = default;

	// @brief デストラクタ
	~ConstantBuffer() = default;

	// @brief コンスタンとバッファー用に作成
	// @param initData 作成時にバッファに書き込むデータ nullptrで何も書き込まない
	// @return 成功...true
	bool Create(const DataType* initData = nullptr)
	{
		if (initData)
			m_work = *initData;

		// 書き込むデータ
		D3D11_SUBRESOURCE_DATA srd;
		srd.pSysMem			 = &m_work;
		srd.SysMemPitch		 = 0;
		srd.SysMemSlicePitch = 0;
		return m_buffer.Create(D3D11_BIND_CONSTANT_BUFFER, sizeof(DataType), D3D11_USAGE_DYNAMIC, &srd);
	}

	// @brief 定数バッファへ書き込む
	// @note m_isDirtyがtrueの時のみ、バッファに書き込まれる
	void Write()
	{
		if (!m_isDirty) return;

		m_buffer.WriteData(&m_work, m_buffer.GetSize());
		m_isDirty = false;
	}

	//--------------------------------------------------
	// 取得
	//--------------------------------------------------

	// @brief バッファアドレスを返す
	// @return バッファアドレス
	ID3D11Buffer* const* GetAddress() const { return m_buffer.GetAddress(); }

	// @brief 作業領域を返す
	// @return 操作可能な作業領域
	DataType& Work() { m_isDirty = true; return m_work; }

	// @brief 作業領域取得(読み取り専用)
	// @return 読み取り専用の作業領域
	const DataType& Get() const { return m_work; }

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------

	// @brief 頂点シェーダにセット
	// @param slot スロット番号
	inline void VSSetToDevice(int slot) const {
		g_graphicsDevice->g_cpContext.Get()->VSSetConstantBuffers(slot, 1, m_buffer.GetAddress());
	}

	// @brief ピクセルシェーダにセット
	// @param slot スロット番号
	inline void PSSetToDevice(int slot) const {
		g_graphicsDevice->g_cpContext.Get()->PSSetConstantBuffers(slot, 1, m_buffer.GetAddress());
	}

	// @brief 計算シェーダにセット
	// @param slot スロット番号
	inline void CSSetToDevice(int slot) const {
		g_graphicsDevice->g_cpContext.Get()->CSSetConstantBuffers(slot, 1, m_buffer.GetAddress());
	}

	// @brief 各シェーダにセット
	// @param slot スロット番号
	inline void SetToDevice(int slot) const {
		g_graphicsDevice->g_cpContext.Get()->VSSetConstantBuffers(slot, 1, m_buffer.GetAddress());
		g_graphicsDevice->g_cpContext.Get()->PSSetConstantBuffers(slot, 1, m_buffer.GetAddress());
		g_graphicsDevice->g_cpContext.Get()->CSSetConstantBuffers(slot, 1, m_buffer.GetAddress());
	}

private:

	Buffer		m_buffer;			// 定数バッファ
	DataType	m_work;				// 作業用定数バッファ ※内容
	bool		m_isDirty = true;	// データ更新フラグ

private:

	ConstantBuffer(const ConstantBuffer& src) = delete;
	void operator=(const ConstantBuffer& src) = delete;
	//ConstantBuffer& operator= (const ConstantBuffer&) = delete;

};



// スレッドセーフなリングバッファ
// @param DataType データ
// @param capacity 確保するサイズ
// https://github.com/turanszkij/WickedEngine/blob/master/WickedEngine/wiContainers.h
template <typename DataType, size_t capacity>
class ThreadSafeRingBuffer
{
public:

	// @brief コンストラクタ
	ThreadSafeRingBuffer() = default;

	// @brief デストラクタ
	~ThreadSafeRingBuffer()
	{
		auto size = sizeof(data);
		DebugLog(std::string("リングバッファサイズ" + std::to_string(size) + "\n").c_str());
	}

	// @brief 空き容量があれば、アイテムを最後までプッシュする
	// @return 成功...true 十分なスペースがない...false
	inline bool push_back(const DataType& item)
	{
		bool result = false;
		memoryOrder.Lock();
		size_t next = (head + 1) % capacity;// インデックスをずらす
		if (next != tail)// 空き容量確認
		{
			data[head] = item;// 書き込み
			head = next;
			result = true;
		}
		memoryOrder.Unlock();
		return result;
	}

	// @brief アイテムがあれば、それを取得する
	// @return 成功...true アイテムがない...false
	inline bool pop_front(DataType& item)
	{
		bool result = false;
		memoryOrder.Lock();
		if (tail != head)
		{
			item = data[tail];// 取り出し
			tail = (tail + 1) % capacity;// インデックスをずらす
			result = true;
		}
		memoryOrder.Unlock();
		return result;
	}

private:

	DataType		data[capacity];	// GPU側と同じ構造の構造体情報
	CPU::SpinLock	memoryOrder;	// メモリオーダー操作
	size_t			head = 0;		// データの先頭
	size_t			tail = 0;		// 空き領域の先頭(使用領域の終端)

private:

	// コピー禁止
	ThreadSafeRingBuffer(const ThreadSafeRingBuffer&) = delete;
	ThreadSafeRingBuffer& operator= (const ThreadSafeRingBuffer&) = delete;

};
