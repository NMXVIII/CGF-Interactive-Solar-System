cmake_minimum_required(VERSION 3.10)
project(CGF_SolarSystem CXX C)

set(CMAKE_CXX_STANDARD 17)

# search current directory for this folders: glad/ and KHR/ headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# find OpenGL package
find_package(OpenGL REQUIRED)

# on macOS, we ignore deprecated warnings , and add directories for Homebrew libraries
if (APPLE) 
    add_compile_definitions(GL_SILENCE_DEPRECATION)
    include_directories(/opt/homebrew/include)
    link_directories(/opt/homebrew/lib)
    find_package(glfw3 REQUIRED)
    set(EXTRA_LIBS glfw OpenGL::GL)
elseif (WIN32)
    # on Windows, CMake looks for GLFW via vcpkg, local folder, or system paths
    find_package(glfw3 REQUIRED)
    set(EXTRA_LIBS glfw OpenGL::GL)
endif()

add_executable(solar main.cpp glad.c)
target_link_libraries(solar ${EXTRA_LIBS})
