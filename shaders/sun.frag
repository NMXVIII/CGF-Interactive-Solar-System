#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 sunColor;
uniform sampler2D sunTexture;
uniform bool useTexture;

void main() {
    if (useTexture) {
        FragColor = texture(sunTexture, TexCoords);
    } else {
        FragColor = vec4(sunColor, 1.0);
    }
}
