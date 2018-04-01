#version 400 core
in vec4 ourColor;
in vec3 Normal;
in vec2 TexCoord;

out vec4 color;

uniform vec4 line_color;

void main()
{
	float x,y;
	x = fract(TexCoord.x*25.0);
	y = fract(TexCoord.y*25.0);

	// Draw a black and white grid.
	if (x > 0.93 || y > 0.93) 
	{
	color = line_color;
	}
	else
{
	discard;
	}
}