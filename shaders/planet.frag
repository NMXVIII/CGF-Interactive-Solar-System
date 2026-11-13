#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 planetColor;
uniform sampler2D planetTexture;
uniform bool useTexture;
uniform float specularStrength;
uniform float ambientStrength;
uniform bool lightingEnabled;
uniform float alpha;
uniform bool ringMode;

void main() {
    vec4 tex = useTexture ? texture(planetTexture, TexCoords) : vec4(planetColor, 1.0);
    vec3 baseColor = tex.rgb;
    float outAlpha = alpha;

    if (ringMode) {
        outAlpha = tex.a * alpha;
        if (outAlpha < 0.02) discard;
    }

    vec3 norm = normalize(Normal);
    if (!gl_FrontFacing) norm = -norm;

    vec3 toLight = lightPos - FragPos;
    float dist = length(toLight);
    vec3 lightDir = toLight / dist;
    float attenuation = 1.0 / (1.0 + 0.0008 * dist * dist);

    vec3 result;
    if (lightingEnabled) {
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), 48.0) * step(0.0, diff);
        vec3 ambient = ambientStrength * baseColor;
        vec3 diffuse = diff * baseColor * lightColor;
        vec3 specular = specularStrength * spec * lightColor;
        result = ambient + attenuation * (diffuse + specular);
    } else {
        result = baseColor;
    }
    FragColor = vec4(result, outAlpha);
}
