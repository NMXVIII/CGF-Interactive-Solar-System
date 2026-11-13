#pragma once
#include "core.h"



struct Body {
    string name;
    string fact[3];
    int parent;
    float orbitRadius;
    float periodDays;
    float radius;
    float spinHours;
    float tiltDeg;
    float phase;
    string textureFile;
    string placeholderFile;
    vec3 color;
    bool hasRing;
    float specular;
    unsigned int texture = 0;
    vec3 position = vec3(0.0f);
    mat4 model = mat4(1.0f);
};

vector<Body> gBodies;


void buildBodies() {
    auto add = [](Body b) { gBodies.push_back(b); };
    add({"Sun", {"G-type star, about 109 times wider than Earth.", "Surface temperature is about 5,500 C.", "It holds 99.86 % of the mass of the system."},
         -1, 0.0f, 1.0f, 2.0f, 609.0f, 7.25f, 0.0f, "textures/2k_sun.jpg", "", vec3(1.0f, 0.7f, 0.1f), false, 0.0f});
    add({"Mercury", {"Smallest planet and closest to the Sun.", "Year: 88 Earth days. No real atmosphere.", "Day side reaches about 430 C."},
         0, 4.2f, 88.0f, 0.25f, 1407.6f, 0.03f, 0.3f, "textures/2k_mercury.jpg", "textures/placeholder/mercury.png", vec3(0.6f), false, 0.1f});
    add({"Venus", {"Hottest planet, about 465 C at the surface.", "Year: 224.7 Earth days.", "It spins backwards (retrograde rotation)."},
         0, 5.8f, 224.7f, 0.55f, -5832.5f, 2.6f, 1.9f, "textures/2k_venus_surface.jpg", "textures/placeholder/venus.png", vec3(0.9f, 0.7f, 0.4f), false, 0.1f});
    add({"Earth", {"The only known planet with life.", "Year: 365.25 days, one day is 24 hours.", "It has one moon, simply called the Moon."},
         0, 7.8f, 365.25f, 0.6f, 23.93f, 23.4f, 4.0f, "textures/2k_earth_daymap.jpg", "", vec3(0.2f, 0.5f, 1.0f), false, 0.35f});
    add({"Mars", {"The Red Planet, coloured by iron oxide.", "Year: 687 Earth days.", "Home of Olympus Mons, the tallest volcano known."},
         0, 9.9f, 687.0f, 0.35f, 24.62f, 25.2f, 0.7f, "textures/2k_mars.jpg", "textures/placeholder/mars.png", vec3(0.8f, 0.4f, 0.2f), false, 0.1f});
    add({"Jupiter", {"Largest planet, about 11 times wider than Earth.", "Year: about 11.9 Earth years.", "The Great Red Spot is a storm older than 150 years."},
         0, 15.0f, 4332.6f, 1.6f, 9.93f, 3.1f, 5.2f, "textures/2k_jupiter.jpg", "textures/placeholder/jupiter.png", vec3(0.8f, 0.7f, 0.5f), false, 0.05f});
    add({"Saturn", {"Famous for its wide ring system.", "Year: about 29.4 Earth years.", "Its average density is lower than that of water."},
         0, 20.5f, 10759.0f, 1.35f, 10.66f, 26.7f, 2.4f, "textures/2k_saturn.jpg", "textures/placeholder/saturn.png", vec3(0.9f, 0.8f, 0.6f), true, 0.05f});
    add({"Uranus", {"Ice giant that rolls on its side (tilt about 98 deg).", "Year: about 84 Earth years.", "It spins backwards compared to most planets."},
         0, 25.0f, 30687.0f, 0.9f, -17.24f, 97.8f, 3.3f, "textures/2k_uranus.jpg", "textures/placeholder/uranus.png", vec3(0.6f, 0.85f, 0.9f), false, 0.1f});
    add({"Neptune", {"Farthest planet from the Sun.", "Year: about 165 Earth years.", "Winds are faster than 2,000 km/h."},
         0, 29.5f, 60190.0f, 0.85f, 16.11f, 28.3f, 5.9f, "textures/2k_neptune.jpg", "textures/placeholder/neptune.png", vec3(0.2f, 0.35f, 0.9f), false, 0.1f});
    add({"Moon", {"Earth's only natural satellite.", "Orbit: 27.3 days, the same face always points to Earth.", "It is about a quarter of Earth's diameter."},
         3, 1.2f, 27.32f, 0.16f, 655.7f, 6.7f, 0.0f, "textures/2k_moon.jpg", "", vec3(0.7f), false, 0.1f});
    add({"Ganymede", {"Largest moon in the solar system.", "It is bigger than the planet Mercury.", "Orbits Jupiter in 7.2 days."},
         5, 2.4f, 7.155f, 0.14f, 171.7f, 0.2f, 1.0f, "textures/2k_moon.jpg", "", vec3(0.6f, 0.55f, 0.5f), false, 0.1f});
    add({"Callisto", {"Heavily cratered moon of Jupiter.", "One of the oldest surfaces in the solar system.", "Orbits Jupiter in 16.7 days."},
         5, 3.2f, 16.69f, 0.13f, 400.5f, 0.3f, 3.0f, "textures/2k_moon.jpg", "", vec3(0.45f, 0.4f, 0.38f), false, 0.1f});
    add({"Titan", {"Largest moon of Saturn.", "It has a thick nitrogen atmosphere.", "Orbits Saturn in 15.9 days."},
         6, 2.6f, 15.945f, 0.17f, 382.7f, 0.3f, 2.0f, "textures/2k_moon.jpg", "", vec3(0.85f, 0.6f, 0.25f), false, 0.05f});
}



struct Camera {
    vec3 target = vec3(0.0f);
    float yaw = 0.8f;
    float pitch = 0.62f;
    float distance = 62.0f;
    float targetDistance = 62.0f;
};

Camera gCam;
int gSelected = -1;
bool gFollow = false;
bool gPaused = false;
bool gBloom = true;
bool gOrbits = true;
bool gHelp = true;
bool gLighting = true;
int gSpeedIndex = 2;
const float kSpeeds[] = {0.25f, 0.5f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f, 128.0f, 256.0f, 512.0f, 1024.0f};
const int kSpeedCount = 13;
const float kBaseDaysPerSecond = 365.25f / 60.0f;
double gSimDays = 0.0;
vec3 gSunOffset = vec3(0.0f);
set<int> gVisited;
bool gScreenshotRequested = false;

const float kSpinScale = 0.02f;

float sunInitialDistance() { return 62.0f; }

void resetView() {
    gCam = Camera();
    gSelected = -1;
    gFollow = false;
    gSunOffset = vec3(0.0f);
}

void selectBody(int index) {
    if (index < 0 || index >= (int)gBodies.size()) return;
    gSelected = index;
    gFollow = true;
    gCam.targetDistance = std::max(gBodies[index].radius * 5.5f, 1.2f);
    gVisited.insert(index);
    vec3 toSun = gBodies[0].position - gBodies[index].position;
    if (index != 0 && length(toSun) > 0.01f) {
        gCam.yaw = atan2(toSun.z, toSun.x) + 0.7f;
        gCam.pitch = 0.3f;
    }
}

