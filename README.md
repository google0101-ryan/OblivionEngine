# OBLIVION

Oblivion is a game engine that aims to offer a fast and modern user experience, while still being simple and open-source

## BUILDING

For now, building Oblivion is a Linux-only endevour.
First, ensure you have GCC, SDL3, and Vulkan properly installed. Then, simply run from within the repo:
```sh
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=prefix ..
make -j8 install
```

where prefix is the folder you want the engine installed to.

## Technical

Building Oblivion produces several distinct modules. The engine is stratified into layers, with only lower-numbered layers available to higher layers, e.g. `layer1` can only rely on `layer0`. Each layer is implemented in one or more shared libraries (`.so` files on Linux). These shared libraries are loaded by `libengine.so` at runtime. 

While currently unimplemented, it is planned to allow users to write C++ code directly, implemented in a user-defined game library (`libgame_clinet.so` and `libgame_server.so`) which be loaded by the engine. Basic libraries for things such as AI will be provided as seperate optional libraries built on Oblivion in the future to ease game development.

Scripting is planned to be supported to some level, but the brunt of that will be on game developers as scripting is too game-specific. Again, support libraries will most likely be made available in the future.

## Short-term roadmap
- [x] Add CMake build system
- [x] Add layer0 (basic cstdlib replacements)
- [x] Add layer1 (Command and event management)
- [ ] Add layer2 (Window, renderer, resource manager, basic ECS)
    - [ ] Add Vulkan Support
    - [ ] Add OpenGL Support
    - [ ] **UNLIKELY**: Add Metal Support (no Mac to develop on)
- [ ] Implement ECS components (renderable, sound event, etc, etc)
## Long-term roadmap
- [ ] Add support for user-defined client/server DLLs
- [ ] Create standalone libraries for Oblivion game development
- [ ] Add editor/other support systems to ease development
- [ ] Extensive documentation of engine interfaces and systems