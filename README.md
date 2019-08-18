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

### Why Vulkan?
It's simply down to cross-platform support. While DirectX is an awesome API, it only works on Microsoft platforms. DirectX can be added further down the line if it is eventually required.

### Licensing
[MIT](https://github.com/Syncaidius/FerrousEngine/blob/master/LICENSE)