cmake_minimum_required(VERSION 3.10)
project(ConfigApp VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Options
option(BUILD_TESTS "Build tests" ON)
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(BUILD_GUI_TESTS "Build GUI tests" ON)

# Find GTK3 (required for GUI)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# GUI source files
set(GUI_SOURCES
    src/gui.c
    src/gui_main.c
    src/gui_edit.c
    src/gui_common.c
    src/config.c
)

# GUI executable
add_executable(config_gui ${GUI_SOURCES})
target_include_directories(config_gui PRIVATE ${GTK3_INCLUDE_DIRS} src)
target_link_libraries(config_gui ${GTK3_LIBRARIES})

# Business logic library (for tests)
add_library(config_lib STATIC src/config.c)
target_include_directories(config_lib PUBLIC src)

# Tests
if(BUILD_TESTS)
    enable_testing()
    
    # Config tests
    add_executable(test_config tests/test_config.c)
    target_link_libraries(test_config config_lib)
    add_test(NAME ConfigTest COMMAND test_config)
    
    # Safety tests
    add_executable(test_safety tests/test_safety.c)
    target_link_libraries(test_safety config_lib)
    add_test(NAME SafetyTest COMMAND test_safety)
    
    # GUI tests
    if(BUILD_GUI_TESTS)
        add_executable(test_gui tests/test_gui.c)
        target_link_libraries(test_gui ${GTK3_LIBRARIES} config_lib)
        target_include_directories(test_gui PRIVATE ${GTK3_INCLUDE_DIRS} src)
        add_test(NAME GUITest COMMAND test_gui)
    endif()
    
    # Coverage
    if(ENABLE_COVERAGE)
        target_compile_options(test_config PRIVATE --coverage -O0 -g)
        target_link_libraries(test_config --coverage)
        target_compile_options(test_safety PRIVATE --coverage -O0 -g)
        target_link_libraries(test_safety --coverage)
        if(BUILD_GUI_TESTS)
            target_compile_options(test_gui PRIVATE --coverage -O0 -g)
            target_link_libraries(test_gui --coverage)
        endif()
    endif()
endif()

install(TARGETS config_gui DESTINATION bin)
