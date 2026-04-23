# ---------------------------------------------------------------------------
# HKBuild-Template
# SKSE plugin + HKBuild behavior skeleton — C++23, CommonLibSSE-NG
#
# ---- QUICK START ------------------------------------------------------------
#
# 1. Search-and-replace "MyPlugin" with your plugin name (no spaces).
#    Also update AUTHOR_NAME and MOD_FOLDER_NAME in behavior_src/CMakeLists.txt.
#
# 2. Set environment variables (once, system-wide or in your IDE):
#      COMMONLIB_SSE_FOLDER  — path to a local CommonLibSSE-NG clone
#      VCPKG_ROOT            — path to your vcpkg installation
#      SKYRIM_MODS_FOLDER    — path to your MO2 mods/ folder (for auto-deploy)
#
# 3. Configure and build:
#      cmake --preset release
#      cmake --build build --config Release
#
#    Or open the folder in Visual Studio / Rider; CMakePresets.json is detected
#    automatically.
#
# 4. Behavior compilation (optional):
#    Behaviors only compile when HKBuild is available. Run from the HKBuild
#    workspace root, or set HKBUILD_EXE to your HKBuild executable path.
#
# ---- DEPENDENCY SETUP -------------------------------------------------------
#
# CommonLibSSE-NG:
#   git clone https://github.com/CharmedBaryon/CommonLibSSE-NG.git
#   set COMMONLIB_SSE_FOLDER=<path to clone>
#
# vcpkg:
#   git clone https://github.com/microsoft/vcpkg.git
#   .\vcpkg\bootstrap-vcpkg.bat
#   set VCPKG_ROOT=<path to vcpkg>
# ---------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.21)

set(AUTHOR_NAME  "YourName")        # TODO: replace
set(PRODUCT_NAME "MyPlugin")        # TODO: replace (no spaces, used as .dll name)

project(${PRODUCT_NAME} VERSION 0.1.0.0 LANGUAGES CXX)

# ---------------------------------------------------------------------------
# C++ standard
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

add_compile_definitions(_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)

# ---------------------------------------------------------------------------
# Module path
# ---------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ---------------------------------------------------------------------------
# CommonLibSSE-NG
# ---------------------------------------------------------------------------
set(SKYRIM_SUPPORT_AE ON CACHE BOOL "" FORCE)
set(BUILD_TESTS       OFF CACHE BOOL "" FORCE)

if(DEFINED ENV{COMMONLIB_SSE_FOLDER})
    set(COMMONLIB_PATH "$ENV{COMMONLIB_SSE_FOLDER}")
else()
    set(COMMONLIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/CommonLibSSE")
    if(NOT EXISTS "${COMMONLIB_PATH}/CMakeLists.txt")
        message(FATAL_ERROR
            "CommonLibSSE not found.\n"
            "Set the COMMONLIB_SSE_FOLDER environment variable to a local clone of\n"
            "https://github.com/CharmedBaryon/CommonLibSSE-NG, or run:\n"
            "  git submodule update --init --recursive")
    endif()
endif()

add_subdirectory("${COMMONLIB_PATH}"
    "${CMAKE_CURRENT_BINARY_DIR}/_deps/clib-build" EXCLUDE_FROM_ALL)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CommonLibSSE.cmake")

# ---------------------------------------------------------------------------
# vcpkg packages
# Add or remove packages in vcpkg.json as needed.
# ---------------------------------------------------------------------------
find_package(spdlog CONFIG REQUIRED)

# ---------------------------------------------------------------------------
# Include paths
# ---------------------------------------------------------------------------
# Workspace builds receive SHARED_INCLUDE_DIR from the root CMakeLists.
# Standalone builds fall back to this repo's own include/ directory.
if(NOT DEFINED SHARED_INCLUDE_DIR)
    set(SHARED_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
endif()

# ---------------------------------------------------------------------------
# Version resource
# ---------------------------------------------------------------------------
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.rc"
    @ONLY)

set(RESOURCE_FILE   "${CMAKE_CURRENT_BINARY_DIR}/version.rc")
set(RESOURCE_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/version.res")

add_custom_command(
    OUTPUT  ${RESOURCE_OUTPUT}
    COMMAND rc /fo ${RESOURCE_OUTPUT} ${RESOURCE_FILE}
    DEPENDS ${RESOURCE_FILE}
)
add_custom_target(MyPluginResource ALL DEPENDS ${RESOURCE_OUTPUT})

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
file(GLOB_RECURSE PLUGIN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/*.cpp")
list(FILTER PLUGIN_SOURCES EXCLUDE REGEX ".*Plugin\\.cpp$")

# ---------------------------------------------------------------------------
# Plugin target
# ---------------------------------------------------------------------------
add_commonlibsse_plugin(${PROJECT_NAME}
    AUTHOR  ${AUTHOR_NAME}
    SOURCES ${PLUGIN_SOURCES} ${RESOURCE_OUTPUT}
)

target_sources(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/Plugin.cpp"
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        PLUGIN_LOG_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "${SHARED_INCLUDE_DIR}/PCH.h"
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${SHARED_INCLUDE_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        spdlog::spdlog
)

# ---------------------------------------------------------------------------
# Deployment (copies .dll to Skyrim mod folder on build)
# ---------------------------------------------------------------------------
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/automaticGameFolderOutput.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/copyOutputs.cmake")

automaticGameFolderOutput()

# ---------------------------------------------------------------------------
# Behavior source compilation (YAML -> HKX via HKBuild)
# Only runs when HKBuild is available (workspace or HKBUILD_EXE is set).
# ---------------------------------------------------------------------------
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/behavior_src/CMakeLists.txt")
    if(TARGET HKBuild OR DEFINED HKBUILD_EXE)
        add_subdirectory(behavior_src)
        message(STATUS "${PRODUCT_NAME}: behavior compilation enabled (HKBUILD_EXE=${HKBUILD_EXE})")
    else()
        message(STATUS "${PRODUCT_NAME}: HKBuild not found - behavior compilation skipped.")
        message(STATUS "  Build from the HKBuild workspace root, or set HKBUILD_EXE, to enable.")
    endif()
endif()
