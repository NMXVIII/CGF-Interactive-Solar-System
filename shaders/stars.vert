#version 330 core
layout (location = 0) in vec3 aDir;
layout (location = 1) in vec2 aSizeBright;

out float vBright;

uniform mat4 view;
uniform mat4 projection;
uniform float pixelScale;

void main() {
    mat4 rotOnly = mat4(mat3(view));
    vec4 p = projection * rotOnly * vec4(aDir * 100.0, 1.0);
    gl_Position = p.xyww;
    gl_PointSize = aSizeBright.x * pixelScale;
    vBright = aSizeBright.y;
}
