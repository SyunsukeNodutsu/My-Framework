//-----------------------------------------------------------------------------
// File: nzBuffer.h
//
// プリコンパイル済みヘッダー
// コンパイルに関してはVisualStdioで設定
//
// EditHistory:
//  2021/04/29 初回作成
//  2021/05/26 Windows Runtime Library追加
//-----------------------------------------------------------------------------
#pragma once

// Min Maxマクロの無効化：勝手にwindowsのMin Maxに置換されるのを防止
#define NOMINMAX

//--------------------------------------------------
//
// Windows
//
//--------------------------------------------------

#include <SDKDDKVer.h> // 最新のSDKを自動的に定義する
#include <windows.h>
#pragma comment(lib, "winmm.lib")// TODO: WindowInfoで必要っぽい？

// Windows Runtime Library(ComPtr)
#include <wrl.h>
#pragma comment(lib, "runtimeobject.lib")

//--------------------------------------------------
//
// C Liblary
//
//--------------------------------------------------
#include <cassert>

//--------------------------------------------------
//
// C++ Liblary
//
//--------------------------------------------------

// General
#include <chrono>
#include <memory>

// Stream IO
#include <filesystem>
#include <sstream>
#include <iostream>
#include <istream>

// Thread
#include <thread>
#include <mutex>

// Numerical processing
#include <random>

// STL
#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <array>
#include <string>
#include <algorithm>

//--------------------------------------------------
//
// 外部Library
//
//--------------------------------------------------

// 文字列変換 string converter
#include "../../Library/strconv.h"

// Lua
#include <lua.hpp>
#pragma comment(lib, "lua53.lib")

//--------------------------------------------------
//
// My Framework
//
//--------------------------------------------------
#include "Framework/Framework.h"
