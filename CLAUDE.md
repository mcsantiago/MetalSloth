# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

A macOS toy 3D renderer ("Sloth Engine") written in C++ against Apple's **metal-cpp** bindings, with GLFW for windowing, Dear ImGui for UI, ufbx for FBX model loading, and stb for image loading. It loads a rigged FBX model and renders it with Phong lighting and a free-fly camera.

## Build & run

The canonical workflow is terminal CMake (the `MetalTest.xcodeproj` is an alternative, but currently the CLI build is the maintained path):

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/MetalSlothEngine        # MUST run from the repo root
```

Run from the repo root because assets are loaded with **paths relative to the current working directory** (e.g. `assets/X Bot.fbx` in `SlothEngine::loadScene`).

There is no test suite, linter, or CI.

### Prerequisites
- **GLFW** via Homebrew (`brew install glfw`) — CMake finds it via `glfw3Config.cmake` under `/opt/homebrew` or `/usr/local`.
- **Metal Toolchain** — a separately downloaded Xcode component, required to compile `.metal` shaders. If `xcrun metal` reports "missing Metal Toolchain": `xcodebuild -downloadComponent MetalToolchain`. If `xcodebuild` itself fails to load a plugin: `sudo xcodebuild -runFirstLaunch`.

## Architecture

**Layering, top to bottom:**

- `SlothEngine` (`sloth_engine.cpp/.hpp`) — the top-level orchestrator. Owns the GLFW window, Metal device, ImGui setup, and the systems below. `init()` wires everything; `run()` is the main loop (per-frame: poll input → WASD/mouse camera movement → `T`/menu render-mode toggle → call `renderingSystem->run(camera, renderMode)`). Also owns **model loading** (`loadFbxFile` → `loadPolygons`, which triangulates ufbx meshes into a `VertexData` buffer and registers it as an entity).

- `ComponentManager` (`component_manager.cpp/.hpp`) — a minimal ECS keyed by `int entityId`. Components: `Transform`, `KineticPhysicalProperties`, `MeshInfo`, `Texture`. Registration is `register_*`; lookup is `get_*` returning `std::optional<T>` (callers check the optional before use). This is the single source of scene state.

- `MTLRenderingSystem` (`mtl_rendering_system.cpp/.hpp`) — all Metal setup (pipeline, command queue, MSAA + depth textures, render pass) and the per-frame draw. `run()` encodes transforms, draws all entities as instances, then draws the ImGui widgets. Render mode (`enum RenderMode { full, wireframe }`) is passed in **by reference** so both the `T` hotkey (in `SlothEngine::run`) and the **View > Mesh View** ImGui menu item toggle the same value.

- `PhysicsSystem` (`physics_engine.cpp/.hpp`) — exists but its per-frame update is currently commented out in the main loop.

**GPU side:**
- Shaders live in `src/shaders/*.metal`. The renderer calls `metalDevice->newDefaultLibrary()`, which loads `default.metallib`. The CMake build compiles **all** `src/shaders/*.metal` together into that one library and copies it **next to the executable** (where `newDefaultLibrary()` looks — the CLI "main bundle" resource path). Two consequences:
  - Function names must be **unique across all `.metal` files** (they share one library).
  - The pipeline looks up `vertexShader` / `fragmentShader` **by string name** (`createRenderPipeline`) and `assert`s on failure — renaming an entry point breaks the lookup silently at build but fatally at runtime.
- `vertex_data.h` holds the structs **shared between CPU and shader** (`VertexData`, `TransformationData`, `CameraData`, etc.) — both `#include` it, so layout must match on both sides. Buffer binding indices are by convention (vertex data = 0, transforms = 1, camera = 2).

**Native bridge:** metal-cpp has no windowing, so `glfw_bridge.mm` (Objective-C++) attaches the `CAMetalLayer` to the GLFW `NSWindow`. ImGui's Metal backend (`imgui_impl_metal.mm`) is the other Obj-C++ file.

## Conventions & gotchas

- **C++20 is required** (the code uses `std::unordered_map::contains`). The CMake build sets this.
- **ARC** is enabled **only** on the two ARC-based Obj-C++ files (`src/glfw_bridge.mm`, `external/imgui/imgui_impl_metal.mm`) via `set_source_files_properties` in CMake. metal-cpp uses **manual** `retain`/`release` (`alloc()->init()` ... `->release()`) — do not assume ARC elsewhere.
- CMake **globs** `src/*.cpp`, `src/*.mm`, `src/component_types/*.cpp` and `src/shaders/*.metal`, so new source/shader files are picked up automatically on reconfigure (`CONFIGURE_DEPENDS`).
- Include roots: `src`, `src/component_types`, `external` (so `imgui/...`, `ufbx/...`, `stb/...`, `AAPLMathUtilities.h`), and `metal-cpp` (so `<Metal/Metal.hpp>` etc.).
- Third-party code lives in `external/` and `metal-cpp/` (vendored). `assets/` holds models/textures loaded at runtime.
- The ImGui widgets each call `ImGui::NewFrame()`/`ImGui::Render()` as their own pair within a single frame (an unusual pattern — follow it when adding widgets). Font sizing is a single `io.FontGlobalScale = 2.0f` set at init, not per-window scaling.

## Branches

- `main` — the clean, buildable line.
- `raytracing-wip` — an in-progress Metal raytracing effort (a `Scene`/`Geometry` class hierarchy, acceleration-structure descriptors, `raytracer.metal`, and a `vertex_data.h` → `shader_types.h` rename). It is **incomplete and does not build** (stubbed `Scene`/`loadScene` bodies; `cube.metal` and `raytracer.metal` both define `vertexShader`/`fragmentShader`, which would collide). Pick this up rather than restarting if resuming raytracing work.
