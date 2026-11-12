#pragma once
#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Shader.h"
#include "png_writer.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;
using namespace glm;

static const float PI = 3.14159265359f;
static const char* kTitle = "Topic 17: Interactive Solar System";
static const char* kTeam = "Baigurman M., Kabylkhanov A., Nurlanbek M.";

int gWidth = 1280, gHeight = 720;
bool gResized = true;
