#include "Actor.h"
#include "Human.h"
#include "Sky.h"
#include "StageMap.h"
#include "Tank.h"

#include "../ImGuiSystem.h"

//-----------------------------------------------------------------------------
// Actor生成
//-----------------------------------------------------------------------------
std::shared_ptr<Actor> GenerateActor(const std::string& name)
{
	std::shared_ptr<Actor> result = nullptr;

	// 名前から作成
	if (name == "Human")		result = std::make_shared<Human>();
	if (name == "Sky")			result = std::make_shared<Sky>();
	if (name == "StageMap")		result = std::make_shared<StageMap>();
	if (name == "Tank")			result = std::make_shared<Tank>();

	if (result)
		result->Awake();

	DebugLog(std::string("INFO: Generate actor. name: " + name).c_str());
	if (result == nullptr)
		DebugLog(std::string("WARNING: Failed to generate actor. name: " + name).c_str());

	return result;
}
