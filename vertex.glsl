#version 330

layout(location = 0) in vec4 vertex_position;
uniform mat4 matrix_stack;

varying vec4 position;

void main()
{
    position = vertex_position;
    gl_Position = matrix_stack * position;
}
