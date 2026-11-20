# The Simulation

A raylib sandbox: C++23, CMake, Ninja, MinGW-w64.

## Build and run

```sh
cmake --preset default      # first run clones + builds raylib 5.5 (~2 min)
cmake --build build
./build/bin/simulation.exe
```

After the first configure, rebuilds are incremental and take a second or two.
In VS Code: `Ctrl+Shift+B` builds, and the **run** task builds then launches.

Debug build (`-O0 -g`, in a separate tree so it never fights the release cache):

```sh
cmake --preset debug && cmake --build build/debug && ./build/debug/bin/simulation.exe
```

`F5` runs that under gdb, via `.vscode/launch.json`.

## Controls

Handled by `Creator_Movement` — a free-flight camera.

| Input                | Action                               |
| -------------------- | ------------------------------------ |
| `W` `A` `S` `D`      | move along the view / strafe         |
| mouse                | look                                 |
| `Space` / `Ctrl`     | rise / descend along world up        |
| `Shift`              | boost (`boost_multiplier`, 4x)       |
| `Tab`                | release or re-capture the cursor     |
| left click           | re-capture the cursor once released  |
| `Esc`                | quit                                 |

## Layout

```
CMakeLists.txt
CMakePresets.json
src/
  main.cpp                          window, camera, loop, placeholder scene
  Headers/creator_movement.h
  Implementations/creator_movement.cpp
build/                              generated, git-ignored
```

Sources are picked up by a `GLOB_RECURSE ... CONFIGURE_DEPENDS` over `src/*.cpp`,
so **a new `.cpp` anywhere under `src/` is compiled with no CMake edit** — Ninja
re-runs the glob before each build. Headers can be included either as
`"../Headers/foo.h"` or `"Headers/foo.h"`; both `src/` and `src/Headers/` are on
the include path.

The spheres and grid in `main.cpp` are scaffolding so there is something to fly
around. The camera wiring and the frame loop are the parts worth keeping.

## How raylib is wired up

By default CMake clones raylib (pinned to tag `5.5` via `SIM_RAYLIB_VERSION`)
and builds it as a **static** library with its own vendored GLFW. Together with
`-static`, `simulation.exe` links only Windows system DLLs — no raylib, GLFW,
libstdc++ or libwinpthread to ship. Copy the 6 MB exe anywhere and it runs.

The toolchain also has raylib 5.5 from `pacman -S mingw-w64-x86_64-raylib`. To
link that and skip the source build:

```sh
cmake --preset system-raylib && cmake --build build/system
```

That configures in seconds, but MSYS2's raylib pulls in raylib and GLFW as DLLs,
so `C:\msys64\mingw64\bin` must stay on `PATH` when the binary runs. It is on
PATH today, so both presets work — the from-source default is simply the one
that stays portable and version-pinned.

To move raylib versions, change `SIM_RAYLIB_VERSION` in `CMakeLists.txt` and
reconfigure.

## Docs

- Cheatsheet: <https://www.raylib.com/cheatsheet/cheatsheet.html>
- Examples: <https://www.raylib.com/examples.html>
- The vendored headers in `build/_deps/raylib-src/src/` (`raylib.h`, `raymath.h`, `rlgl.h`) are the real reference.
