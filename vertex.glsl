#version 330

layout(location = 0) in vec4 vertex_position;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;

varying vec4 position;

void main()
{
    position = vertex_position;
    gl_Position = view_matrix * projection_matrix * vertex_position;
}
