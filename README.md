# Interactive Solar System (Topic 17)

Computer Graphics Fundamentals, final project. Team: Baigurman Meiirzhan, Kabylkhanov Alimzhan, Nurlanbek Marghulan (IT-2511).

C++17, OpenGL 3.3 core, GLFW 3, GLAD, GLM, stb_image. Builds with CMake on Windows 10/11, macOS and Linux. GLAD and stb_image are in the repository; GLFW and GLM are downloaded automatically by CMake if they are not installed.

## Build and run

### Windows 10 / 11

Install once: Visual Studio 2022 Community with the workload "Desktop development with C++" (it includes the compiler and CMake). Then open "Developer PowerShell for VS 2022", go to the project folder and run:

    cmake -S . -B build
    cmake --build build --config Release
    .\solar.exe

The first `cmake -S . -B build` downloads GLFW and GLM (internet needed, about one minute). Alternative: in Visual Studio choose File > Open > Folder, wait until CMake has configured, select `solar.exe` as startup item and press F5 (the working directory is set to the project folder).

### macOS

    brew install cmake glfw glm
    cmake -S . -B build
    cmake --build build
    ./solar

or simply `make && ./solar`.

### Linux

    sudo apt install cmake build-essential libglfw3-dev libglm-dev
    cmake -S . -B build && cmake --build build
    ./solar

The program loads `shaders/` and `textures/` with relative paths, so the executable `solar` / `solar.exe` is created in the project folder and has to be started from there.

## Keys

| Key | Action |
|---|---|
| Mouse drag, arrows, A / D | rotate the camera around the target |
| Wheel, W / S, + / - | zoom |
| Click on a body, 0 (Sun), 1-8 (planets), M (Moon), TAB / Shift+TAB | select and focus a body, shows name and facts |
| F | follow on / off for the selected body |
| R | reset to the system view |
| T | restart the exploration tour (goal: visit all 13 bodies, a banner appears when done) |
| SPACE | pause time |
| , and . | slower / faster time (x0.25 to x1024) |
| B | bloom on / off (headline technique) |
| O | orbit lines on / off |
| N | lighting on / off (flat colours for comparison) |
| J L I K U Y | move the Sun (the light source and the root of the hierarchy), BACKSPACE returns it |
| H or F1 | help on / off |
| P | screenshot (screenshot_N.png) |
| ESC | quit |

## What is implemented

- Sun, 8 planets with real relative orbital periods (Mercury 88 d ... Neptune 60190 d), distances and sizes are artistic.
- Moons: Moon (Earth), Ganymede and Callisto (Jupiter), Titan (Saturn). Saturn has a ring.
- Hierarchical transforms: Sun -> planet -> moon, each with orbit rotation, axial tilt and spin.
- Sun is an emissive shader and the point light for all planets (Blinn-Phong with distance attenuation).
- Bloom on the Sun: HDR (RGBA16F) 4x MSAA framebuffer, bright pass, 10-pass separable Gaussian blur at half resolution, soft-clip composite.
- Star field (2600 point sprites), orbit lines, on-screen HUD with team, time, FPS, selected body facts, help.

## Assets and licenses

Textures (equirectangular maps): Solar System Scope texture library, https://www.solarsystemscope.com/textures, CC BY 4.0, copyright (c) 2010-2017 Solar System Scope. File names: 2k_sun.jpg, 2k_mercury.jpg, 2k_venus_surface.jpg, 2k_earth_daymap.jpg, 2k_moon.jpg, 2k_mars.jpg, 2k_jupiter.jpg, 2k_saturn.jpg, 2k_saturn_ring_alpha.png, 2k_uranus.jpg, 2k_neptune.jpg. If a file is missing, the program prints a message and uses a generated placeholder (textures/placeholder, made by tools/make_assets.py).

HUD font: DejaVu Sans Mono Bold rendered to textures/font.png by tools/make_assets.py (DejaVu fonts license).

Libraries: GLFW, GLAD, GLM, stb_image (public domain). png_writer.h is our own small PNG writer for screenshots.

## Structure and ownership

    Baigurman Meiirzhan (core engine, main loop, geometry)
      main.cpp      window and context setup, scene render pass, main loop
      core.h        shared includes, constants, window size
      meshes.h      sphere, ring, orbit circle, star field
      orbits.h      orbit hierarchy Sun -> planet -> moon (model matrices)

    Kabylkhanov Alimzhan (graphics and shaders)
      Shader.h      shader loading and uniform helpers
      shaders/      sun, planet, line, stars, text, bloom GLSL
      postfx.h      MSAA HDR framebuffers, bright pass, blur, composite

    Nurlanbek Marghulan (data, camera, interface, assets, build, documentation)
      bodies.h      body table (orbits, periods, facts), simulation state, selection
      camera.h      orbit camera, view/projection, mouse picking
      input.h       keyboard and mouse callbacks
      assets.h      texture loading
      hud.h         text renderer, HUD, help, screenshot
      textures/, tools/make_assets.py, CMakeLists.txt, Makefile, README.md, report, video
