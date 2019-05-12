# Ferrous Engine
Since the project has just been started, there is a shortage of details right now. However, I can list a few things I need/want in this engine:  
 * DirectX 12 from the ground up (and Vulkan later down the line)  - SPIR-V for shared shader code between APIs
 * XAudio2 and/or [OpenAL](https://www.openal.org/) audio, with abstraction to switch between them or another audio system (e.g. [FMOD](https://www.fmod.com/))  
 * [Bullet](https://pybullet.org/wordpress/) physics - open-source, free, cross-platform, vendor-agnostic, nice right?  
 * [AssImp](http://www.assimp.org/) for model loading  
 * [Lua](https://www.lua.org/) for scripting

You may be asking, why DirectX 12? 
Well, it's adoption rate is finally picking up speed. By the time the engine is usable in a game, DirectX12 should be much more commonplace. The same goes for Vulkan. If not, falling back to DX11/OpenGL is always an option later down the line.

# Licensing
[MIT](https://github.com/Syncaidius/MoltenEngine/blob/master/LICENSE)