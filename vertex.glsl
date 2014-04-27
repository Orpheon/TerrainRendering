#version 330

layout(location = 0) in vec4 vertex_position;
uniform mat4 matrix_stack;
uniform vec3 light_position;

out vec4 position;

void main()
{
    position = vertex_position;
    position.y = 5.0*sin(position.x/5.0) + 5.0*cos(position.z/5.0);
    gl_Position = matrix_stack * position;
}
