#version 330 core
in float vBright;
out vec4 FragColor;
void main() {
    vec2 d = gl_PointCoord - vec2(0.5);
    float falloff = smoothstep(0.5, 0.0, length(d));
    FragColor = vec4(vec3(0.85, 0.9, 1.0) * vBright * falloff, 1.0);
}
