#pragma once
#include "core.h"
#include "bodies.h"

void updateBodies() {
    for (size_t i = 0; i < gBodies.size(); ++i) {
        Body& b = gBodies[i];
        if (b.parent < 0) {
            b.position = vec3(0.0f);
            mat4 m = translate(mat4(1.0f), b.position);
            m = rotate(m, radians(b.tiltDeg), vec3(0, 0, 1));
            float spin = (float)(2.0 * PI * gSimDays * 24.0 / b.spinHours) * kSpinScale;
            m = rotate(m, spin, vec3(0, 1, 0));
            m = scale(m, vec3(b.radius));
            b.model = m;
            continue;
        }
        const Body& p = gBodies[b.parent];
        float angle = b.phase + (float)(2.0 * PI * gSimDays / b.periodDays);
        mat4 orbit = translate(mat4(1.0f), p.position);
        orbit = rotate(orbit, angle, vec3(0, 1, 0));
        orbit = translate(orbit, vec3(b.orbitRadius, 0, 0));
        b.position = vec3(orbit[3]);
        mat4 m = rotate(orbit, -angle, vec3(0, 1, 0));
        m = rotate(m, radians(b.tiltDeg), vec3(0, 0, 1));
        float spin = (float)(2.0 * PI * gSimDays * 24.0 / b.spinHours) * kSpinScale;
        m = rotate(m, spin, vec3(0, 1, 0));
        m = scale(m, vec3(b.radius));
        b.model = m;
    }
}

