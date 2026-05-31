cmake_minimum_required(VERSION 3.22.1)
project(configeditor C)

# ── json-c ───────────────────────────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    json-c
    GIT_REPOSITORY https://github.com/json-c/json-c.git
    GIT_TAG        json-c-0.17-20230812
)
set(BUILD_TESTING   OFF CACHE BOOL "" FORCE)
set(BUILD_DOCS      OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(json-c)

# ── json-c shim ───────────────────────────────────────────────────────────────
# json-c installs headers flat (json-c-build/json.h, json-c-src/json_object.h)
# but source code uses #include <json-c/json.h>. Create a shim directory that
# has a json-c/ subdirectory symlinking to the real header locations so the
# prefix resolves correctly.
set(JSONC_SHIM_DIR ${CMAKE_BINARY_DIR}/json-c-shim)
file(MAKE_DIRECTORY ${JSONC_SHIM_DIR}/json-c)
file(CREATE_LINK ${json-c_BINARY_DIR} ${JSONC_SHIM_DIR}/json-c/build SYMBOLIC)
file(CREATE_LINK ${json-c_SOURCE_DIR} ${JSONC_SHIM_DIR}/json-c/src   SYMBOLIC)

# Generate a single forwarding json.h in the shim so <json-c/json.h> works
file(WRITE ${JSONC_SHIM_DIR}/json-c/json.h
    "#pragma once\n"
    "#include \"build/json.h\"\n"
    "#include \"src/json_object.h\"\n"
    "#include \"src/json_tokener.h\"\n"
    "#include \"src/json_util.h\"\n"
)

# ── Path to repo root (5 levels up from android/app/src/main/cpp) ────────────
set(REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../src)

# ── Our native library ────────────────────────────────────────────────────────
add_library(
    configeditor
    SHARED

    # JNI glue
    jni_bridge.c

    # Backend sources — resolved directly, no symlinks required
    ${REPO_ROOT}/backend/api_handler.c
    ${REPO_ROOT}/backend/config_manager.c
)

target_include_directories(configeditor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${REPO_ROOT}/backend
    ${REPO_ROOT}/middleware
    ${JSONC_SHIM_DIR}          # provides json-c/json.h via the shim above
    ${json-c_SOURCE_DIR}       # direct includes of json_object.h etc. if needed
    ${json-c_BINARY_DIR}       # json_config.h
)

target_link_libraries(configeditor
    json-c
    android
    log
)

target_compile_options(configeditor PRIVATE
    -Wall
    -Wextra
    -O2
)
