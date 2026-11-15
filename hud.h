#pragma once
#include "core.h"
#include "bodies.h"
#include "assets.h"



struct TextRenderer {
    unsigned int vao = 0, vbo = 0, font = 0;
    vector<float> verts;

    void init() {
        font = loadTexture("textures/font.png", false, true);
        glBindTexture(GL_TEXTURE_2D, font);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void quad(float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1) {
        float q[24] = {x0, y0, u0, v0, x1, y0, u1, v0, x1, y1, u1, v1,
                       x0, y0, u0, v0, x1, y1, u1, v1, x0, y1, u0, v1};
        verts.insert(verts.end(), q, q + 24);
    }

    void flush(const Shader& s, bool solid, vec4 color) {
        if (verts.empty()) return;
        s.use();
        s.setVec2("screenSize", (float)gWidth, (float)gHeight);
        s.setVec4("color", color.r, color.g, color.b, color.a);
        s.setBool("solid", solid);
        s.setInt("fontTex", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, font);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(verts.size() / 4));
        verts.clear();
    }

    void rect(const Shader& s, float x, float y, float w, float h, vec4 color) {
        quad(x, y, x + w, y + h, 0, 0, 0, 0);
        flush(s, true, color);
    }

    void text(const Shader& s, const string& str, float x, float y, float scale, vec4 color) {
        const float cw = 8.0f * scale, ch = 16.0f * scale;
        float cx = x;
        for (char c : str) {
            if (c == '\n') { cx = x; y += ch; continue; }
            unsigned char u = (unsigned char)c;
            if (u < 32 || u > 126) u = '?';
            float u0 = (u % 16) / 16.0f, v0 = (u / 16) / 8.0f;
            quad(cx, y, cx + cw, y + ch, u0, v0, u0 + 1.0f / 16.0f, v0 + 1.0f / 8.0f);
            cx += cw;
        }
        flush(s, false, color);
    }
};

TextRenderer gText;

float textWidth(const string& s, float scale) { return (float)s.size() * 8.0f * scale; }



void drawHud(const Shader& textShader, float fps) {
   
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float s = std::max(1.0f, gHeight / 720.0f) * 1.0f;
    float lh = 16.0f * s;
    vec4 white(1, 1, 1, 1), grey(0.75f, 0.8f, 0.9f, 1), gold(1.0f, 0.85f, 0.35f, 1);
    gText.rect(textShader, 8 * s, 8 * s, 560 * s, 5 * lh + 8 * s, vec4(0, 0, 0, 0.55f));
    gText.text(textShader, kTitle, 14 * s, 12 * s, s, gold);
    gText.text(textShader, string("Team: ") + kTeam, 14 * s, 12 * s + lh, s, grey);
    char line[256];
    snprintf(line, sizeof(line), "Time x%g %s | Day %.0f | Bloom %s | Orbits %s | Lighting %s",
             kSpeeds[gSpeedIndex], gPaused ? "[PAUSED]" : "", gSimDays, gBloom ? "ON" : "OFF",
             gOrbits ? "ON" : "OFF", gLighting ? "ON" : "OFF");
    gText.text(textShader, line, 14 * s, 12 * s + 2 * lh, s, white);
    snprintf(line, sizeof(line), "FPS %.0f | Explored %d/%d bodies | Camera: %s", fps, (int)gVisited.size(),
             (int)gBodies.size(), gFollow && gSelected >= 0 ? "following" : "free orbit");
    gText.text(textShader, line, 14 * s, 12 * s + 3 * lh, s, white);
    gText.text(textShader, "H = help | T = restart tour", 14 * s, 12 * s + 4 * lh, s, grey);
    if (gVisited.size() >= gBodies.size()) {
        const char* done = "TOUR COMPLETE - every body visited!  (T = new tour)";
        float tw = textWidth(done, s * 1.5f);
        gText.rect(textShader, gWidth - tw - 34 * s, 14 * s, tw + 24 * s, 28 * s, vec4(0, 0, 0, 0.6f));
        gText.text(textShader, done, gWidth - tw - 22 * s, 18 * s, s * 1.5f, gold);
    }

    if (gSelected >= 0) {
        const Body& b = gBodies[gSelected];
        float pw = 520 * s, ph = 6 * lh + 12 * s;
        float px = gWidth - pw - 10 * s, py = gHeight - ph - 10 * s;
        gText.rect(textShader, px, py, pw, ph, vec4(0, 0, 0, 0.6f));
        string title = b.name;
        if (b.parent == 0) title += "  (planet)";
        else if (b.parent > 0) title += "  (moon of " + gBodies[b.parent].name + ")";
        else title += "  (star)";
        gText.text(textShader, title, px + 8 * s, py + 6 * s, s * 1.25f, gold);
        for (int i = 0; i < 3; ++i)
            gText.text(textShader, b.fact[i], px + 8 * s, py + 6 * s + lh * (1.6f + i * 1.0f), s, white);
        if (b.parent >= 0) {
            snprintf(line, sizeof(line), "Orbital period: %g Earth days (model distance %.1f)", b.periodDays, b.orbitRadius);
            gText.text(textShader, line, px + 8 * s, py + 6 * s + lh * 4.8f, s, grey);
        }
    }

    if (gHelp) {
        const char* help =
            "CONTROLS\n"
            "Mouse drag / arrows / A D : rotate camera\n"
            "Wheel / W S / + - : zoom\n"
            "Click or 0-8 / M / TAB : select Sun, planet, Moon, next\n"
            "F : follow on/off      R : reset to system view\n"
            "T : restart the exploration tour (visit all bodies)\n"
            "SPACE : pause          , . : slower / faster time\n"
            "B : bloom on/off (headline technique)\n"
            "O : orbits   N : lighting   P : screenshot\n"
            "J L I K U Y : move the light, BACKSPACE : back\n"
            "H or F1 : hide this help   ESC : quit";
        float hw = 440 * s, hh = 12 * lh + 10 * s;
        gText.rect(textShader, 8 * s, gHeight - hh - 8 * s, hw, hh, vec4(0, 0, 0, 0.6f));
        gText.text(textShader, help, 14 * s, gHeight - hh - 2 * s, s, white);
    }
    glDisable(GL_BLEND);

}

bool saveScreenshot(const string& path) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    vector<unsigned char> px((size_t)gWidth * gHeight * 3), flip(px.size());
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, gWidth, gHeight, GL_RGB, GL_UNSIGNED_BYTE, px.data());
    for (int y = 0; y < gHeight; ++y)
        memcpy(&flip[(size_t)y * gWidth * 3], &px[(size_t)(gHeight - 1 - y) * gWidth * 3], (size_t)gWidth * 3);
    return writePNG(path, gWidth, gHeight, flip);
}
