#include "../main.h"

#include "Environment/Sky.h"
#include "Environment/StageMap.h"
#include "Environment/Tree.h"

#include "Process/GameProcess.h"
#include "Process/TitleProcess.h"

#include "Human.h"
#include "CesiumMan.h"

//-----------------------------------------------------------------------------
// Actor生成
//-----------------------------------------------------------------------------
std::shared_ptr<Actor> GenerateActor(const std::string& name)
{
	std::shared_ptr<Actor> result = nullptr;

	// 名前から作成
	if (name == "Actor")		result = std::make_shared<Actor>();

	if (name == "Human")		result = std::make_shared<Human>();
	if (name == "Sky")			result = std::make_shared<Sky>();
	if (name == "StageMap")		result = std::make_shared<StageMap>();
	if (name == "Tree")			result = std::make_shared<Tree>();
	if (name == "CesiumMan")	result = std::make_shared<CesiumMan>();

	if (name == "GameProcess")	result = std::make_shared<GameProcess>();
	if (name == "TitleProcess") result = std::make_shared<TitleProcess>();

	// null actorでもいいかも
	if (result) result->Awake();

	ApplicationChilled::GetApplication()->g_imGuiSystem->AddLog(
		result
		? "INFO: Generate actor. name: " + name
		: "WARNING: Failed to generate actor. name: " + name
	);

	return result;
}
