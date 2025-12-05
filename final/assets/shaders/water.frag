#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

in float waveCalc;

uniform samplerCube skybox;
uniform vec3 viewPos;

void main()
{
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-viewDir, Normal);
    vec3 skyboxReflection = texture(skybox, reflectDir).rgb;

    float alpha = waveCalc * 0.2 + 0.7;
    vec3 waveColor = mix(vec3(0.1, 0.3, 0.8), skyboxReflection, .5);

    FragColor = vec4(waveColor, alpha);
}