#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 sunColor;
uniform sampler2D sunTexture;
uniform bool useTexture;
uniform float intensity;

void main() {
    vec3 c = useTexture ? texture(sunTexture, TexCoords).rgb : sunColor;
    FragColor = vec4(c * intensity, 1.0);
}
