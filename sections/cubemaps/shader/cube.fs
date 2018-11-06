#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D cube;

void main()
{             
  FragColor = texture(cube, TexCoords);
}
