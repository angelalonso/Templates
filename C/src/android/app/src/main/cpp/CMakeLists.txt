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
set(JSONC_SHIM_DIR ${CMAKE_BINARY_DIR}/json-c-shim)
file(MAKE_DIRECTORY ${JSONC_SHIM_DIR}/json-c)
file(CREATE_LINK ${json-c_BINARY_DIR} ${JSONC_SHIM_DIR}/json-c/build SYMBOLIC)
file(CREATE_LINK ${json-c_SOURCE_DIR} ${JSONC_SHIM_DIR}/json-c/src   SYMBOLIC)
file(WRITE ${JSONC_SHIM_DIR}/json-c/json.h
    "#pragma once\n"
    "#include \"build/json.h\"\n"
    "#include \"src/json_object.h\"\n"
    "#include \"src/json_tokener.h\"\n"
    "#include \"src/json_util.h\"\n"
)

# ── Path to repo root (6 levels up from src/android/app/src/main/cpp) ────────
set(REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../)

# ── Our native library ────────────────────────────────────────────────────────
add_library(
    configeditor
    SHARED
    jni_bridge.c
    ${REPO_ROOT}/src/backend/api_handler.c
    ${REPO_ROOT}/src/backend/config_manager.c
)
target_include_directories(configeditor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${REPO_ROOT}/src/backend
    ${REPO_ROOT}/src/middleware
    ${JSONC_SHIM_DIR}
    ${json-c_SOURCE_DIR}
    ${json-c_BINARY_DIR}
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
