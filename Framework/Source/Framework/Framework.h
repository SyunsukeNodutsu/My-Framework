//-----------------------------------------------------------------------------
// File: Framework.h
//
// フレームワーク リンク
//-----------------------------------------------------------------------------
#pragma once

// DirectX11
#include <dxgi.h>
#include <d3d11.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// X2Audio X3DAudio
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>

#pragma comment(lib,"xaudio2.lib")

// 算術
#include <DirectXMath.h>

// DirectX ToolKit
#pragma comment(lib, "DirectXTK.lib")
#include "DDSTextureLoader.h" // DDS形式(圧縮状態で扱い可能)
#include "WICTextureLoader.h" // BMP PNG とか
#include <SimpleMath.h>

// Effekseer

//--------------------------------------------------
// 自作Framework
//--------------------------------------------------

// エイリアスが含まれているので最優先
#include "Utility/Types.h"
#include "Utility/Utility.h"

#include "Utility/FpsTimer.h"

#include "Window/Window.h"

#include "Graphics/Device.h"
#include "Graphics/Texture.h"
#include "Graphics/Buffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Camera.h"

#include "Model/Mesh.h"
#include "Model/Model.h"
#include "Model/Animation.h"

#include "Input/RawInput.h"

#include "Scripting/Lua.h"

#include "Audio/AudioDevice.h"
#include "Audio/AudioDeviceChild.h"
#include "Audio/Sound.h"
#include "Audio/Sound3D.h"

//--------------------------------------------------
// Shader
//--------------------------------------------------
#include "Shader/ShaderManager.h"
