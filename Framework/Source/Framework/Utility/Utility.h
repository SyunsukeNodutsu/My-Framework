//-----------------------------------------------------------------------------
// File: Utility.h
//
// 便利機能を記述
// エイリアス 計算系 など
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// 短縮.エイリアス
//-----------------------------------------------------------------------------

// COM短縮
using Microsoft::WRL::ComPtr;

// OutputDebugStringA()の名前が長いので短縮
#define DebugLog( str ) OutputDebugStringA( str )

//-----------------------------------------------------------------------------
// 便利機能
//-----------------------------------------------------------------------------

// 円周率
constexpr float PI = 3.141592654f;

// 角度変換
constexpr float ToRadians = (PI / 180.0f);
constexpr float ToDegrees = (180.0f / PI);

// 安全にRelease
template<class T>
void SafeRelease(T*& p) { if (p) { p->Release(); p = nullptr; } }

// 安全にDelete
template<class T>
void SafeDelete(T*& p) { if (p) { delete p; p = nullptr; } }

//
constexpr float operator"" _deg(long double deg) { return static_cast<float>(deg) * (PI / 180.0f); }

//
constexpr float operator"" _rad(long double rad) { return static_cast<float>(rad) * (180.0f / PI); }

// @brief メディアファイルの位置を確認するためのヘルパー関数
// @param strDestPath
// @param cchDest
// @param strFilename ファイルを確認するパス(位置)
// @return 成功...S_OK 失敗...エラーメッセージ
HRESULT FindMediaFileCch(_Out_writes_(cchDest) WCHAR* strDestPath, _In_ int cchDest, _In_z_ LPCWSTR strFilename);

// @brief プレハブ指定の場合 mergeを試みる
// @param srcJson ソースとなるjsonデータ
// @param prefab プレハブを行うjsonの文字列
void MergePrefab(json11::Json& srcJson, const std::string& prefab = "Prefab");

// @brief std::string版 sprintf
template <typename ... Args>
std::string FormatBuffer(const std::string& fmt, Args ... args)
{
	size_t len = std::snprintf(nullptr, 0, fmt.c_str(), args ...);
	std::vector<char> buf(len + 1);
	std::snprintf(&buf[0], len + 1, fmt.c_str(), args ...);
	return std::string(&buf[0], &buf[0] + len);
}

// @brief ファイルパスから 親ディレクトリまでのパスを取得
inline std::string GetDirFromPath(const std::string& path)
{
	const std::string::size_type pos = std::max<signed>((const signed int)path.find_last_of('/'), (const signed int)path.find_last_of('\\'));
	return (pos == std::string::npos) ? std::string() : path.substr(0, pos + 1);
}

//-----------------------------------------------------------------------------
// プロセッサ
//-----------------------------------------------------------------------------

// CPU関連の情報を 取得 操作
// Microsoft: MultithreadedRendering11参考
namespace CPU
{
	// @brief 現在のシステムのプロセッサに関する情報を照会するためのヘルパー関数
	typedef BOOL(WINAPI* LPFN_GLPI)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);

	// @brief プロセッサーマスクのビット数を返す
	// @param bitMask カウントするプロセッサーマスク
	// @return プロセッサーマスクのビット数
	static DWORD CountBits(ULONG_PTR bitMask)
	{
		DWORD result = 0;
		DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
		ULONG_PTR bitTest = ULONG_PTR(1) << LSHIFT;

		for (DWORD i = 0; i <= LSHIFT; ++i) {
			result += ((bitMask & bitTest) ? 1 : 0);
			bitTest /= 2;
		}
		return result;
	}

	// @brief システム情報を返す
	// @return 現在のコンピューター システムに関する情報
	SYSTEM_INFO GetSystemInfo()
	{
		SYSTEM_INFO info = {};
		GetSystemInfo(&info);
		return info;
	}

	// @brief 処理に使用できる物理CPU/コアの数を返す
	// @return 使用可能な物理CPUの数
	static int GetPhysicalProcessorCount()
	{
		DWORD result = 0;// 失敗したら0を返す。 これで表示されます。

		// Windowsオペレーティングシステムにおいて"kernel32.dll"は
		// コアプロセスまたはオペレーティングシステムの心臓部を含む中心的なモジュールです
		// 起動時に"kernel32.dll"がメモリに読み込まれ、ユーザーがさまざまなタスクやプログラムを
		// 実行する際の操作を調整します。
		HMODULE hMod = GetModuleHandle(L"kernel32");
		assert(hMod);
		_Analysis_assume_(hMod);

		// 論理プロセッサと関連ハードウェアに関する情報を取得します。
		LPFN_GLPI Glpi = reinterpret_cast<LPFN_GLPI>(GetProcAddress(hMod, "GetLogicalProcessorInformation"));
		if (!Glpi) {
			assert(0 && "GetLogicalProcessorInformationはサポートされていません");
			return result;
		}

		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = nullptr;
		DWORD returnLength = 0;

		while (1)
		{
			// 正常
			if (Glpi(buffer, &returnLength) == TRUE)
				break;

			// 取得失敗
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				if (buffer)
					free(buffer);

				buffer = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(malloc(returnLength));

				if (!buffer)
					return result;// アロケーション失敗
			}
			else
				return result;// 予期せぬエラー
		}

		assert(buffer);
		_Analysis_assume_(buffer);

		DWORD byteOffset = 0;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = buffer;
		while (byteOffset < returnLength)
		{
			if (ptr->Relationship == RelationProcessorCore)
			{
				if (ptr->ProcessorCore.Flags)
				{
					// ハイパースレッディングまたはSMTが有効である。
					// 論理的なプロセッサが同じコアにある。
					result += 1;
				}
				else
				{
					// 論理的なプロセッサは異なるコア上にあります。
					result += CountBits(ptr->ProcessorMask);
				}
			}
			byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
			ptr++;
		}

		free(buffer);

		return result;
	}

	// メモリーオーダーの簡易操作
	// https://cpprefjp.github.io/reference/atomic/memory_order.html
	class SpinLock
	{
	public:

		// @brief 現在の状態をロック状態にする
		void Lock() {
			while (!TryLock()) {}
		}

		// @brief 値をアンロック状態にする
		void Unlock() {
			state.clear(std::memory_order_release);
		}

	private:

		std::atomic_flag state = ATOMIC_FLAG_INIT; // state表現 TAS

	private:

		// @brief ロック
		// @return 完了...true ロック中...false
		bool TryLock() { return !state.test_and_set(std::memory_order_acquire); }

	};

}
