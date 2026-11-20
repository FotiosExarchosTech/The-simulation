cmake_minimum_required(VERSION 3.21)

project(simulation
    VERSION     0.1.0
    DESCRIPTION "The Simulation - a raylib sandbox"
    LANGUAGES   CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Single-config generators (Ninja, Make) start with no build type; pick a sane one.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

# Predictable output location: build/bin/simulation.exe
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Export compile_commands.json so clangd / the VS Code C++ extension see the real flags.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------------------
# raylib
#
# Default: build raylib from source, pinned to a tag. This produces a static
# library with raylib's own vendored GLFW, so simulation.exe is self-contained.
#
# -DSIM_USE_SYSTEM_RAYLIB=ON instead links the raylib already installed in the
# toolchain (MSYS2: pacman -S mingw-w64-x86_64-raylib). That configures in
# seconds, but links raylib and GLFW as DLLs, so mingw64/bin must stay on PATH.
# ---------------------------------------------------------------------------
set(SIM_RAYLIB_VERSION 5.5 CACHE STRING "raylib version to build/require")
option(SIM_USE_SYSTEM_RAYLIB "Link the toolchain's raylib instead of building it from source" OFF)

if(SIM_USE_SYSTEM_RAYLIB)
    find_package(raylib ${SIM_RAYLIB_VERSION} REQUIRED)
    message(STATUS "raylib ${SIM_RAYLIB_VERSION}: using toolchain install (shared)")
else()
    include(FetchContent)

    # raylib build knobs. Must be set before MakeAvailable.
    set(BUILD_EXAMPLES     OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS  OFF CACHE BOOL "" FORCE)
    set(CUSTOMIZE_BUILD    OFF CACHE BOOL "" FORCE)
    set(SUPPORT_MODULE_RMODELS ON CACHE BOOL "" FORCE)

    FetchContent_Declare(raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        ${SIM_RAYLIB_VERSION}
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
        SYSTEM
    )
    FetchContent_MakeAvailable(raylib)
    message(STATUS "raylib ${SIM_RAYLIB_VERSION}: built from source (static)")
endif()

# ---------------------------------------------------------------------------
# The executable
# ---------------------------------------------------------------------------
# Sources are globbed with CONFIGURE_DEPENDS: Ninja re-runs the glob when it
# checks the build, so a new .cpp under src/ is compiled without re-running
# cmake by hand. Convenient while the layout is still moving; if you ever want
# the stricter behaviour, replace this with an explicit list.
file(GLOB_RECURSE SIM_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")

if(NOT SIM_SOURCES)
    message(FATAL_ERROR "No .cpp files found under src/")
endif()

add_executable(simulation ${SIM_SOURCES})

# Lets headers be reached as "Headers/foo.h" from anywhere, alongside the
# relative "../Headers/foo.h" form the implementation files already use.
target_include_directories(simulation PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/Headers"
)

target_link_libraries(simulation PRIVATE raylib)

if(WIN32)
    # raylib's own target already pulls these in, but naming them is harmless
    # and keeps the system-raylib path (whose config file omits them) linkable.
    target_link_libraries(simulation PRIVATE opengl32 gdi32 winmm)
endif()

if(MSVC)
    target_compile_options(simulation PRIVATE /W4)
else()
    target_compile_options(simulation PRIVATE -Wall -Wextra -Wpedantic)
endif()

# With raylib built from source there is nothing left to resolve at runtime, so
# fold in the GCC runtimes too and the .exe becomes fully standalone. This also
# sidesteps Git-for-Windows' mingw64/bin, which sits earlier on PATH and ships
# its own libstdc++/libwinpthread. Skipped for the system-raylib path, which
# needs the dynamic loader for raylib and GLFW anyway.
if(MINGW AND NOT SIM_USE_SYSTEM_RAYLIB)
    target_link_options(simulation PRIVATE -static -static-libgcc -static-libstdc++)
endif()
