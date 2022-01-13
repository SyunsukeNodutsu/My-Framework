//-----------------------------------------------------------------------------
// File: Component.h
//
// Component ... Actorにアタッチされる拡張部品
// 
// C++20で実装予定だったReflectionが見送りになったので
// 仕方なくstatic_pointer_castで取得
//-----------------------------------------------------------------------------
#pragma once

//==========================================================
// コンポーネント基底クラス
//==========================================================
class Component
{
public:

	// @brief コンストラクタ
	// @param owner 所持者の参照
	Component(Actor& owner)
		: m_owner(owner)
	{
	}

	// @brief 初期化
	virtual void Initialize() {}

	// @brief 終了
	virtual void Finalize() {}

	// @brief 更新
	// @param deltaTime 前フレームからの経過時間
	virtual void Update(float deltaTime) {}

	// @brief 描画後更新
	// @param deltaTime 前フレームからの経過時間
	virtual void LateUpdate(float deltaTime) {}

protected:

	// このコンポーネントの所持者
	Actor& m_owner;

};

//==========================================================
// コンポーネント管理クラス
//==========================================================
class ComponentLocator
{
public:

	// @brief コンポーネントの追加
	// @param component 追加するコンポーネント
	template<class type>
	static void AddComponent(std::shared_ptr<type> component)
	{
		if (component == nullptr) return;
		m_components[typeid(type).hash_code()] = component;
	}

	// @brief tpyeに一致するコンポーネントを返す
	// @reeturn 成功...typeに一致するコンポーネント, 失敗...nullptr
	template<class type>
	static std::shared_ptr<type> GetComponent()
	{
		// hash値比較の後 一致すればキャスト
		const auto& it = m_components.find(typeid(type).hash_code());
		if (it == m_components.end()) return nullptr;
		return std::static_pointer_cast<type>(m_components[typeid(type).hash_code()]);
	}

	// @brief tpyeに一致するコンポーネントを削除する
	template<class type>
	static void RemoveComponent()
	{
		const auto& it = m_components.find(typeid(type).hash_code());
		if (it == m_components.end()) return;
		m_components.erase(it);
	}

private:

	// コンポーネント一覧
	static inline std::unordered_map<
		size_t, std::shared_ptr<Component>> m_components;

};
