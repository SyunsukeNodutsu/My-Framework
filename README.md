# My-Framework
暇つぶし自作フレームワーク
DirectX11/C++/HLSL

// ■フレームワーク全体像(2022/02/04)
// 
// Application(main.h)------AudioDevice
//   |    |  |                   |
// Window |  |             AudioDeviceChild
//		  |  |                |        |
//		  |  ┗-----┓  SoundDirector Sound
//		  |         |                  |
//	 InputDevice    |                Sound3D
//	  |       |     |
// Keyboard   Mouse |
//					|
//			  GraphicsDevice                         Model---Animation
//					|                                |   |
//			GraphicsDeviceChild---------------------Mesh |
//			 |      |       |  ┗-------┓               |
//		   Buffer Texture Shader   |  EffectDevice   Material
//			 |              |      |         |
//	   ConstantBuffer -- 各Shader  |   EffectDeviceChild
//			 |                     |
//			 ┗------------------Renderer
//
// ■使用ライブラリ
// 外部 ---
// DirectXTK
// DirectXTex
// Dear imGui
// Effekseer(1.62a)
// 学内配布 ---
// json
// gltfLoader
