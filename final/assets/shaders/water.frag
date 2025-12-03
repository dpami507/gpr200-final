#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

in float waveCalc;

uniform sampler2D texture1;
uniform vec2 uvTiling;
uniform vec3 color;

void main()
{
    vec2 uv = TexCoord;
    uv.x *= uvTiling.x;
    uv.y *= uvTiling.y;
    uv = fract(uv);

    vec4 tex = texture(texture1, uv);

    float alpha = waveCalc * 0.1 + 0.5;
    vec3 waterColor = vec3(0, 64, 255);

    vec3 result = (vec3(tex) * (color / 255));
    FragColor = vec4(result * (waterColor / 255), alpha);
}