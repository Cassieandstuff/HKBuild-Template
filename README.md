# HKBuild Plugin Template

A starter template for SKSE plugins that compile Skyrim behavior graphs from YAML source files using [HKBuild](https://github.com/Cassieandstuff/HKBuild).

---

## What's in this template?

| Directory | Contents |
|---|---|
| `behavior_src/` | HKBuild YAML source tree — compile to HKX behavior files |
| `src/cpp/` | SKSE plugin C++ entry point |
| `cmake/` | CMake helper scripts (CommonLibSSE, auto-deploy, version resource) |
| `include/` | Shared headers (PCH, PluginLogger) |

---

## Prerequisites

| Tool | Where to get it |
|---|---|
| **Visual Studio 2022** | [visualstudio.microsoft.com](https://visualstudio.microsoft.com/) — install the *Desktop development with C++* workload |
| **CMake 3.21+** | [cmake.org](https://cmake.org/download/) |
| **vcpkg** | `git clone https://github.com/microsoft/vcpkg && .\vcpkg\bootstrap-vcpkg.bat` |
| **CommonLibSSE-NG** | `git clone https://github.com/CharmedBaryon/CommonLibSSE-NG` |
| **HKBuild** *(optional)* | Required only to compile `.yaml` behaviors to `.hkx`. Not needed for SKSE-only builds. |

---

## Setup

### 1. Clone this repository

```
git clone https://github.com/Cassieandstuff/HKBuild-Template.git MyPlugin
cd MyPlugin
```

### 2. Set environment variables

Set these once in your system environment or IDE:

```
COMMONLIB_SSE_FOLDER = C:\path\to\CommonLibSSE-NG
VCPKG_ROOT           = C:\path\to\vcpkg
SKYRIM_MODS_FOLDER   = C:\path\to\ModOrganizer2\mods   # optional: auto-deploy
```

### 3. Rename things

Search and replace `MyPlugin` throughout the project with your plugin name (no spaces).

Also update:
- `AUTHOR_NAME` in `CMakeLists.txt`
- `MOD_FOLDER_NAME` in `behavior_src/CMakeLists.txt`
- `"name"` in `vcpkg.json` (lowercase, hyphens OK)

### 4. Build (SKSE plugin only)

```
cmake --preset release
cmake --build build --config Release
```

The built `.dll` lands in `build/Release/`. If `SKYRIM_MODS_FOLDER` is set, it is also copied to `mods/<PRODUCT_NAME>/SKSE/Plugins/` automatically.

### 5. Build with behavior compilation

Behavior compilation requires HKBuild. Build from the HKBuild workspace root (all workspace plugins build together), or pass the path to HKBuild's executable directly:

```
cmake --preset release -DHKBUILD_EXE="C:\path\to\HKBuild.exe"
cmake --build build --config Release
```

Compiled `.hkx` files land in the build directory and are deployed to `SKYRIM_MODS_FOLDER` automatically if set.

---

## Behavior source structure

HKBuild reads a tree of YAML files organized by behavior graph:

```
behavior_src/
  character/                          # actor type (character, dragon, etc.)
    behaviors/
      MyPlugin.hkx/                   # folder name = output filename
        behavior.yaml                 # entry point — defines graph name and root
        data/
          graphdata.yaml              # variables and events
        states/
          MyRootSM.yaml               # root state machine
          ST_MyIdle.yaml              # individual states
        clips/
          CLIP_MyIdle.yaml            # clip generators
```

Each `.hkx` folder compiles to a single binary HKX file.

### Adding a new behavior

1. Create a new `<Name>.hkx/` folder under the appropriate actor type directory.
2. Add `behavior.yaml` as the entry point.
3. Add `data/graphdata.yaml` for variables and events.
4. Add state machine and clip YAML files in `states/` and `clips/`.
5. Rebuild.

### YAML reference

| Class | Purpose |
|---|---|
| `hkbStateMachine` | State machine — routes between states via events |
| `hkbStateMachineStateInfo` | Individual state — wraps a generator |
| `hkbClipGenerator` | Plays an animation clip |
| `hkbBlenderGenerator` | Blends between two generators |
| `hkbBlendingTransitionEffect` | Crossfade between states |
| `BSiStateManagerModifier` | Reads/writes graph variables |

See the HKBuild documentation and the example files in this template for syntax.

---

## Project structure conventions

```
MyPlugin/
  behavior_src/     YAML behaviors (compiled by HKBuild)
  cmake/            CMake helper modules — do not edit unless you know what you're doing
  include/          Shared headers (PCH.h, PluginLogger.h)
  src/
    cpp/            C++ source files
      Plugin.cpp    SKSE entry point (SKSEPluginLoad)
  CMakeLists.txt    Main build script
  CMakePresets.json Configure/build presets
  vcpkg.json        vcpkg dependency manifest
```

---

## Adding C++ source files

Add `.cpp` files to `src/cpp/`. CMakeLists.txt picks them up automatically via `file(GLOB_RECURSE ...)`. Plugin.cpp is excluded from the glob and added explicitly so it links last.

## Adding vcpkg dependencies

Add package names to the `dependencies` array in `vcpkg.json`, then add a matching `find_package(...)` and `target_link_libraries(...)` call in `CMakeLists.txt`.

Common packages for SKSE plugins:
- `spdlog` — logging (already included)
- `xbyak` — runtime code patching
- `ryml` — fast YAML parsing (add if your plugin reads YAML at runtime)

---

## License

See [LICENSE.md](LICENSE.md).
