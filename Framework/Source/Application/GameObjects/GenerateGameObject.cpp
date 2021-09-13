#include "GameObject.h"
#include "Human.h"
#include "Sky.h"
#include "StageMap.h"
#include "Tank.h"

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
	if (name == "Human")		result = std::make_shared<Human>();
	if (name == "Sky")			result = std::make_shared<Sky>();
	if (name == "StageMap")		result = std::make_shared<StageMap>();
	if (name == "Tank")			result = std::make_shared<Tank>();

	if (result)
		result->Awake();

	return result;
}
