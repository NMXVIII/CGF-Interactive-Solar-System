#version 330 core
in vec2 UV;
out vec4 FragColor;
uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloomOn;
uniform float bloomStrength;
uniform float exposure;

vec3 softClip(vec3 c) {
    vec3 hi = 0.8 + 0.2 * (1.0 - exp(-(c - 0.8) / 0.2));
    return mix(c, hi, step(vec3(0.8), c));
}

void main() {
    vec3 c = texture(scene, UV).rgb;
    if (bloomOn) c += texture(bloomBlur, UV).rgb * bloomStrength;
    c *= exposure;
    FragColor = vec4(softClip(c), 1.0);
}
