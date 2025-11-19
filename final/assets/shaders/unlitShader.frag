#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform float lightStrength;
uniform vec3 lightColor;

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

    vec3 result = lightStrength * lightColor * (vec3(tex) * (color / 255));
    FragColor = vec4(result, 1.0);
}