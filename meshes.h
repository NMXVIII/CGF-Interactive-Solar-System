#pragma once
#include "core.h"

struct Mesh {
    unsigned int vao = 0, vbo = 0, ebo = 0, count = 0;
};

Mesh createSphere(float radius, unsigned int rings, unsigned int sectors) {
    vector<float> vertices;
    vector<unsigned int> indices;
    for (unsigned int r = 0; r <= rings; ++r) {
        float v = (float)r / rings;
        float phi = v * PI;
        for (unsigned int s = 0; s <= sectors; ++s) {
            float u = (float)s / sectors;
            float theta = u * 2.0f * PI;
            float x = cos(theta) * sin(phi);
            float y = cos(phi);
            float z = sin(theta) * sin(phi);
            float row[8] = {x * radius, y * radius, z * radius, x, y, z, u, 1.0f - v};
            vertices.insert(vertices.end(), row, row + 8);
        }
    }
    for (unsigned int r = 0; r < rings; ++r) {
        for (unsigned int s = 0; s < sectors; ++s) {
            unsigned int first = r * (sectors + 1) + s;
            unsigned int second = first + sectors + 1;
            unsigned int quad[6] = {first, first + 1, second, second, first + 1, second + 1};
            indices.insert(indices.end(), quad, quad + 6);
        }
    }
    Mesh m;
    m.count = (unsigned int)indices.size();
    glGenVertexArrays(1, &m.vao);
    glGenBuffers(1, &m.vbo);
    glGenBuffers(1, &m.ebo);
    glBindVertexArray(m.vao);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    return m;
}

Mesh createRing(float inner, float outer, unsigned int segments) {
    vector<float> vertices;
    vector<unsigned int> indices;
    for (unsigned int i = 0; i <= segments; ++i) {
        float a = 2.0f * PI * i / segments;
        float c = cos(a), s = sin(a);
        float rowIn[8] = {inner * c, 0.0f, inner * s, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f};
        float rowOut[8] = {outer * c, 0.0f, outer * s, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f};
        vertices.insert(vertices.end(), rowIn, rowIn + 8);
        vertices.insert(vertices.end(), rowOut, rowOut + 8);
    }
    for (unsigned int i = 0; i < segments; ++i) {
        unsigned int k = i * 2;
        unsigned int quad[6] = {k, k + 1, k + 2, k + 2, k + 1, k + 3};
        indices.insert(indices.end(), quad, quad + 6);
    }
    Mesh m;
    m.count = (unsigned int)indices.size();
    glGenVertexArrays(1, &m.vao);
    glGenBuffers(1, &m.vbo);
    glGenBuffers(1, &m.ebo);
    glBindVertexArray(m.vao);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    return m;
}

unsigned int createUnitCircle(int segments, int& count) {
    vector<float> v;
    for (int i = 0; i < segments; ++i) {
        float a = 2.0f * PI * i / segments;
        v.push_back(cos(a));
        v.push_back(0.0f);
        v.push_back(sin(a));
    }
    count = segments;
    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    return vao;
}

unsigned int createStars(int n, int& count) {
    vector<float> v;
    unsigned int seed = 12345u;
    auto rnd = [&seed]() {
        seed = seed * 1664525u + 1013904223u;
        return (float)((seed >> 8) & 0xFFFFFF) / 16777216.0f;
    };
    for (int i = 0; i < n; ++i) {
        float z = rnd() * 2.0f - 1.0f;
        float a = rnd() * 2.0f * PI;
        float r = sqrt(std::max(0.0f, 1.0f - z * z));
        float size = 1.0f + 2.2f * rnd() * rnd();
        float bright = 0.35f + 0.65f * rnd();
        float row[5] = {r * cos(a), z, r * sin(a), size, bright};
        v.insert(v.end(), row, row + 5);
    }
    count = n;
    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    return vao;
}


void drawMesh(const Mesh& m) {
    glBindVertexArray(m.vao);
    glDrawElements(GL_TRIANGLES, m.count, GL_UNSIGNED_INT, 0);
}
