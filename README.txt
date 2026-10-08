========================================================================
Interactive 3D Solar System Simulation - Topic 17
Computer Graphics Fundamentals (IT-2511)
========================================================================

DESCRIPTION:
Real-time 3D Solar System simulation built with C++17 and OpenGL 3.3 Core.
Features procedural UV sphere meshes, hierarchical orbital transformations
(Sun -> Earth -> Moon), custom GLSL shaders (emissive Sun, Blinn-Phong 
planet lighting with attenuation), and equirectangular texture mapping.

------------------------------------------------------------------------
BUILD & RUN INSTRUCTIONS
------------------------------------------------------------------------

MACOS (Apple Silicon / Intel):
  Option 1 (Direct Terminal):
    $ make && ./solar

  Option 2 (CMake):
    $ cmake -B build && cmake --build build && ./build/solar

WINDOWS (Visual Studio 2022 / CMake):
  Option 1: Open project folder in Visual Studio 2022 (auto-detects CMake) -> Press F5.
  Option 2 (Command Prompt):
    > cmake -B build
    > cmake --build build --config Release
    > .\build\Release\solar.exe

------------------------------------------------------------------------
PROJECT STRUCTURE
------------------------------------------------------------------------
├── main.cpp                # Render loop & camera setup
├── Shader.h                # GLSL compilation & uniform helper
├── stb_image.h             # Image loader
├── glad.c / glad/ / KHR/   # OpenGL 3.3 Loader
├── shaders/                # GLSL source files (sun, planet)
└── textures/               # 2K texture maps (sun, earth, moon)
