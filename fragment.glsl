#version 330

varying vec4 position;

out vec4 outputColor;
void main()
{
   outputColor = vec4(1.0f, 1.0 - 2.0*position.y, 1.0 - 2.0*position.y, 1.0f);
}
