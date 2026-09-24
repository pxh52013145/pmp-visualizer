# PMP Visualizer

PMP Visualizer is an open source native real-time visual workbench for Pixel Matrix Player. It is intended for production workflows: composable graphs, live GPU previews, timelines, parameter inspection, assets, plugins, and deterministic offline rendering.

The project is deliberately separate from the existing Tauri application. The host application owns audio playback, permissions, plugin discovery, configuration, and telemetry. This process owns the editor and renderer.

## Current status

The repository contains the initial Qt 6 workbench shell and a renderer backend boundary. The first Windows target is Direct3D 12. Vulkan and bgfx are reserved behind the same boundary, so the graph and UI model do not depend on one graphics API.

## Build

Requirements:

- CMake 3.25 or newer
- C++20 compiler
- Qt 6.6 or newer with the Widgets module
- Windows: Visual Studio 2022 and the Windows SDK

Configure the Windows prototype with:

```powershell
cmake -S . -B build-msvc -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_PREFIX_PATH=D:/DEV/QT/6.10.1/msvc2022_64 `
  -DPMP_RENDER_BACKEND=auto
cmake --build build-msvc --config Debug
```

`CMAKE_PREFIX_PATH` must point to the Qt kit that matches the compiler. The
MSVC generator uses `msvc2022_64`; a MinGW generator must use
`D:/DEV/QT/6.10.1/mingw_64` instead.

Other backend selections are explicit:

```powershell
cmake -S . -B build-vulkan -DPMP_RENDER_BACKEND=native-vulkan
cmake -S . -B build-bgfx -DPMP_RENDER_BACKEND=bgfx
```

The bgfx option is intentionally guarded until the dependency packaging and shader toolchain are finalized.

## Architecture direction

```text
Qt workbench
  - outliner
  - inspector
  - node graph
  - timeline
  - asset browser
          |
          v
Project / graph model  <->  undo/redo  <->  versioned project files
          |
          v
Renderer interface
  - native D3D12 (Windows first)
  - native Vulkan (cross-platform)
  - bgfx adapter (optional)
          |
          v
GPU viewport / offline renderer
```

## License

MIT. Third-party dependencies keep their own licenses.
