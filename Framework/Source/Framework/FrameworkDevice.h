//-----------------------------------------------------------------------------
// File: EffekseerDevice.h
//
// フレームワークの各デバイスの基底クラス
// 各デバイスはアプリケーションに存在するデバイス間でアクセスを行いやすく
//-----------------------------------------------------------------------------
#pragma once
#include "../Application/main.h"

//フレームワークのデバイス基底クラス
class FrameworkDevice
{
public:

	//@brief コンストラクタ
	FrameworkDevice(Application& _operator)
		: g_operator(_operator)
	{
	}

	//操作するクラス ※各デバイスはアプリケーションに存在する
	Application& g_operator;

};
