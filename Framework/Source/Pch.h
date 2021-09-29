﻿//-----------------------------------------------------------------------------
// File: nzBuffer.h
//
// プリコンパイル済みヘッダー
// コンパイルに関してはVisualStdioで設定
//-----------------------------------------------------------------------------
#pragma once

//--------------------------------------------------
//
// Windows
//
//--------------------------------------------------

// Min Maxマクロの無効化：勝手にwindowsのMin Maxに置換されるのを防止
#define NOMINMAX

#include <SDKDDKVer.h> // 最新のSDKを自動的に定義する
#include <windows.h>
#pragma comment(lib, "winmm.lib")

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
#include <fstream>

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

// string converter (文字列変換)
#include "../../Library/strconv.h"

// imGui
#include <imgui.h>
#define IMGUI_DEFINE_PLACEMENT_NEW
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <imgui_glyph_ranges_jp.h> // 日本語対応

// json
#include <json.hpp>

// Effekseer
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

#pragma comment(lib, "Effekseer.lib")
#pragma comment(lib, "EffekseerRendererDX11.lib")

//--------------------------------------------------
//
// My Framework
//
//--------------------------------------------------
#include "Framework/Framework.h"
