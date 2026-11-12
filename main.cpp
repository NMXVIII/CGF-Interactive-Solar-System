#include "core.h"
#include "bodies.h"
#include "assets.h"
#include "meshes.h"
#include "postfx.h"
#include "hud.h"
#include "orbits.h"
#include "camera.h"
#include "input.h"

int main(int argc, char** argv) {
    string shotPath;
    int startSelect = -1, shotFrames = 4;
    double startDays = 0.0;
    float optYaw = NAN, optPitch = NAN, optDist = NAN;
    bool startBloom = true, startHelp = true;
    bool startSunMove = false;
    for (int i = 1; i < argc; ++i) {
        string a = argv[i];
        if (a == "--screenshot" && i + 1 < argc) shotPath = argv[++i];
        else if (a == "--select" && i + 1 < argc) startSelect = atoi(argv[++i]);
        else if (a == "--days" && i + 1 < argc) startDays = atof(argv[++i]);
        else if (a == "--yaw" && i + 1 < argc) optYaw = (float)atof(argv[++i]);
        else if (a == "--pitch" && i + 1 < argc) optPitch = (float)atof(argv[++i]);
        else if (a == "--dist" && i + 1 < argc) optDist = (float)atof(argv[++i]);
        else if (a == "--nobloom") startBloom = false;
        else if (a == "--nohelp") startHelp = false;
        else if (a == "--movesun") startSunMove = true;
    }
    gBloom = startBloom;
    gHelp = startHelp;
    gSimDays = startDays;

    if (!glfwInit()) {
        cerr << "Failed to initialise GLFW" << endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, kTitle, NULL, NULL);
    if (!window) {
        cerr << "Failed to create GLFW window (OpenGL 3.3 core required)" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to initialise GLAD" << endl;
        return -1;
    }
    cout << "OpenGL " << glGetString(GL_VERSION) << " | " << glGetString(GL_RENDERER) << endl;

    glfwSetFramebufferSizeCallback(window, framebufferSize);
    glfwSetMouseButtonCallback(window, mouseButton);
    glfwSetCursorPosCallback(window, cursorMove);
    glfwSetScrollCallback(window, scrollWheel);
    glfwSetKeyCallback(window, keyCallback);
    glfwGetFramebufferSize(window, &gWidth, &gHeight);

    Shader sunShader("shaders/sun.vert", "shaders/sun.frag");
    Shader planetShader("shaders/planet.vert", "shaders/planet.frag");
    Shader lineShader("shaders/line.vert", "shaders/line.frag");
    Shader starShader("shaders/stars.vert", "shaders/stars.frag");
    Shader textShader("shaders/text.vert", "shaders/text.frag");
    Shader brightShader("shaders/quad.vert", "shaders/bright.frag");
    Shader blurShader("shaders/quad.vert", "shaders/blur.frag");
    Shader compositeShader("shaders/quad.vert", "shaders/composite.frag");

    buildBodies();
    for (Body& b : gBodies) b.texture = loadBodyTexture(b);
    unsigned int ringTex = loadTexture("textures/2k_saturn_ring_alpha.png", true, false);
    if (!ringTex) ringTex = loadTexture("textures/placeholder/saturn_ring.png", true, true);

    Mesh sphere = createSphere(1.0f, 48, 64);
    Mesh ring = createRing(1.35f, 2.4f, 96);
    int circleCount, starCount;
    unsigned int circleVAO = createUnitCircle(180, circleCount);
    unsigned int starVAO = createStars(2600, starCount);
    unsigned int emptyVAO;
    glGenVertexArrays(1, &emptyVAO);
    gText.init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_PROGRAM_POINT_SIZE);

    if (startSunMove) gSunOffset = vec3(10.0f, 0.0f, -6.0f);
    updateBodies();
    if (startSelect >= 0) {
        selectBody(startSelect);
        gCam.distance = gCam.targetDistance;
        gCam.target = gBodies[startSelect].position;
    }
    if (!isnan(optYaw)) gCam.yaw = optYaw;
    if (!isnan(optPitch)) gCam.pitch = optPitch;
    if (!isnan(optDist)) { gCam.distance = optDist; gCam.targetDistance = optDist; }

    cout << "Controls: 0-8 planets, M Moon, TAB next, click to select, F follow, R reset, SPACE pause, "
        ", . speed, B bloom, O orbits, N lighting, J/L/I/K/U/Y move the Sun, H help, P screenshot" << endl;

    double last = glfwGetTime();
    double fpsTimer = 0;
    int fpsFrames = 0;
    float fps = 0;
    int frame = 0;

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        float dt = (float)std::min(now - last, 0.1);
        last = now;
        fpsTimer += dt;
        fpsFrames++;
        if (fpsTimer >= 0.5) { fps = (float)(fpsFrames / fpsTimer); fpsTimer = 0; fpsFrames = 0; }

        glfwPollEvents();
        pollKeys(window, dt);
        if (!gPaused) gSimDays += dt * kBaseDaysPerSecond * kSpeeds[gSpeedIndex];

        if (gResized || gT.w != gWidth || gT.h != gHeight) {
            createTargets(gWidth, gHeight);
            gResized = false;
        }
        if (gWidth < 2 || gHeight < 2) { glfwSwapBuffers(window); continue; }

        updateBodies();
        updateCamera(dt);

        glBindFramebuffer(GL_FRAMEBUFFER, gT.msFbo);
        glViewport(0, 0, gWidth, gHeight);
        glClearColor(0.0f, 0.0f, 0.012f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        starShader.use();
        starShader.setMat4("view", gView);
        starShader.setMat4("projection", gProj);
        starShader.setFloat("pixelScale", std::max(1.0f, gHeight / 720.0f));
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBindVertexArray(starVAO);
        glDrawArrays(GL_POINTS, 0, starCount);
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        if (gOrbits) {
            lineShader.use();
            lineShader.setMat4("view", gView);
            lineShader.setMat4("projection", gProj);
            glBindVertexArray(circleVAO);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            for (size_t i = 1; i < gBodies.size(); ++i) {
                const Body& b = gBodies[i];
                const Body& p = gBodies[b.parent];
                mat4 m = scale(translate(mat4(1.0f), p.position), vec3(b.orbitRadius));
                lineShader.setMat4("model", m);
                bool sel = (int)i == gSelected;
                bool moon = b.parent != 0;
                float a = sel ? 0.95f : (moon ? 0.28f : 0.38f);
                vec3 c = sel ? vec3(1.0f, 0.85f, 0.3f) : vec3(0.55f, 0.65f, 0.85f);
                lineShader.setVec4("lineColor", c.r, c.g, c.b, a);
                glDrawArrays(GL_LINE_LOOP, 0, circleCount);
            }
            glDisable(GL_BLEND);
        }

        sunShader.use();
        sunShader.setMat4("view", gView);
        sunShader.setMat4("projection", gProj);
        sunShader.setMat4("model", gBodies[0].model);
        sunShader.setFloat("intensity", 2.2f);
        sunShader.setVec3("sunColor", gBodies[0].color);
        sunShader.setBool("useTexture", gBodies[0].texture != 0);
        sunShader.setInt("sunTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gBodies[0].texture);
        drawMesh(sphere);

        planetShader.use();
        planetShader.setMat4("view", gView);
        planetShader.setMat4("projection", gProj);
        planetShader.setVec3("lightPos", gBodies[0].position);
        planetShader.setVec3("viewPos", gEye);
        planetShader.setVec3("lightColor", vec3(1.25f, 1.2f, 1.1f));
        planetShader.setBool("lightingEnabled", gLighting);
        planetShader.setFloat("ambientStrength", 0.07f);
        planetShader.setFloat("alpha", 1.0f);
        planetShader.setBool("ringMode", false);
        planetShader.setInt("planetTexture", 0);
        for (size_t i = 1; i < gBodies.size(); ++i) {
            const Body& b = gBodies[i];
            planetShader.setMat4("model", b.model);
            planetShader.setMat3("normalMatrix", transpose(inverse(mat3(b.model))));
            planetShader.setBool("useTexture", b.texture != 0);
            planetShader.setVec3("planetColor", b.color);
            planetShader.setFloat("specularStrength", b.specular);
            glBindTexture(GL_TEXTURE_2D, b.texture);
            drawMesh(sphere);
        }

        const Body& saturn = gBodies[6];
        if (saturn.hasRing && ringTex) {
            mat4 rm = translate(mat4(1.0f), saturn.position);
            rm = rotate(rm, radians(saturn.tiltDeg), vec3(0, 0, 1));
            rm = scale(rm, vec3(saturn.radius));
            planetShader.setMat4("model", rm);
            planetShader.setMat3("normalMatrix", transpose(inverse(mat3(rm))));
            planetShader.setBool("useTexture", true);
            planetShader.setBool("ringMode", true);
            planetShader.setFloat("specularStrength", 0.0f);
            planetShader.setFloat("alpha", 0.9f);
            glBindTexture(GL_TEXTURE_2D, ringTex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);
            glDisable(GL_CULL_FACE);
            drawMesh(ring);
            glEnable(GL_CULL_FACE);
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
            planetShader.setBool("ringMode", false);
        }

        resolveAndBloom(brightShader, blurShader, compositeShader, emptyVAO);
        drawHud(textShader, fps);

        frame++;
        if (gScreenshotRequested) {
            gScreenshotRequested = false;
            static int shotIndex = 0;
            string name = "screenshot_" + to_string(++shotIndex) + ".png";
            if (saveScreenshot(name)) cout << "saved " << name << endl;
        }
        if (!shotPath.empty() && frame == shotFrames) {
            saveScreenshot(shotPath);
            break;
        }

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
