#version 330 core
in vec4 ourColor;
in vec3 Normal;
in vec2 TexCoord;

out vec4 color;

uniform bool has_material_color;
uniform vec4 material_color;
uniform float material_alpha;
uniform bool alpha_interpolation;
uniform bool color_interpolation;
uniform vec3 color_to_show;
uniform float alpha_percentage;
uniform bool has_texture;
uniform sampler2D ourTexture;

void main()
{
	if(has_texture)
	{
		color = texture(ourTexture, TexCoord);
		color.rgb = color.rgb + material_color.rgb;
	}
	else if(has_material_color)
		color = material_color;
	else
		color = ourColor;
   if(color.a < 0.1f)
		discard;
	if (material_alpha != 1)
		color.a = material_alpha;
	if(alpha_interpolation)
		color.a = alpha_percentage;
}