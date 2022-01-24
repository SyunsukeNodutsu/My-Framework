//-----------------------------------------------------------------------------
// File: EffekseerDevice.h
//
// フレームワークの各デバイスの基底クラス
// 各デバイスはアプリケーションに存在するデバイス間でアクセスを行いやすく
// 
// ロケーターで管理
//-----------------------------------------------------------------------------
#pragma once
#include "../Application/main.h"

//==========================================================
//フレームワークのデバイス基底クラス
//==========================================================
class FrameworkDevice
{
public:

	//@brief コンストラクタ
	FrameworkDevice(Application& _operator)
		: g_operator(_operator)
	{
	}

	//@brief 初期化
	virtual void Initialize() {}

	//@brief 終了
	virtual void Finalize() {}

private:

	//操作するクラス ※各デバイスはアプリケーションに存在する
	Application& g_operator;

};



//==========================================================
//デバイス管理クラス
//==========================================================
class FrameworkDeviceLocator
{
public:

	//@brief デバイスの追加
	//@param component 追加するデバイス
	template<class type>
	static void AddDevice(std::shared_ptr<type> device)
	{
		if (device == nullptr) return;
		m_devices[typeid(type).hash_code()] = device;
	}

	//@brief tpyeに一致するデバイスを返す
	//@reeturn 成功...typeに一致するデバイス, 失敗...nullptr
	template<class type>
	static std::shared_ptr<type> GetDevice()
	{
		//hash値比較の後 一致すればキャスト
		const auto& it = m_devices.find(typeid(type).hash_code());
		if (it == m_devices.end()) return nullptr;
		return std::static_pointer_cast<type>(m_devices[typeid(type).hash_code()]);

	//@brief tpyeに一致するデバイスを削除する
	template<class type>
	static void RemoveDevice()
	{
		const auto& it = m_devices.find(typeid(type).hash_code());
		if (it == m_devices.end()) return;
		m_devices.erase(it);
	}

private:

	//デバイス一覧
	static inline std::unordered_map< size_t, std::shared_ptr<FrameworkDevice> > m_devices;

};
