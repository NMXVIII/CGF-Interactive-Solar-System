#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 planetColor;
uniform sampler2D planetTexture;
uniform bool useTexture;

void main() {
    vec3 baseColor = useTexture ? texture(planetTexture, TexCoords).rgb : planetColor;


    vec3 ambient = 0.25 * baseColor;
    
   
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * baseColor;
    
 
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.3) * spec;
    
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
