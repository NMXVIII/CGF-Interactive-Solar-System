#version 330 core
in vec2 UV;
out vec4 FragColor;

uniform sampler2D fontTex;
uniform vec4 color;
uniform bool solid;

void main() {
    if (solid) {
        FragColor = color;
    } else {
        float a = texture(fontTex, UV).r;
        FragColor = vec4(color.rgb, color.a * a);
    }
}
