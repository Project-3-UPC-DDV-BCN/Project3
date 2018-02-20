#version 330 core
in vec4 ourColor;
in vec3 Normal;
in vec2 TexCoord;

out vec4 color;

uniform bool has_material_color;
uniform vec4 material_color;
uniform bool has_texture;
uniform sampler2D ourTexture;

void main()
{
	if(has_texture)
		color = texture(ourTexture, TexCoord);
	else if(has_material_color)
		color = material_color;
	else
		color = ourColor;
}