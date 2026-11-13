#version 330 core
in vec2 UV;
out vec4 FragColor;
uniform sampler2D scene;
uniform float threshold;
void main() {
    vec3 c = texture(scene, UV).rgb;
    float lum = dot(c, vec3(0.2126, 0.7152, 0.0722));
    float k = max(lum - threshold, 0.0) / max(lum, 0.0001);
    FragColor = vec4(c * k, 1.0);
}
