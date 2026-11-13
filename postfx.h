#pragma once
#include "core.h"

// ---------------------------------------------------------------- framebuffers

struct Targets {
    unsigned int msFbo = 0, msColor = 0, msDepth = 0;
    unsigned int hdrFbo = 0, hdrTex = 0;
    unsigned int pingFbo[2] = {0, 0}, pingTex[2] = {0, 0};
    int w = 0, h = 0, bw = 0, bh = 0;
};

Targets gT;

void destroyTargets() {
    if (gT.msFbo) {
        glDeleteFramebuffers(1, &gT.msFbo);
        glDeleteRenderbuffers(1, &gT.msColor);
        glDeleteRenderbuffers(1, &gT.msDepth);
        glDeleteFramebuffers(1, &gT.hdrFbo);
        glDeleteTextures(1, &gT.hdrTex);
        glDeleteFramebuffers(2, gT.pingFbo);
        glDeleteTextures(2, gT.pingTex);
    }
    gT = Targets();
}

unsigned int makeColorTexture(int w, int h) {
    unsigned int t;
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return t;
}

void checkFbo(const char* name) {
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        cerr << "[FBO] " << name << " is not complete (status 0x" << hex << status << dec << ")" << endl;
}

void createTargets(int w, int h) {
    destroyTargets();
    gT.w = w;
    gT.h = h;
    gT.bw = std::max(w / 2, 1);
    gT.bh = std::max(h / 2, 1);

    glGenFramebuffers(1, &gT.msFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gT.msFbo);
    glGenRenderbuffers(1, &gT.msColor);
    glBindRenderbuffer(GL_RENDERBUFFER, gT.msColor);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA16F, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, gT.msColor);
    glGenRenderbuffers(1, &gT.msDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, gT.msDepth);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT24, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gT.msDepth);
    checkFbo("multisample scene");

    glGenFramebuffers(1, &gT.hdrFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gT.hdrFbo);
    gT.hdrTex = makeColorTexture(w, h);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gT.hdrTex, 0);
    checkFbo("resolved scene");

    glGenFramebuffers(2, gT.pingFbo);
    for (int i = 0; i < 2; ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, gT.pingFbo[i]);
        gT.pingTex[i] = makeColorTexture(gT.bw, gT.bh);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gT.pingTex[i], 0);
        checkFbo("bloom ping-pong");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


// resolve the 4x MSAA scene, run bright pass + separable blur, composite to the window
void resolveAndBloom(Shader& brightShader, Shader& blurShader, Shader& compositeShader, unsigned int emptyVAO) {
    // ---- resolve to the HDR texture
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gT.msFbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gT.hdrFbo);
    glBlitFramebuffer(0, 0, gWidth, gHeight, 0, 0, gWidth, gHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // ---- bloom: bright pass + separable gaussian blur (half resolution)
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glBindVertexArray(emptyVAO);
    unsigned int bloomTex = gT.pingTex[0];
    if (gBloom) {
        glViewport(0, 0, gT.bw, gT.bh);
        glBindFramebuffer(GL_FRAMEBUFFER, gT.pingFbo[0]);
        brightShader.use();
        brightShader.setInt("scene", 0);
        brightShader.setFloat("threshold", 0.75f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gT.hdrTex);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        blurShader.use();
        blurShader.setInt("image", 0);
        bool horizontal = true;
        for (int i = 0; i < 10; ++i) {
            int dst = horizontal ? 1 : 0;
            glBindFramebuffer(GL_FRAMEBUFFER, gT.pingFbo[dst]);
            blurShader.setBool("horizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, gT.pingTex[1 - dst]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            horizontal = !horizontal;
        }
        bloomTex = gT.pingTex[0];
    }

    // ---- composite to the window
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, gWidth, gHeight);
    compositeShader.use();
    compositeShader.setInt("scene", 0);
    compositeShader.setInt("bloomBlur", 1);
    compositeShader.setBool("bloomOn", gBloom);
    compositeShader.setFloat("bloomStrength", 1.5f);
    compositeShader.setFloat("exposure", 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gT.hdrTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bloomTex);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glActiveTexture(GL_TEXTURE0);

}
