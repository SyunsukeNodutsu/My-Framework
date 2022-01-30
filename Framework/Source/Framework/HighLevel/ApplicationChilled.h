//-----------------------------------------------------------------------------
// File: ApplicationChilled.h
//
// 継承先に単一のアプリケーションへの参照を許可します
// TODO: main.hに定義しているApplicationクラスをフレームワーク側に移動
//-----------------------------------------------------------------------------
#pragma once
#include "../../Application/main.h"

class ApplicationChilled
{
public:

    //@brief アプリケーションの設定
    //@param application 設定するソース
    static void SetApplication(Application* application);

    //@brief アプリケーションを返す
    //@return アプリケーション
    static Application* GetApplication() { return g_application; }

protected:

    //単一のデバイスを参照
    static Application* g_application;
};
