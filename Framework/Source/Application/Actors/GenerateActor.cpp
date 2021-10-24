#include "../main.h"

#include "Environment/Sky.h"
#include "Environment/StageMap.h"
#include "Environment/Tree.h"

#include "Weapon/Tank.h"

#include "Process/GameProcess.h"
#include "Process/TitleProcess.h"

#include "Human.h"

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
	if (name == "Tree")			result = std::make_shared<Tree>();

	if (name == "GameProcess")	result = std::make_shared<GameProcess>();
	if (name == "TitleProcess") result = std::make_shared<TitleProcess>();

	// TODO: nullActor作成
	if (result)
		result->Awake();

	APP.g_imGuiSystem->AddLog(
		result
		? std::string("INFO: Generate actor. name: " + name).c_str()
		: std::string("WARNING: Failed to generate actor. name: " + name).c_str()
	);

	return result;
}
