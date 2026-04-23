# ---------------------------------------------------------------------------
# behavior_src/CMakeLists.txt
#
# Compiles all .hkx YAML folders in behavior_src/character/ into binary HKX
# files using HKBuild, then deploys them to your MO2 mod folder.
#
# ---- GETTING STARTED --------------------------------------------------------
#
# 1. Rename "MyPlugin.hkx" to match your behavior file (e.g. MyMod.hkx).
#    The folder name (without .hkx) becomes the output filename.
#
# 2. Edit BEHAVIOR_TARGET and MOD_FOLDER_NAME below.
#
# 3. Add YAML files under character/behaviors/<YourBehavior>.hkx/:
#      behavior.yaml   -- entry point (required, defines behavior name / root)
#      data/           -- graphdata.yaml (variables + events)
#      states/         -- hkbStateMachine YAML files
#      clips/          -- hkbClipGenerator YAML files
#
# 4. For non-character actors (dragon, horse, etc.) change CREATURE_DIR below.
#
# ---- HOW IT WORKS -----------------------------------------------------------
#
# HKBuild reads a .hkx folder tree and:
#   Step 1: compiles YAML -> XML  (HKBuild <folder> -o <name>.xml)
#   Step 2: packs  XML  -> HKX   (HKBuild pack <name>.xml -o <name>.hkx)
#
# The resulting HKX files are deployed to:
#   $SKYRIM_MODS_FOLDER/<MOD_FOLDER_NAME>/meshes/actors/character/behaviors/
#
# This target only builds when HKBuild is available (workspace build or
# HKBUILD_EXE is set explicitly). Standalone SKSE-only builds skip it silently.
# ---------------------------------------------------------------------------

set(BEHAVIOR_TARGET "MyPluginBehaviors")   # TODO: rename
set(MOD_FOLDER_NAME "My Plugin")           # TODO: rename (matches MO2 mod folder name)

# ---------------------------------------------------------------------------
# Boilerplate — discovers all .hkx folders and wires custom commands
# ---------------------------------------------------------------------------

# The creature-type root mirrors the vanilla actor layout:
#   character/
#     behaviors/     -> meshes/actors/character/behaviors/
set(CREATURE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/character")
set(CREATURE_TYPE "character")             # TODO: change for non-humanoid actors

set(_all_hkx_outputs "")
set(_deploy_hkx_files "")
set(_deploy_subdirs "")

file(GLOB_RECURSE _all_yaml_files
    "${CREATURE_DIR}/*behavior.yaml"
    "${CREATURE_DIR}/*character.yaml"
)

foreach(_yaml IN LISTS _all_yaml_files)
    get_filename_component(_src_dir "${_yaml}" DIRECTORY)
    get_filename_component(_folder_name "${_src_dir}" NAME)

    if(NOT _folder_name MATCHES "\\.hkx$")
        continue()
    endif()

    get_filename_component(_yaml_name "${_yaml}" NAME)
    if(_yaml_name STREQUAL "behavior.yaml")
        set(_entry_type "behavior")
    elseif(_yaml_name STREQUAL "character.yaml")
        set(_entry_type "character")
    else()
        continue()
    endif()

    string(REGEX REPLACE "\\.hkx$" "" _base_name "${_folder_name}")

    set(_xml "${CMAKE_CURRENT_BINARY_DIR}/${_base_name}.xml")
    set(_hkx "${CMAKE_CURRENT_BINARY_DIR}/${_base_name}.hkx")

    add_custom_command(
        OUTPUT  "${_xml}"
        COMMAND "${HKBUILD_EXE}" "${_src_dir}" -o "${_xml}"
        DEPENDS "${_yaml}" HKBuild
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "HKBuild: compiling ${_base_name} (${_entry_type} YAML -> XML)"
        VERBATIM
    )

    add_custom_command(
        OUTPUT  "${_hkx}"
        COMMAND "${HKBUILD_EXE}" pack "${_xml}" -o "${_hkx}"
        DEPENDS "${_xml}" HKBuild
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "HKBuild: packing ${_base_name} (XML -> HKX)"
        VERBATIM
    )

    list(APPEND _all_hkx_outputs "${_hkx}")

    get_filename_component(_parent_dir "${_src_dir}" DIRECTORY)
    file(RELATIVE_PATH _rel_subdir "${CREATURE_DIR}" "${_parent_dir}")

    list(APPEND _deploy_hkx_files "${_hkx}")
    list(APPEND _deploy_subdirs "${_rel_subdir}")
endforeach()

add_custom_target(${BEHAVIOR_TARGET} ALL
    DEPENDS ${_all_hkx_outputs}
)

# ---------------------------------------------------------------------------
# Deploy to MO2 mod folder (only when SKYRIM_MODS_FOLDER env var is set)
# ---------------------------------------------------------------------------
if(DEFINED ENV{SKYRIM_MODS_FOLDER} AND IS_DIRECTORY "$ENV{SKYRIM_MODS_FOLDER}")
    set(_mod_base "$ENV{SKYRIM_MODS_FOLDER}/${MOD_FOLDER_NAME}/meshes/actors/${CREATURE_TYPE}")

    list(LENGTH _deploy_hkx_files _deploy_count)
    math(EXPR _deploy_last "${_deploy_count} - 1")
    foreach(_i RANGE 0 ${_deploy_last})
        list(GET _deploy_hkx_files ${_i} _hkx)
        list(GET _deploy_subdirs ${_i} _subdir)

        get_filename_component(_hkx_name "${_hkx}" NAME)
        set(_deploy_dir "${_mod_base}/${_subdir}")

        add_custom_command(TARGET ${BEHAVIOR_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_deploy_dir}"
            COMMAND ${CMAKE_COMMAND} -E copy "${_hkx}" "${_deploy_dir}/${_hkx_name}"
            COMMENT "Deploying ${_hkx_name} -> ${_subdir}/"
            VERBATIM
        )
    endforeach()
endif()
