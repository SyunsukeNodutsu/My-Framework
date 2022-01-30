#include "ApplicationChilled.h"

Application* ApplicationChilled::g_application = nullptr;

//-----------------------------------------------------------------------------
// アプリケーションの設定
//-----------------------------------------------------------------------------
void ApplicationChilled::SetApplication(Application* application)
{
	if (application == nullptr) return;
	g_application = application;
}
