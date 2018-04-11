#version 400 core
in vec4 ourColor;
in vec3 Normal;
in vec3 TexCoord;

out vec4 color;

uniform samplerCube ourTexture;

void main()
{
	color = texture(ourTexture, TexCoord);
}