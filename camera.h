#pragma once
#include "core.h"
#include "bodies.h"

mat4 gView, gProj;
vec3 gEye;

void updateCamera(float dt) {
    if (gFollow && gSelected >= 0) {
        float k = 1.0f - exp(-6.0f * dt);
        gCam.target += (gBodies[gSelected].position - gCam.target) * k;
    } else if (!gFollow) {
        float k = 1.0f - exp(-6.0f * dt);
        gCam.target += (gSunOffset - gCam.target) * k;
    }
    float k2 = 1.0f - exp(-8.0f * dt);
    gCam.distance += (gCam.targetDistance - gCam.distance) * k2;
    gCam.pitch = std::clamp(gCam.pitch, -1.5f, 1.5f);

    vec3 offset(cos(gCam.pitch) * cos(gCam.yaw), sin(gCam.pitch), cos(gCam.pitch) * sin(gCam.yaw));
    gEye = gCam.target + offset * gCam.distance;
    gView = lookAt(gEye, gCam.target, vec3(0, 1, 0));
    float aspect = (float)gWidth / (float)std::max(gHeight, 1);
    gProj = perspective(radians(45.0f), aspect, 0.05f, 600.0f);
}

int pickBody(double mx, double my) {
    float x = (float)(2.0 * mx / gWidth - 1.0);
    float y = (float)(1.0 - 2.0 * my / gHeight);
    mat4 inv = inverse(gProj * gView);
    vec4 a = inv * vec4(x, y, -1.0f, 1.0f);
    vec4 b = inv * vec4(x, y, 1.0f, 1.0f);
    vec3 o = vec3(a) / a.w;
    vec3 d = normalize(vec3(b) / b.w - o);
    int best = -1;
    float bestT = 1e30f;
    for (size_t i = 0; i < gBodies.size(); ++i) {
        vec3 c = gBodies[i].position;
        float dist = length(c - gEye);
        float r = std::max(gBodies[i].radius, dist * 0.018f);
        vec3 oc = o - c;
        float bq = dot(oc, d);
        float cq = dot(oc, oc) - r * r;
        float disc = bq * bq - cq;
        if (disc < 0.0f) continue;
        float t = -bq - sqrt(disc);
        if (t > 0.0f && t < bestT) { bestT = t; best = (int)i; }
    }
    return best;
}

