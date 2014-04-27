#version 330

uniform vec3 light_position;
in vec4 position;

out vec4 outputColor;
void main()
{
    float lighting = max(0.2, min(1.0, 10/sqrt(length(position.xyz - light_position.xyz))));
    outputColor = vec4(lighting, lighting, lighting, 1.0f);
}
