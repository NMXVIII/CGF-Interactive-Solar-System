#pragma once
#include "core.h"
#include "bodies.h"



unsigned int loadTexture(const string& path, bool flip, bool report) {
    stbi_set_flip_vertically_on_load(flip);
    int w, h, n;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &n, 0);
    if (!data) {
        if (report) cerr << "[Texture] could not load " << path << endl;
        return 0;
    }
    GLenum format = n == 4 ? GL_RGBA : (n == 3 ? GL_RGB : GL_RED);
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    return id;
}

unsigned int loadBodyTexture(Body& b) {
    unsigned int t = loadTexture(b.textureFile, true, false);
    if (t) return t;
    if (!b.placeholderFile.empty()) {
        t = loadTexture(b.placeholderFile, true, false);
        if (t) {
            cerr << "[Texture] " << b.textureFile << " not found, using placeholder for " << b.name << endl;
            return t;
        }
    }
    cerr << "[Texture] no texture for " << b.name << ", using flat colour" << endl;
    return 0;
}
