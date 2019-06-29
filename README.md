# Ferrous Engine
The project is still in it's early days, but here are a few things I would like to see in this engine:  
 * Fast and consistent memory allocation via custom allocators
 * DirectX 12 and Vulkan from the ground up - SPIR-V for shared shader code between APIs
 * [OpenAL Soft](https://github.com/kcat/openal-soft) for audio. An open-source implementation of OpenAL.  
 * [PhysX 4.x](https://github.com/NVIDIAGameWorks/PhysX) for physics - Surprise, it's open source!
 * [AssImp](http://www.assimp.org/) for model loading
 * [Lua](https://www.lua.org/) for scripting
 * [RapidJSON](http://rapidjson.org/) for serialization
 * [FreeType](https://www.freetype.org/) for font processing. Repo located [here](https://git.savannah.gnu.org/cgit/freetype/)

### Why DirectX 12? 
The main reason is that I'm very familiar with the DirectX 11 API. Much of the DX12 API is very similar to DX11, which means I can hit the ground running sooner than if I start out with OpenGL or Vulkan. It will also provide a chance to figure out what (and how much) abstraction will be needed to allow more than one graphics API within the engine.

### Why Vulkan?
Three words: Cross-platform support. While DirectX is an awesome API, it only works on Microsoft platforms.  

### Licensing
[MIT](https://github.com/Syncaidius/MoltenEngine/blob/master/LICENSE)