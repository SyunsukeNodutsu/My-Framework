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

// DirectInput
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")

// 算術
#include <DirectXMath.h>

// DirectX ToolKit
#include <SimpleMath.h>

#pragma comment(lib, "DirectXTK.lib")

// DirectXTex
#include <DirectXTex.h>

#pragma comment(lib,"DirectXTex.lib")

//--------------------------------------------------
// 自作Framework
//--------------------------------------------------

// エイリアスが含まれているので最優先
// TODO: Windo 各Deice はまとめた方がいいかも
#include "Utility/Types.h"
#include "Utility/Utility.h"

#include "Utility/FpsTimer.h"

#include "Utility/CoroutineGeneration.h"

#include "Window/Window.h"

#include "Graphics/Buffer.h"

#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsDeviceChild.h"

#include "Graphics/Texture.h"
#include "Graphics/Renderer.h"
#include "Graphics/CubeMapGenerator.h"

#include "Graphics/Camera/Camera.h"

#include "Graphics/Effect/EffekseerDevice.h"
#include "Graphics/Effect/EffekseerDeviceChild.h"
#include "Graphics/Effect/EffekseerEffect.h"

#include "Model/Mesh.h"
#include "Model/Model.h"
#include "Model/Animation.h"

#include "Physics/Collision.h"
#include "Physics/SpatialPartitioning.h"

#include "Input/RawInputDevice.h"
#include "Input/DirectInput/DirectInputDevice.h"

#include "Shader/ShaderManager.h"

#include "Audio/AudioDevice.h"
#include "Audio/AudioDeviceChild.h"
#include "Audio/Sound.h"
#include "Audio/Sound3D.h"

#include "HighLevel/ResourceFactory.h"
#include "HighLevel/Actor.h"
