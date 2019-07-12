# Ferrous Engine
The project is still in its early days, but here are a few things that have been decided on:  
 * Fast and consistent memory allocation via custom allocators
 * [Vulkan](https://www.khronos.org/vulkan/) from the ground up
 * [GLM](https://github.com/g-truc/glm) - GL Mathematics to provide most of the common math types
 * [OpenAL Soft](https://github.com/kcat/openal-soft) for audio. An open-source implementation of OpenAL.  
 * [PhysX 4.x](https://github.com/NVIDIAGameWorks/PhysX) for physics - open-source, finally!
 * [AssImp](http://www.assimp.org/) for model loading
 * [Lua](https://www.lua.org/) for scripting
 * [RapidJSON](http://rapidjson.org/) for serialization
 * [FreeType](https://www.freetype.org/) for font processing. Repo located [here](https://git.savannah.gnu.org/cgit/freetype/)

### Why DirectX 12? 
The main reason is that I'm very familiar with the DirectX 11 API. Much of the DX12 API is very similar to DX11, which means I can hit the ground running sooner than if I start out with OpenGL or Vulkan. It will also provide a chance to figure out what (and how much) abstraction will be needed to allow more than one graphics API within the engine.

### Why Vulkan?
Three words: Cross-platform support. While DirectX is an awesome API, it only works on Microsoft platforms.  

### Licensing
[MIT](https://github.com/Syncaidius/FerrousEngine/blob/master/LICENSE)