#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "Shader.h"
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
using namespace glm;

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else {
        cout << "[Texture Info] No texture found at: " << path << " (Using fallback color)" << endl;
        stbi_image_free(data);
        return 0;
    }
    return textureID;
}

void createSphere(float radius, unsigned int rings, unsigned int sectors,
    unsigned int& vao, unsigned int& vbo, unsigned int& ebo, unsigned int& indexCount) {
    vector<float> vertices;
    vector<unsigned int> indices;
    const float PI = 3.14159265359f;
    for (unsigned int r = 0; r <= rings; ++r) {
        float v = (float)r / (float)rings;
        float phi = v * PI;
        for (unsigned int s = 0; s <= sectors; ++s) {
            float u = (float)s / (float)sectors;
            float theta = u * 2.0f * PI;
            float x = cos(theta) * sin(phi);
            float y = cos(phi);
            float z = sin(theta) * sin(phi);
            vertices.push_back(x * radius);
            vertices.push_back(y * radius);
            vertices.push_back(z * radius);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }
    for (unsigned int r = 0; r < rings; ++r) {
        for (unsigned int s = 0; s < sectors; ++s) {
            unsigned int first = (r * (sectors + 1)) + s;
            unsigned int second = first + sectors + 1;
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    indexCount = static_cast<unsigned int>(indices.size());
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

unsigned int createOrbitRing(float radius, int segments = 100) {
    std::vector<float> vertices;
    const float PI = 3.14159265359f;
    for (int i = 0; i < segments; ++i) {
        float angle = 2.0f * PI * i / segments;
        vertices.push_back(radius * cos(angle));
        vertices.push_back(0.0f);
        vertices.push_back(radius * sin(angle));
    }
    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    return vao;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Interactive Solar System", NULL, NULL);
    if (!window) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader sunShader("shaders/sun.vert", "shaders/sun.frag");
    Shader planetShader("shaders/planet.vert", "shaders/planet.frag");

    unsigned int sunTex = loadTexture("textures/2k_sun.jpg");
    unsigned int earthTex = loadTexture("textures/2k_earth_daymap.jpg");
    unsigned int moonTex = loadTexture("textures/2k_moon.jpg");

    unsigned int sphereVAO, sphereVBO, sphereEBO, sphereIndexCount;
    createSphere(1.0f, 30, 30, sphereVAO, sphereVBO, sphereEBO, sphereIndexCount);

    unsigned int earthOrbitVAO = createOrbitRing(5.0f);

    vec3 cameraPos = vec3(0.0f, 6.0f, 10.0f);
    mat4 view = lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 projection = perspective(radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        float time = (float)glfwGetTime();

        glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sunShader.use();
        sunShader.setMat4("view", view);
        sunShader.setMat4("projection", projection);
        sunShader.setMat4("model", mat4(1.0f));
        sunShader.setBool("useTexture", false);
        sunShader.setVec3("sunColor", vec3(0.3f, 0.3f, 0.4f));
        glBindVertexArray(earthOrbitVAO);
        glDrawArrays(GL_LINE_LOOP, 0, 100);

        mat4 sunModel = scale(mat4(1.0f), vec3(1.2f));
        sunShader.setMat4("model", sunModel);
        if (sunTex != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sunTex);
            sunShader.setInt("sunTexture", 0);
            sunShader.setBool("useTexture", true);
        }
        else {
            sunShader.setBool("useTexture", false);
            sunShader.setVec3("sunColor", vec3(1.0f, 0.8f, 0.1f));
        }
        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);

        planetShader.use();
        planetShader.setMat4("view", view);
        planetShader.setMat4("projection", projection);
        planetShader.setVec3("lightPos", vec3(0.0f));
        planetShader.setVec3("viewPos", cameraPos);

        mat4 earthWorld = rotate(mat4(1.0f), time * 0.6f, vec3(0.0f, 1.0f, 0.0f));
        earthWorld = translate(earthWorld, vec3(5.0f, 0.0f, 0.0f));

        mat4 earthRenderMatrix = rotate(earthWorld, time * 1.5f, vec3(0.0f, 1.0f, 0.0f));
        earthRenderMatrix = scale(earthRenderMatrix, vec3(0.4f));
        planetShader.setMat4("model", earthRenderMatrix);

        mat3 normalMatrix = transpose(inverse(mat3(earthRenderMatrix)));
        planetShader.setMat3("normalMatrix", normalMatrix);

        if (earthTex != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, earthTex);
            planetShader.setInt("planetTexture", 0);
            planetShader.setBool("useTexture", true);
        }
        else {
            planetShader.setBool("useTexture", false);
            planetShader.setVec3("planetColor", vec3(0.2f, 0.5f, 1.0f));
        }
        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);

        vec3 earthPos = vec3(earthWorld[3]);
        mat4 moonWorld = translate(mat4(1.0f), earthPos);
        moonWorld = rotate(moonWorld, time * 2.5f, vec3(0.0f, 1.0f, 0.0f));
        moonWorld = translate(moonWorld, vec3(0.8f, 0.0f, 0.0f));
        moonWorld = scale(moonWorld, vec3(0.12f));

        planetShader.setMat4("model", moonWorld);

        mat3 moonNormalMatrix = transpose(inverse(mat3(moonWorld)));
        planetShader.setMat3("normalMatrix", moonNormalMatrix);

        if (moonTex != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, moonTex);
            planetShader.setInt("planetTexture", 0);
            planetShader.setBool("useTexture", true);
        }
        else {
            planetShader.setBool("useTexture", false);
            planetShader.setVec3("planetColor", vec3(0.8f, 0.8f, 0.8f));
        }
        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}