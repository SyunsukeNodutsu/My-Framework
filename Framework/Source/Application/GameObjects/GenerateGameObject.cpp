#include "GameObject.h"
#include "Sky.h"
#include "StageMap.h"

//-----------------------------------------------------------------------------
// GameObject生成
//-----------------------------------------------------------------------------
std::shared_ptr<GameObject> GenerateGameObject(const std::string& name)
{
	if (name.empty()) {
		assert(0 && "エラー：nameが不正です.");
		return nullptr;
	}

	std::shared_ptr<GameObject> result = nullptr;

	// 名前から作成
	/*if (name == "Human")		result = std::make_shared<Human>();
	if (name == "MainCamera")	result = std::make_shared<MainCamera>();
	if (name == "SailingShip")	result = std::make_shared<SailingShip>();*/
	//if (name == "Sky")			result = std::make_shared<Sky>();
	if (name == "StageMap")		result = std::make_shared<StageMap>();
	/*
	if (name == "Tank")			result = std::make_shared<Tank>();
	if (name == "Tree")			result = std::make_shared<Tree>();
	if (name == "Watchtower")	result = std::make_shared<Watchtower>();*/

	if (result)
		result->Awake();
	else
		assert(0 && "エラー：GameObject生成失敗.");

	return result;
}
