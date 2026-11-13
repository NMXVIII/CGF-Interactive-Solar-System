cmake_minimum_required(VERSION 3.18)
project(CGF_SolarSystem C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()


option(SOLAR_FORCE_FETCH "Always download GLFW and GLM, ignore installed copies" OFF)

include(FetchContent)


if(NOT SOLAR_FORCE_FETCH)
    find_package(glfw3 3.3 QUIET)
endif()
if(NOT glfw3_FOUND)
    message(STATUS "GLFW not installed: downloading GLFW 3.4")
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(glfw URL https://github.com/glfw/glfw/archive/refs/tags/3.4.zip)
    FetchContent_MakeAvailable(glfw)
endif()


set(GLM_DIR_FOUND "")
if(NOT SOLAR_FORCE_FETCH)
    find_path(GLM_INCLUDE_DIR glm/glm.hpp
        HINTS /opt/homebrew/include /usr/local/include /usr/include)
    if(GLM_INCLUDE_DIR)
        set(GLM_DIR_FOUND ${GLM_INCLUDE_DIR})
    endif()
endif()
if(NOT GLM_DIR_FOUND)
    message(STATUS "GLM not installed: downloading GLM 1.0.1")
    
    FetchContent_Declare(glm URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip SOURCE_SUBDIR no_cmake_here)
    FetchContent_MakeAvailable(glm)
    set(GLM_DIR_FOUND ${glm_SOURCE_DIR})
endif()

find_package(OpenGL REQUIRED)

add_executable(solar main.cpp glad.c)
target_include_directories(solar PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${GLM_DIR_FOUND})
target_link_libraries(solar PRIVATE glfw OpenGL::GL)

if(APPLE)
    target_compile_definitions(solar PRIVATE GL_SILENCE_DEPRECATION)
endif()
if(MSVC)
    target_compile_definitions(solar PRIVATE NOMINMAX _CRT_SECURE_NO_WARNINGS _USE_MATH_DEFINES)
    target_compile_options(solar PRIVATE /utf-8)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT solar)
endif()

# The program loads shaders/ and textures/ with relative paths, so the executable is
# placed in the project folder itself: run ./solar (Mac, Linux) or .\solar.exe (Windows).
set_target_properties(solar PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_SOURCE_DIR}
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
