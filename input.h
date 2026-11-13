#pragma once
#include "core.h"
#include "bodies.h"

#include "camera.h"


double gMouseX = 0, gMouseY = 0, gPressX = 0, gPressY = 0;
bool gDragging = false;

void framebufferSize(GLFWwindow*, int w, int h) {
    gWidth = std::max(w, 1);
    gHeight = std::max(h, 1);
    gResized = true;
}

void mouseButton(GLFWwindow* window, int button, int action, int) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;
    glfwGetCursorPos(window, &gMouseX, &gMouseY);
    if (action == GLFW_PRESS) {
        gDragging = true;
        gPressX = gMouseX;
        gPressY = gMouseY;
    } else {
        gDragging = false;
        double moved = fabs(gMouseX - gPressX) + fabs(gMouseY - gPressY);
        if (moved < 5.0) {
            int w, h, fw, fh;
            glfwGetWindowSize(window, &w, &h);
            glfwGetFramebufferSize(window, &fw, &fh);
            double sx = (double)fw / std::max(w, 1), sy = (double)fh / std::max(h, 1);
            int hit = pickBody(gMouseX * sx, gMouseY * sy);
            if (hit >= 0) selectBody(hit);
        }
    }
}

void cursorMove(GLFWwindow*, double x, double y) {
    if (gDragging) {
        gCam.yaw += (float)(x - gMouseX) * 0.006f;
        gCam.pitch += (float)(y - gMouseY) * 0.006f;
    }
    gMouseX = x;
    gMouseY = y;
}

void scrollWheel(GLFWwindow*, double, double dy) {
    float minD = gSelected >= 0 ? gBodies[gSelected].radius * 1.6f : 3.0f;
    gCam.targetDistance = std::clamp(gCam.targetDistance * (float)pow(0.9, dy), minD, 220.0f);
}

void keyCallback(GLFWwindow* window, int key, int, int action, int mods) {
    if (action != GLFW_PRESS) return;
    switch (key) {
    case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, true); break;
    case GLFW_KEY_SPACE: gPaused = !gPaused; break;
    case GLFW_KEY_B: gBloom = !gBloom; break;
    case GLFW_KEY_O: gOrbits = !gOrbits; break;
    case GLFW_KEY_N: gLighting = !gLighting; break;
    case GLFW_KEY_H:
    case GLFW_KEY_F1: gHelp = !gHelp; break;
    case GLFW_KEY_R: resetView(); break;
    case GLFW_KEY_T: gVisited.clear(); resetView(); break;
    case GLFW_KEY_F:
        if (gSelected >= 0) gFollow = !gFollow;
        break;
    case GLFW_KEY_COMMA: gSpeedIndex = std::max(gSpeedIndex - 1, 0); break;
    case GLFW_KEY_PERIOD: gSpeedIndex = std::min(gSpeedIndex + 1, kSpeedCount - 1); break;
    case GLFW_KEY_TAB: {
        int n = (int)gBodies.size();
        int next = (mods & GLFW_MOD_SHIFT) ? (gSelected - 1 + n) % n : (gSelected + 1) % n;
        if (gSelected < 0) next = 0;
        selectBody(next);
        break;
    }
    case GLFW_KEY_BACKSPACE: gSunOffset = vec3(0.0f); break;
    case GLFW_KEY_P: gScreenshotRequested = true; break;
    case GLFW_KEY_0: selectBody(0); break;
    case GLFW_KEY_M: selectBody(9); break;
    default:
        if (key >= GLFW_KEY_1 && key <= GLFW_KEY_8) selectBody(key - GLFW_KEY_1 + 1);
        break;
    }
}

void pollKeys(GLFWwindow* window, float dt) {
    auto down = [&](int k) { return glfwGetKey(window, k) == GLFW_PRESS; };
    float rot = 1.5f * dt;
    if (down(GLFW_KEY_LEFT)) gCam.yaw -= rot;
    if (down(GLFW_KEY_RIGHT)) gCam.yaw += rot;
    if (down(GLFW_KEY_UP)) gCam.pitch += rot;
    if (down(GLFW_KEY_DOWN)) gCam.pitch -= rot;
    float minD = gSelected >= 0 ? gBodies[gSelected].radius * 1.6f : 3.0f;
    if (down(GLFW_KEY_W) || down(GLFW_KEY_EQUAL)) gCam.targetDistance = std::max(gCam.targetDistance * (1.0f - 1.5f * dt), minD);
    if (down(GLFW_KEY_S) || down(GLFW_KEY_MINUS)) gCam.targetDistance = std::min(gCam.targetDistance * (1.0f + 1.5f * dt), 220.0f);
    if (down(GLFW_KEY_A)) gCam.yaw -= rot;
    if (down(GLFW_KEY_D)) gCam.yaw += rot;

    float move = 8.0f * dt;
    if (down(GLFW_KEY_J)) gSunOffset.x -= move;
    if (down(GLFW_KEY_L)) gSunOffset.x += move;
    if (down(GLFW_KEY_I)) gSunOffset.z -= move;
    if (down(GLFW_KEY_K)) gSunOffset.z += move;
    if (down(GLFW_KEY_U)) gSunOffset.y += move;
    if (down(GLFW_KEY_Y)) gSunOffset.y -= move;
}


